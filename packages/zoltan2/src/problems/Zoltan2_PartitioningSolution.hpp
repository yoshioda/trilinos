// @HEADER
// ***********************************************************************
//
//                Copyright message goes here.   TODO
//
// ***********************************************************************
// @HEADER
//

/*! \file Zoltan2_PartitioningSolution.hpp
    \brief Defines the PartitioningSolution class.
*/

#ifndef _ZOLTAN2_PARTITIONINGSOLUTION_HPP_
#define _ZOLTAN2_PARTITIONINGSOLUTION_HPP_

#include <Zoltan2_IdentifierMap.hpp>
#include <Zoltan2_Solution.hpp>

#include <cmath>
#include <algorithm>
#include <vector>

namespace Zoltan2 {

/*! \brief A PartitioningSolution is a solution to a partitioning problem.

    It is initialized by a PartitioningProblem,
    written to by an algorithm, and may be read by the user or by
    a data migration routine in an input adapter.
    
    \todo handle more metrics
    \todo save an RCB tree, so it can be used in repartitioning, and
                supplied to the caller.
    \todo doxyfy the comments in this file.
*/

template <typename User_t>
  class PartitioningSolution : public Solution
{
public:

  typedef typename InputTraits<User_t>::gno_t gno_t;
  typedef typename InputTraits<User_t>::lno_t lno_t;
  typedef typename InputTraits<User_t>::gid_t gid_t;

/*! \brief Constructor when part sizes are not supplied.
 *
 *   The Solution constructor may require global communication.
 *   The rest of the Solution methods do not.
 *
 *    \param env the environment for the application
 *    \param comm the communicator for the problem associated with this solution
 *    \param idMap  the IdentifierMap corresponding to the solution
 *    \param userWeightDim  the number of weights supplied by the application
 *                         for each object.
 *
 *   It is possible that part sizes were supplied on other processes,
 *   so this constructor does do a check to see if part sizes need
 *   to be globally calculated.
 */
 
  PartitioningSolution( RCP<const Environment> &env,
    RCP<const Comm<int> > &comm,
    RCP<const IdentifierMap<User_t> > &idMap,
    int userWeightDim);

/*! \brief Constructor when part sizes are supplied.
 *
 *   The Solution constructor may require global communication.
 *   The rest of the Solution methods do not.
 *
 *    \param env the environment for the application
 *    \param comm the communicator for the problem associated with this solution
 *    \param idMap  the IdentifierMap corresponding to the solution
 *    \param userWeightDim  the number of weights supplied by the application
 *                         for each object.
 *    \param reqPartIds  reqPartIds[i] is a list of
 *          of part numbers for weight dimension i.
 *    \param reqPartSizes  reqPartSizes[i] is the list
 *          of part sizes for weight i corresponding to parts in 
 *          reqPartIds[i]
 *
 *   If reqPartIds[i].size() and reqPartSizes[i].size() are zero for 
 *   all processes, it is assumed that part sizes for weight 
 *   dimension "i" are uniform.
 *
 *   If across the application there are some part numbers that are not
 *   included in the reqPartIds lists, then those part sizes are assumed
 *   to be the average of the supplied part sizes.
 *
 *   \todo handle errors that may arise - like duplicate part numbers
 */

  PartitioningSolution( RCP<const Environment> &env,
    RCP<const Comm<int> > &comm,
    RCP<const IdentifierMap<User_t> > &idMap,
    int userWeightDim, ArrayView<ArrayRCP<partId_t> > reqPartIds,
    ArrayView<ArrayRCP<float> > reqPartSizes );
  
  ////////////////////////////////////////////////////////////////////
  // Information that the algorithm may wish to query.
  
/*! \brief Returns the global number of parts.
 */
  size_t getGlobalNumberOfParts() const { return nGlobalParts_; }
  
/*! \brief Returns the number of parts (or fraction of one part) to 
            be assigned to this process.
 */
  float getLocalNumberOfParts() const { return nLocalParts_; }
  
/*! \brief Is the part-to-process distribution is one-to-one.
     \return true if Process p owns part p for all p, and false if the part
                  to process distribution is more complex.

   If this is true, then getPartDistribution() and getProcDistribution()
   return NULL pointers.  If either of the latter two methods is non-NULL,
   then this method returns false.
 */

