#ifndef TSFMULTIVECTOROPERATOR_H
#define TSFMULTIVECTOROPERATOR_H

#include "TSFConfig.h"
#include "TSFVector.h"
#include "TSFLinearOperatorBase.h"
#include "TSFArray.h"
#include "TSFLinearOperator.h"

namespace TSF
{

  /** \ingroup LinearOperatorSubtypes
   */

  class TSFMultiVectorOperator : public TSFLinearOperatorBase
    {
    public:
      /** Empty ctor, sets domain and range to empty spaces */
      TSFMultiVectorOperator();
      /** Ctor sets domain and range spaces to specified values */
      TSFMultiVectorOperator(const TSFVectorSpace& space,
                             int numVectors,
                             bool isVertical);
      /*
         TSFMultiVectorOperator(const TSFVectorSpace& space,
         bool isVertical,
         const TSFArray<TSFVector>& vectors);
      */


      /* the usual virtual dtor */
      virtual ~TSFMultiVectorOperator(){;}


      /** apply operator to a vector in the domain space, returning a vector
       * in the range space */
      virtual void apply(const TSFVector& in,
                         TSFVector& out) const ;

      /** apply adjoint operator to a vector in the domain space, returning
       * a vector in the range space. The default implementation throws an
       * exception */
      virtual void applyAdjoint(const TSFVector& in,
                                TSFVector& out) const ;

      /** get one of the vectors that comprise this operator */
      virtual TSFVector getVector(int i) const {return vectors_[i];}


      /** */
      int numVectors() const {return vectors_.length();}


      /** get the i-th row  */
      virtual void getRow(int row, TSFArray<int>& indices,
                          TSFArray<TSFReal>& values) const;


      /**  create the transpose */
      virtual TSFLinearOperator* getTranspose();

      /**
       * Write to a stream
       */
      virtual void print(ostream& os) const ;

    protected:

      void verticalMVMult(const TSFVector& in,
                          TSFVector& out) const ;
      void horizontalMVMult(const TSFVector& in,
                            TSFVector& out) const ;


      bool isVertical_;

      TSFArray<TSFVector> vectors_;

      TSFLinearOperator opTrp_;
      bool haveTranspose_;

    };

  /** */
  class TSFVerticalMultiVectorOperator : public TSFMultiVectorOperator
    {
    public:
      /** */
      TSFVerticalMultiVectorOperator(const TSFVectorSpace& space,
                                     int numVectors)
        : TSFMultiVectorOperator(space, numVectors, true) {}

      /*
      TSFVerticalMultiVectorOperator(const TSFVectorSpace& space,
                                     const TSFArray<TSFVector>& vectors)
        : TSFMultiVectorOperator(space, true, vectors) {}
      */
      /** */
      virtual ~TSFVerticalMultiVectorOperator(){;}


    };


  /** */
  class TSFHorizontalMultiVectorOperator : public TSFMultiVectorOperator
    {
    public:
      /** */
      TSFHorizontalMultiVectorOperator(const TSFVectorSpace& space,
                                       int numVectors)
        : TSFMultiVectorOperator(space, numVectors, false) {}

      /** */
      virtual ~TSFHorizontalMultiVectorOperator(){;}


    };
}

#endif