  bool oneToOnePartDistribution() const { return onePartPerProc_; }
  
/*! \brief Return a distribution by part.

    \return If any parts are
       divided across processes, then a mapping \c A is returned. 
     \c A such that \c A[i] is the lowest numbered process 
       owning part \c i.  The length of the array is one greater than the 
       global number of parts.
       The value of the last element is the global number of processes.

     Parts are only divided across processes if there are fewer parts
     than processes and the caller did not define "num_local_parts" for
     each process.  In this case, parts are divided somewhat evenly
     across the processes.  This situation is more likely to arise in
     Zoltan2 algorithms than in user applications.

   If either oneToOnePartDistribution() is true or getProcDistribution() is
   non-NULL, then this method returns a NULL pointer.  The three are mutually 
   exclusive and collective exhaustive.
 */
  const int *getPartDistribution() const { return &partDist_[0]; }
  
/*! \brief Return a distribution by process.

    \return If the mapping of parts to processes is not one-to-one, and
      if parts are not divided across processes, then the mapping
      \c A is returned. \c A such that \c A[i] is the first part 
      assigned to process \c i.  (Parts are assigned sequentially to processes.)
      However if \c A[i+1] is equal to \c A[i], there
      are no parts assigned to process \c i.  The length of the array is
      one greater than the number of processes.  The last element of
      the array is the global number of parts.

    If the mapping is one-to-one, or if parts are divided across processes,
    then this method returns NULL pointer, and either 
    oneToOnePartDistribution() or getPartDistribution() describes the mapping.
 */
  const partId_t *getProcDistribution() const { return &procDist_[0]; }
  
/*! \brief Determine if balancing criteria (weight dimension) has uniform
                parts.
    \param idx   A value from 0 to one less than the number of weights per 
                   object.
    \return true if part sizes are uniform for this criteria.
 */
  bool criteriaHasUniformPartSizes(int idx) const { return pSizeUniform_[idx];}
  
/*! \brief Get the size for a given weight dimension and a given part. 

    \param idx   A value from 0 to one less than the number of weights per 
                       object.
    \param part  A value from 0 to one less than the global number of parts 
                   to be computed
    \return   The size for that part.  Part sizes for a given weight 
                    dimension sum to 1.0.

      \todo It would be useful to algorithms to get the sum of
           part sizes from a to b, or the sum or a list of parts.
 */
  float getCriteriaPartSize(int idx, partId_t part) const { 
    if (pSizeUniform_[idx]) 
      return 1.0 / nGlobalParts_;
    else if (pCompactIndex_[idx].size())
      return pSize_[idx][pCompactIndex_[idx][part]];
    else
      return pSize_[idx][part];
  }

  ////////////////////////////////////////////////////////////////////
  // Method used by the algorithm to set results.

  /*! \brief The algorithm uses setParts to set the solution.
   *
   *   \param gnoList  A view of the list of global numbers that was
   *     supplied to the algorithm by the model. If the model used
   *     the application's global IDs as our internal global numbers,
   *     this may be a view of the global IDs in the application's memory.
   *     Otherwise it is a view of the internal global numbers created
   *     by the IdentifierMap, or a re-ordered set of global numbers
   *     created during the course of the algorithm.
   *
   *   \param partList  The part assigned to gnoList[i] by the algorithm
   *      should be in partList[i].  The partList is allocated and written
   *      by the algorithm. We save an RCP to the part list.
   *
   *   \param  imbalance  The algorithm computes one imbalance for each
   *      weight dimension.  It allocates and writes the imbalance array.
   *      We copy the metrics.
   *
   *  \todo If the algorithm will always provide the partList in the
   *     same order as the gnos listed in the model, then the gnoList
   *     can be omitted.
   */
  
  void setParts(ArrayView<const gno_t> gnoList, ArrayRCP<partId_t> partList,
    ArrayRCP<float> &imbalance);
  
  ////////////////////////////////////////////////////////////////////
  // Results that may be queried by the user or by migration methods.
  // We return raw pointers so users don't have to learn about our
  // pointer wrappers.

  /*! \brief Return the communicator associated with the solution.
   */
  const RCP<const Comm<int> > &getCommunicator() const { return comm_;}

  /*! \brief Returns the local number of Ids.
   */
  size_t getLocalNumberOfIds() const { return gids_.size(); }

  /*! \brief Returns the user's global ID list.
   */
  const gid_t *getIdList() const { return gids_.getRawPtr(); }

  /*! \brief Returns the part list corresponding to the global ID list.
   */
  const zoltan2_partId_t *getPartList() const { return parts_.getRawPtr();}

  /*! \brief Returns the process list corresponding to the global ID list.
      \return The return value is a NULL pointer if part IDs are
                synonomous with process IDs.
   */
  const int *getProcList() const { return procs_.getRawPtr();}

  /*! \brief Returns the imbalance of the solution.
   *      \todo add more metrics
   */
  const float *getImbalance() const { return imbalance_.getRawPtr(); }

  /*! \brief Get the parts belonging to a process.
   *  \param procId a process rank
   *  \param numParts on return will be set the number of parts belonging
   *                    to the process.
   *  \param partMin on return will be set to minimum part number
   *  \param partMax on return will be set to maximum part number
   *  
   * Normally \c numParts is at least one. But if there are more processes 
   * than parts, one of two things can happen.  Either there are processes
   * with no parts, and so \c numParts will be zero, or a part may be
   * split across more than one process, in which \c numParts will
   * be non-zero but less than 1.
   *
   * In the latter case, \c numParts is 1.0 divided by the number of
   * processes that share the part.
   */

  void getPartsForProc(int procId, double &numParts, partId_t &partMin, 
    partId_t &partMax) const
  {
    env_->localInputAssertion(__FILE__, __LINE__, "invalid process id",
      procId >= 0 && procId < comm_->getSize(), BASIC_ASSERTION);

    procToParts(procId, numParts, partMin, partMax);
  }

  /*! \brief Get the processes containing a part.
   *  \param partId a part number from 0 to one less than the global number
   *                of parts.
   *  \param procMin on return will be set to minimum proc number
   *  \param procMax on return will be set to maximum proc number
   *  
   * Normally \c procMin and \c procMax are the same value and a part
   * is assigned to one process.  But if there are more processes than
   * parts, it's possible that a part will be divided across more than
   * one process.
   */
  void getProcsForPart(partId_t partId, int &procMin, int &procMax) const
  {
    env_->localInputAssertion(__FILE__, __LINE__, "invalid part id",
      partId >= 0 && partId < nGlobalParts_, BASIC_ASSERTION);

    partToProcs(partId, procMin, procMax);
  }

private:
  void procToParts(int procId, double &numParts, partId_t &partMin, 
    partId_t &partMax) const;

  void partToProcs(partId_t partId, int &procMin, int &procMax) const;

  void setPartDistribution();

  void setPartSizes(ArrayView<ArrayRCP<partId_t> > reqPartIds,
    ArrayView<ArrayRCP<float> > reqPartSizes);

  void computePartSizes(int wdim, ArrayView<partId_t> ids, 
    ArrayView<float> sizes);

  void broadcastPartSizes(int wdim);

  RCP<const Environment> env_;             // has application communicator
  RCP<const Comm<int> > comm_;             // the problem communicator
  RCP<const IdentifierMap<User_t> > idMap_;

  gno_t nGlobalParts_; // the global number of parts
  float nLocalParts_;  // Zero, fraction of one part, or number of whole parts
  int weightDim_;      // if user has no weights, this is 1

  // If process p is to be assigned part p for all p, then onePartPerProc_ 
  // is true. Otherwise it is false, and either procDist_ or partDist_
  // describes the allocation of parts to processes.
  //
  // If parts are never split across processes, then procDist_ is defined
  // as follows:
  //
  //   partId              = procDist_[procId]
  //   partIdNext          = procDist_[procId+1]
  //   globalNumberOfParts = procDist_[numProcs]
  //
  // meaning that the parts assigned to process procId range from
  // [partId, partIdNext).  If partIdNext is the same as partId, then
  // process procId has no parts.
  //
  // If the number parts is less than the number of processes, and the
  // user did not specify "num_local_parts" for each of the processes, then
  // parts are split across processes, and partDist_ is defined rather than
  // procDist_.
  //
  //   procId              = partDist_[partId]
  //   procIdNext          = partDist_[partId+1]
  //   globalNumberOfProcs = partDist_[numParts]
  //
  // which implies that the part partId is shared by processes in the
  // the range [procId, procIdNext).
  //
  // We use std::vector so we can use upper_bound algorithm
  
  bool             onePartPerProc_;   // either this is true...
  std::vector<int>      partDist_;      // or this is defined ...
  std::vector<partId_t> procDist_;      // or this is defined.

  // In order to minimize the storage required for part sizes, we
  // have three different representations.
  //
  // If the part sizes for weight dimension w are all the same, then:
  //    pSizeUniform_[w] = true
  //    pCompactIndex_[w].size() = 0
  //    pSize_[w].size() = 0
  //
  // and the size for part p is 1.0 / nparts.
  //
  // If part sizes differ for each part in weight dimension w, but there
  // are no more than 64 distinct sizes:
  //    pSizeUniform_[w] = false
  //    pCompactIndex_[w].size() = number of parts
  //    pSize_[w].size() = number of different sizes
  //
  // and the size for part p is pSize_[pCompactIndex_[p]].
  //
  // If part sizes differ for each part in weight dimension w, and there
  // are more than 64 distinct sizes:
  //    pSizeUniform_[w] = false
  //    pCompactIndex_[w].size() = 0
  //    pSize_[w].size() = nparts
  //
  // and the size for part p is pSize_[p].
  //
  // NOTE: If we expect to have similar cases, i.e. a long list of scalars
  //   where it is highly possible that just a few unique values appear,
  //   then we may want to make this a class.  The advantage is that we
  //   save a long list of 1-byte indices instead of a long list of scalars.

  ArrayRCP<bool> pSizeUniform_;
  ArrayRCP<ArrayRCP<unsigned char> > pCompactIndex_;
  ArrayRCP<ArrayRCP<float> > pSize_;

  ////////////////////////////////////////////////////////////////
  // The algorithm sets these values upon completion.  (The
  // algorithm gives gnos to the solution, which converts them
  // to the user's gids.)

  ArrayRCP<const gid_t>  gids_;   // User's global IDs 
  ArrayRCP<partId_t> parts_;      // part number assigned to gids_[i]
  ArrayRCP<float> imbalance_;     // weightDim_ imbalance measures

  ////////////////////////////////////////////////////////////////
  // The solution calculates this from the part assignments,
  // unless onePartPerProc_.

  ArrayRCP<int> procs_;       // process rank assigned to gids_[i]
};

////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////

template <typename User_t>
  PartitioningSolution<User_t>::PartitioningSolution(
    RCP<const Environment> &env, 
    RCP<const Comm<int> > &comm,
    RCP<const IdentifierMap<User_t> > &idMap, int userWeightDim)
    : env_(env), comm_(comm), idMap_(idMap),
      nGlobalParts_(0), nLocalParts_(0), weightDim_(),
      onePartPerProc_(false), partDist_(), procDist_(), 
      pSizeUniform_(), pCompactIndex_(), pSize_(),
      gids_(), parts_(), imbalance_(), procs_()
{
  weightDim_ = (userWeightDim ? userWeightDim : 1); 

  setPartDistribution();

  // We must call setPartSizes() because part sizes may have
  // been provided by the user on other processes.

  ArrayRCP<partId_t> *noIds = new ArrayRCP<partId_t> [weightDim_];
  ArrayRCP<float> *noSizes = new ArrayRCP<float> [weightDim_];
  ArrayRCP<ArrayRCP<partId_t> > ids(noIds, 0, weightDim_, true);
  ArrayRCP<ArrayRCP<float> > sizes(noSizes, 0, weightDim_, true);

  setPartSizes(ids.view(0, weightDim_), sizes.view(0, weightDim_));
}

template <typename User_t>
  PartitioningSolution<User_t>::PartitioningSolution(
    RCP<const Environment> &env, 
    RCP<const Comm<int> > &comm,
    RCP<const IdentifierMap<User_t> > &idMap, int userWeightDim,
    ArrayView<ArrayRCP<partId_t> > reqPartIds, 
    ArrayView<ArrayRCP<float> > reqPartSizes)
    : env_(env), comm_(comm), idMap_(idMap),
      nGlobalParts_(0), nLocalParts_(0), weightDim_(),
      onePartPerProc_(false), partDist_(), procDist_(), 
      pSizeUniform_(), pCompactIndex_(), pSize_(),
      gids_(), parts_(), imbalance_(), procs_()
{
  weightDim_ = (userWeightDim ? userWeightDim : 1); 

  setPartDistribution();

  setPartSizes(reqPartIds, reqPartSizes);
}

template <typename User_t>
  void PartitioningSolution<User_t>::setPartDistribution()
{
  int rank = env_->myRank_;
  int nprocs = env_->numProcs_;
  
  // Did the caller define num_global_parts and/or num_local_parts?

  size_t haveGlobalNumParts=0, haveLocalNumParts=0;

  const ParameterList *params = NULL;
  if (env_->hasPartitioningParameters()){
    const ParameterList &allParams = env_->getParameters();
    const ParameterList &partitioningParams = allParams.sublist("partitioning");
    params = &partitioningParams;

    const double *entry1 = params->getPtr<double>(string("num_global_parts"));
    const double *entry2 = params->getPtr<double>(string("num_local_parts"));
  
    if (entry1){
      haveGlobalNumParts = 1;
      double val = *entry1;
      nGlobalParts_ = size_t(val);
    }
  
    if (entry2){
      haveLocalNumParts = 1;
      double val = *entry2;
      nLocalParts_ = float(val);
    }
  }

  size_t vals[4] = {haveGlobalNumParts, haveLocalNumParts, 
    nGlobalParts_, size_t(nLocalParts_)};
  size_t reducevals[4];

  try{
    reduceAll<int, size_t>(*comm_, Teuchos::REDUCE_SUM, 4, vals, reducevals);
  }
  Z2_THROW_OUTSIDE_ERROR(*env_);

  size_t sumHaveGlobal = reducevals[0];
  size_t sumHaveLocal = reducevals[1];
  size_t sumGlobal = reducevals[2];
  size_t sumLocal = reducevals[3];

  env_->localInputAssertion(__FILE__, __LINE__,
    "Either all procs specify num_global/local_parts or none do",
    (sumHaveGlobal == 0 || sumHaveGlobal == nprocs) &&
    (sumHaveLocal == 0 || sumHaveLocal == nprocs), 
    BASIC_ASSERTION);

  if (sumHaveGlobal == 0 && sumHaveLocal == 0){
    onePartPerProc_ = true;   // default if user did not specify
    nGlobalParts_ = nprocs;
    nLocalParts_ = 1;
    return;
  }

  if (sumHaveGlobal == nprocs){
    vals[0] = nGlobalParts_;
    vals[1] = size_t(nLocalParts_);
    try{
      reduceAll<int, size_t>(*comm_, Teuchos::REDUCE_MAX, 2, vals, reducevals);
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    size_t maxGlobal = reducevals[0];
    size_t maxLocal = reducevals[1];

    env_->localInputAssertion(__FILE__, __LINE__,
      "Value for num_global_parts is different on different processes.",
      maxGlobal * nprocs == sumGlobal, BASIC_ASSERTION);

    if (sumLocal){
      env_->localInputAssertion(__FILE__, __LINE__,
        "Sum of num_local_parts does not equal requested num_global_parts",
        sumLocal == nGlobalParts_, BASIC_ASSERTION);

      if (sumLocal == nprocs && maxLocal == 1){
        onePartPerProc_ = true;   // user specified one part per proc
        return;
      }
    }
    else{
      if (maxGlobal == nprocs){
        onePartPerProc_ = true;   // user specified num parts is num procs
        nLocalParts_ = 1;
        return;
      }
    }
  }

  // If we are here, we do not have #parts == #procs.

  if (sumHaveLocal == nprocs){
    //
    // We will go by the number of local parts specified.
    //

    try{
      procDist_.resize(nprocs+1);
    }
    catch (std::exception &e){
      throw(std::bad_alloc());
    }

    int *procArray = &procDist_[0];

    try{
      partId_t tmp = partId_t(nLocalParts_);
      gatherAll<int, partId_t>(*comm_, 1, &tmp, nprocs, procArray + 1); 
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    procArray[0] = 0;

    for (int proc=0; proc < nprocs; proc++)
      procArray[proc+1] += procArray[proc];

    nGlobalParts_ = procArray[nprocs];
  }
  else{
    //
    // We will allocate global number of parts to the processes.
    //
    double fParts = nGlobalParts_;
    double fProcs = nprocs;

    if (fParts < fProcs){

      try{
        partDist_.resize(size_t(fParts+1));
      }
      catch (std::exception &e){
        throw(std::bad_alloc());
      }

      int *partArray = &partDist_[0];

      double each = floor(fProcs / fParts);
      double extra = fmod(fProcs, fParts);
      partDist_[0] = 0;     

      for (partId_t part=0; part < nGlobalParts_; part++){
        int numOwners = int(each + ((part<extra) ? 1 : 0));
        partArray[part+1] = partArray[part] + numOwners;
        if ((nLocalParts_ == 0) && (partArray[part+1] > rank))
          nLocalParts_ = 1.0 / numOwners;
      }

      env_->globalBugAssertion(__FILE__, __LINE__, "#parts != #procs", 
        partDist_[nGlobalParts_] == nprocs, COMPLEX_ASSERTION, comm_);
    }
    else if (fParts > fProcs){ 

      try{
        procDist_.resize(size_t(fProcs+1));
      }
      catch (std::exception &e){
        throw(std::bad_alloc());
      }

      int *procArray = &procDist_[0];

      double each = floor(fParts / fProcs);
      double extra = fmod(fParts, fProcs);
      procArray[0] = 0;     

      for (int proc=0; proc < nprocs; proc++){
        partId_t numParts = partId_t(each + ((proc<extra) ? 1 : 0));
        procArray[proc+1] = procArray[proc] + numParts;
        if (proc == rank)
            nLocalParts_ = numParts;
      }

      env_->globalBugAssertion(__FILE__, __LINE__, "#parts != #procs", 
        procDist_[nprocs] == nGlobalParts_, COMPLEX_ASSERTION, comm_);
    }
    else{
      env_->globalBugAssertion(__FILE__, __LINE__, 
        "should never get here", 1, COMPLEX_ASSERTION, comm_);
    }
  }
}

template <typename User_t>
  void PartitioningSolution<User_t>::setPartSizes(
    ArrayView<ArrayRCP<partId_t> > ids, ArrayView<ArrayRCP<float> > sizes)
{
  int wdim = weightDim_;
  bool fail=false;
  partId_t *counts = new partId_t [wdim*2];

  fail = ((ids.size() != wdim) || (sizes.size() != wdim));

  for (int w=0; !fail && w < wdim; w++){
    counts[w] = ids[w].size();
    if (ids[w].size() != sizes[w].size()) fail=true;
  }

  env_->globalBugAssertion(__FILE__, __LINE__, "bad argument arrays", fail==0, 
    COMPLEX_ASSERTION, comm_);

  // Are all part sizes the same?  This is the common case.

  ArrayRCP<float> *emptySizes= new ArrayRCP<float> [wdim];
  pSize_ = arcp(emptySizes, 0, wdim);

  ArrayRCP<unsigned char> *emptyIndices= new ArrayRCP<unsigned char> [wdim];
  pCompactIndex_ = arcp(emptyIndices, 0, wdim);

  bool *info = new bool [wdim];
  pSizeUniform_ = arcp(info, 0, wdim);
  for (int w=0; w < wdim; w++)
    pSizeUniform_[w] = true;

  if (nGlobalParts_ == 1){   
    return;   // there's only one part in the whole problem
  }

  try{
    reduceAll<int, partId_t>(*comm_, Teuchos::REDUCE_MAX, wdim, counts, 
      counts + wdim);
  }
  Z2_THROW_OUTSIDE_ERROR(*env_);

  bool zero = true;

  for (int w=0; w < wdim; w++)
    if (counts[wdim+w] > 0){
      zero = false;
      pSizeUniform_[w] = false;
    }

  if (zero) // Part sizes for all criteria are uniform.
    return; 

  // Compute the part sizes for criteria for which part sizes were
  // supplied.  Normalize for each criteria so part sizes sum to one.

  int nprocs = comm_->getSize();
  int rank = comm_->getRank();
  Array<long> sendCount(nprocs, 0);
  ArrayRCP<long> recvCount;
  ArrayRCP<float> recvSizes;
  ArrayRCP<partId_t> recvIds;

  for (int w=0; w < wdim; w++){
    if (pSizeUniform_[w]) continue;
    
    // Send all ids and sizes to rank 0

    long len = ids[w].size();
    sendCount[0] = len;

    try{
      AlltoAllv<partId_t, long>(*comm_, *env_, ids[w].view(0, len),
        sendCount.view(0, nprocs), recvIds, recvCount);
    }
    Z2_FORWARD_EXCEPTIONS

    try{
      AlltoAllv<float, long>(*comm_, *env_, sizes[w].view(0, len),
        sendCount.view(0, nprocs), recvSizes, recvCount);
    }
    Z2_FORWARD_EXCEPTIONS

    if (rank == 0){
      try{
        partId_t numVals = recvIds.size();
        computePartSizes(w, recvIds.view(0,numVals), recvSizes.view(0,numVals));
      }
      Z2_FORWARD_EXCEPTIONS
    }

    broadcastPartSizes(w);
  } 
}

template <typename User_t>
  void PartitioningSolution<User_t>::broadcastPartSizes(int wdim)
{
  env_->localBugAssertion(__FILE__, __LINE__, "preallocations", 
    pSize_.size()>wdim && 
    pSizeUniform_.size()>wdim && pCompactIndex_.size()>wdim, 
    COMPLEX_ASSERTION);

  int rank = env_->myRank_;
  int nprocs = env_->numProcs_;
  partId_t nparts = nGlobalParts_;

  if (nprocs < 2)
    return;

  char flag=0;

  if (rank == 0){
    if (pSizeUniform_[wdim] == true)
      flag = 1;
    else if (pCompactIndex_[wdim].size() > 0)
      flag = 2;
    else
      flag = 3;
  }

  try{
    Teuchos::broadcast<int, char>(*comm_, 0, 1, &flag);
  }
  Z2_THROW_OUTSIDE_ERROR(*env_);

  if (flag == 1){
    if (rank > 0)
      pSizeUniform_[wdim] = true;

    return;
  }

  if (flag == 2){

    // broadcast the indices into the size list

    unsigned char *idxbuf = NULL;

    if (rank > 0){
      idxbuf = new unsigned char [nparts];
      env_->localMemoryAssertion(__FILE__, __LINE__, nparts, idxbuf);
    }
    else{
      env_->localBugAssertion(__FILE__, __LINE__, "index list size", 
        pCompactIndex_[wdim].size() == nparts, COMPLEX_ASSERTION);
      idxbuf = pCompactIndex_[wdim].getRawPtr();
    }

    try{
      // broadcast of unsigned char is not supported
      Teuchos::broadcast<int, char>(*comm_, 0, nparts, 
        reinterpret_cast<char *>(idxbuf));
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    if (rank > 0)
      pCompactIndex_[wdim] = arcp(idxbuf, 0, nparts);

    // broadcast the list of different part sizes

    unsigned char maxIdx=0;
    for (partId_t p=0; p < nparts; p++)
      if (idxbuf[p] > maxIdx) maxIdx = idxbuf[p];

    int numSizes = maxIdx + 1;
  
    float *sizeList = NULL;

    if (rank > 0){
      sizeList = new float [numSizes];
      env_->localMemoryAssertion(__FILE__, __LINE__, numSizes, sizeList);
    }
    else{
      env_->localBugAssertion(__FILE__, __LINE__, "wrong number of sizes", 
        numSizes == pSize_[wdim].size(), COMPLEX_ASSERTION);

      sizeList = pSize_[wdim].getRawPtr();
    }

    try{
      Teuchos::broadcast<int, float>(*comm_, 0, numSizes, sizeList);
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    if (rank > 0)
      pSize_[wdim] = arcp(sizeList, 0, numSizes);

    return;
  }

  if (flag == 3){

    // broadcast the size of each part

    float *sizeList = NULL;

    if (rank > 0){
      sizeList = new float [nparts];
      env_->localMemoryAssertion(__FILE__, __LINE__, nparts, sizeList);
    }
    else{
      env_->localBugAssertion(__FILE__, __LINE__, "wrong number of sizes", 
        nparts == pSize_[wdim].size(), COMPLEX_ASSERTION);

      sizeList = pSize_[wdim].getRawPtr();
    }

    try{
      Teuchos::broadcast<int, float>(*comm_, 0, nparts, sizeList);
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    if (rank > 0)
      pSize_[wdim] = arcp(sizeList, 0, nparts);

    return;
  }
}

template <typename User_t>
  void PartitioningSolution<User_t>::computePartSizes(int wdim,
    ArrayView<partId_t> ids, ArrayView<float> sizes)
{
  int len = ids.size();

  if (len == 1){
    pSizeUniform_[wdim] = true;
    return;
  }

  env_->localBugAssertion(__FILE__, __LINE__, "bad array sizes", 
    len>0 && sizes.size()==len, COMPLEX_ASSERTION);

  env_->localBugAssertion(__FILE__, __LINE__, "bad index", 
    wdim>=0 && wdim<weightDim_, COMPLEX_ASSERTION);

  env_->localBugAssertion(__FILE__, __LINE__, "preallocations", 
    pSize_.size()>wdim && 
    pSizeUniform_.size()>wdim && pCompactIndex_.size()>wdim, 
    COMPLEX_ASSERTION);

  // Check ids and sizes and find min, max and average sizes.

  partId_t nparts = nGlobalParts_;
  unsigned char *buf = new unsigned char [nparts];
  env_->localMemoryAssertion(__FILE__, __LINE__, nparts, buf);
  memset(buf, 0, nparts);
  ArrayRCP<unsigned char> partIdx(buf, 0, nparts, true);

  float min, max, sum;

  for (int i=0; i < len; i++){
    partId_t id = ids[i];
    float size = sizes[i];

    env_->localInputAssertion(__FILE__, __LINE__, "invalid part id", 
      id>=0 && id<nparts, BASIC_ASSERTION);

    env_->localInputAssertion(__FILE__, __LINE__, "invalid part size", size>=0,
      BASIC_ASSERTION);

    // TODO: we could allow users to specify multiple sizes for the same
    // part if we add a parameter that says what we are to do with them:
    // add them or take the max.

    env_->localInputAssertion(__FILE__, __LINE__, 
      "multiple sizes provided for one part", partIdx[id]==0, BASIC_ASSERTION);

    partIdx[id] = 1;

    if (i==0 || size < min) min = size;
    if (i==0 || size > max) max = size;
    sum += size;
  }

  if (sum == 0){   // user has given us a list of parts of size 0
    
    float *allSizes = new float [2];
    env_->localMemoryAssertion(__FILE__, __LINE__, 2, allSizes);

    ArrayRCP<float> sizeArray(allSizes, 0, 2, true);

    allSizes[0] = 0.0;
    allSizes[1] = 1.0 / (nparts - len);

    for (partId_t p=0; p < nparts; p++)
      buf[p] = 1;                 // index to default part size

    for (int i=0; i < len; i++)
      buf[ids[i]] = 0;            // index to part size zero
    
    pSize_[wdim] = sizeArray;
    pCompactIndex_[wdim] = partIdx;

    return;
  }

  double epsilon = (max - min) * 10e-5;  // to distinguish different sizes

  if (max - min <= epsilon){
    pSizeUniform_[wdim] = true;
    return;
  }

  float avg = sum / len;    // size for parts that were not specified

  // We are going to merge part sizes that are very close.  This takes
  // computation time now, but can save considerably in the storage of
  // all part sizes on each process.  For example, a common case may
  // be some parts are size 1 and all the rest are size 2.

  float *tmp = new float [len];
  env_->localMemoryAssertion(__FILE__, __LINE__, len, tmp);
  memcpy(tmp, sizes.getRawPtr(), sizeof(float) * len);
  ArrayRCP<float> partSizes(tmp, 0, len, true);

  std::sort(partSizes.begin(), partSizes.end());

  // create a list of sizes that are unique within epsilon

  Array<float> nextUniqueSize;
  nextUniqueSize.push_back(partSizes[len-1]);   // largest
  float curr = partSizes[len-1];
  int avgIndex = len;
  bool haveAvg = false;
  if (curr - avg <= epsilon)
     avgIndex = 0;

  for (int i=len-2; i >= 0; i--){
    float val = partSizes[i];
    if (curr - val > epsilon){
      nextUniqueSize.push_back(val);  // the highest in the group
      curr = val;
      if (avgIndex==len && val > avg && val - avg <= epsilon){
        // the average would be in this group
        avgIndex = nextUniqueSize.size() - 1;
        haveAvg = true;
      }
    }
  }

  partSizes.clear();

  size_t numSizes = nextUniqueSize.size();
  int sizeArrayLen = numSizes;

  if (numSizes < 64){
    // We can store the size for every part in a compact way.

    // Create a list of all sizes in increasing order

    if (!haveAvg) sizeArrayLen++;   // need to include average
    
    float *allSizes = new float [sizeArrayLen];
    env_->localMemoryAssertion(__FILE__, __LINE__, sizeArrayLen, allSizes);
    ArrayRCP<float> sizeArray(allSizes, 0, sizeArrayLen, true);

    int newAvgIndex = sizeArrayLen;

    for (int i=numSizes-1, idx=0; i >= 0; i--){

      if (newAvgIndex == sizeArrayLen){

        if (haveAvg && i==avgIndex)
          newAvgIndex = idx;

        else if (!haveAvg && avg < nextUniqueSize[i]){
          newAvgIndex = idx;
          allSizes[idx++] = avg;
        }
      }

      allSizes[idx++] = nextUniqueSize[i];
    }

    env_->localBugAssertion(__FILE__, __LINE__, "finding average in list", 
      newAvgIndex < sizeArrayLen, COMPLEX_ASSERTION);

    for (int i=0; i < nparts; i++){
      buf[i] = newAvgIndex;   // index to default part size
    }

    sum = (nparts - len) * allSizes[newAvgIndex];

    for (int i=0; i < len; i++){
      int id = ids[i];
      float size = sizes[i];
      int index;

      // Find the first size greater than or equal to this size.

      if (size < avg && avg - size <= epsilon)
        index = newAvgIndex;
      else{
        ArrayRCP<float>::iterator found = std::lower_bound(sizeArray.begin(),
          sizeArray.end(), size);

        env_->localBugAssertion(__FILE__, __LINE__, "size array", 
          found != sizeArray.end(), COMPLEX_ASSERTION);

        index = found - sizeArray.begin();
      }

      buf[id] = index;
      sum += allSizes[index];
    }

    for (int i=0; i < sizeArrayLen; i++){
      sizeArray[i] /= sum;
    }

    pCompactIndex_[wdim] = partIdx;
    pSize_[wdim] = sizeArray;
  }
  else{
    // To have access to part sizes, we must store nparts floats on 
    // every process.  We expect this is a rare case.

    partIdx.clear();

    tmp = new float [nparts];
    env_->localMemoryAssertion(__FILE__, __LINE__, nparts, tmp);

    sum += ((nparts - len) * avg);

    for (int i=0; i < nparts; i++){
      tmp[i] = avg/sum;
    }

    for (int i=0; i < len; i++){
      tmp[ids[i]] = sizes[i]/sum;
    }

    pSize_[wdim] = arcp(tmp, 0, nparts);
  }
}


template <typename User_t>
  void PartitioningSolution<User_t>::setParts(
    ArrayView<const gno_t> gnoList, ArrayRCP<partId_t> partList,
    ArrayRCP<float> &imbalance)
{
  size_t ngnos = gnoList.size();
  
  if (ngnos){
  
    // Create list of application's global IDs: gids_
  
    if (idMap_->gnosAreGids()){
      gids_ = Teuchos::arcpFromArrayView<const gid_t>(gnoList);
    }
    else{
      ArrayView<gno_t> gnoView = av_const_cast<gno_t>(gnoList);
  
      gid_t *gidList = new gid_t [gnoList.size()];
      env_->localMemoryAssertion(__FILE__, __LINE__, ngnos, gidList);
  
      gids_ = arcp<const gid_t>(gidList, 0, ngnos);
   
      ArrayView<gid_t> gidView(gidList, ngnos);
      
      try{
        idMap_->gidTranslate(gidView , gnoView, TRANSLATE_LIB_TO_APP);
      }
      Z2_FORWARD_EXCEPTIONS
    }

    // Save part_ list, create proc_ list if it's not the same.
  
    parts_ = partList;

    if (!onePartPerProc_){

      int *procs = new int [ngnos];
      env_->localMemoryAssertion(__FILE__, __LINE__, ngnos, procs);
      procs_ = arcp<int>(procs, 0, ngnos);

      partId_t *parts = partList.getRawPtr();

      if (procDist_.size() > 0){    // parts are not split across procs

        int procId;
        for (lno_t i=0; i < ngnos; i++){
          partToProcs(parts[i], procs[i], procId);
        }
      }
      else{  // harder - we need to split the parts across multiple procs

        lno_t *partCounter = new lno_t [nGlobalParts_];
        env_->localMemoryAssertion(__FILE__, __LINE__, nGlobalParts_, 
          partCounter);

        int numProcs = comm_->getSize();

        for (lno_t i=0; i < partList.size(); i++)
          partCounter[parts[i]]++;

        lno_t *procCounter = new lno_t [numProcs];
        env_->localMemoryAssertion(__FILE__, __LINE__, numProcs, procCounter);
        
        int proc2 = partDist_[0];

        for (lno_t part=1; part < nGlobalParts_; part++){
          int proc1 = proc2;
          proc2 = partDist_[part+1];
          int nprocs = proc2 - proc1;

          double dNum = partCounter[part];
          double dProcs = nprocs;
          
          double each = floor(dNum/dProcs);
          double extra = fmod(dNum,dProcs);

          for (int proc=proc1, i=0; proc<proc2; proc++, i++){
            if (i < extra)
              procCounter[proc] = lno_t(each) + 1;
            else
              procCounter[proc] = lno_t(each);
          }          
        }

        delete [] partCounter;

        for (lno_t i=0; i < partList.size(); i++){
          partId_t partNum = parts[i];
          int proc1 = partDist_[partNum];
          int proc2 = partDist_[partNum + 1];
          int proc=0;
          
          for (proc=proc1; proc < proc2; proc++){
            if (procCounter[proc] > 0){
              procs[i] = proc;
              procCounter[proc]--;
              break;
            }
          }
          env_->localBugAssertion(__FILE__, __LINE__, "part to proc", 
            proc < proc2, COMPLEX_ASSERTION);
        }

        delete [] procCounter;
      }
    }
  }

  // Create imbalance list: one for each weight dimension: weights_
  
  float *imbList = new float [weightDim_];
  env_->localMemoryAssertion(__FILE__, __LINE__, weightDim_, imbList);
  memcpy(imbList, imbalance.getRawPtr(), sizeof(float) * weightDim_);
  
  imbalance_ = arcp<float>(imbList, 0, weightDim_);
}

template <typename User_t>
  void PartitioningSolution<User_t>::procToParts(int procId, 
    double &numParts, partId_t &partMin, partId_t &partMax) const
{
  if (onePartPerProc_){
    numParts = 1.0;
    partMin = partMax = procId;
  }
  else if (procDist_.size() > 0){
    partMin = procDist_[procId];
    partMax = procDist_[procId+1] - 1;
    numParts = procDist_[procId+1] - partMin;
  }
  else{
    // find the first p such that partDist_[p] > procId

    std::vector<int>::const_iterator entry;
    entry = std::upper_bound(partDist_.begin(), partDist_.end(), procId);

    size_t partIdx = entry - partDist_.begin();
    int numProcs = partDist_[partIdx] - partDist_[partIdx-1];
    partMin = partMax = int(partIdx) - 1;
    numParts = 1.0 / numProcs;
  }
}

template <typename User_t>
  void PartitioningSolution<User_t>::partToProcs(partId_t partId, 
    int &procMin, int &procMax) const
{
  if (onePartPerProc_){
    procMin = procMax = int(partId);
  }
  else if (procDist_.size() > 0){
    // find the first p such that procDist_[p] > partId

    std::vector<partId_t>::const_iterator entry;
    entry = std::upper_bound(procDist_.begin(), procDist_.end(), partId);

    size_t procIdx = entry - procDist_.begin();
    procMin = procMax = int(procIdx) - 1;
  }
  else{
    procMin = partDist_[partId];
    procMax = partDist_[partId+1] - 1;
  }
}

}  // namespace Zoltan2

#endif
