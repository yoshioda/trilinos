// $Id$ 
// $Source$ 
// @HEADER
// ***********************************************************************
// 
//                           Stokhos Package
//                 Copyright (2008) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Eric T. Phipps (etphipp@sandia.gov).
// 
// ***********************************************************************
// @HEADER

#include "Stokhos_SGModelEvaluator.hpp"

#include <algorithm>
#include "Teuchos_TestForException.hpp"
#ifdef HAVE_MPI
#include "EpetraExt_MultiMpiComm.h"
#endif
#include "EpetraExt_BlockUtility.h"
#include "EpetraExt_BlockCrsMatrix.h"
#include "Stokhos_MatrixFreeEpetraOp.hpp"
#include "Stokhos_MeanEpetraOp.hpp"

Stokhos::SGModelEvaluator::SGModelEvaluator(
     const Teuchos::RCP<EpetraExt::ModelEvaluator>& me_,
     const Teuchos::RCP<const Stokhos::OrthogPolyBasis<int,double> >& sg_basis_,
     const std::vector<int>& sg_p_index_,
     const std::vector<int>& sg_g_index_,
     const Teuchos::Array< Teuchos::Array< Teuchos::RCP<Epetra_Vector> > >& initial_p_sg_coeffs_,
     const Teuchos::RCP<Teuchos::ParameterList>& params_,
     const Teuchos::RCP<const Epetra_Comm>& comm) 
  : me(me_),
    sg_basis(sg_basis_),
    sg_p_index(sg_p_index_),
    sg_g_index(sg_g_index_),
    params(params_),
    num_sg_blocks(sg_basis->size()),
    supports_x(false),
    x_map(me->get_x_map()),
    f_map(me->get_f_map()),
    sg_comm(),
    sg_x_map(),
    sg_f_map(),
    num_p(sg_p_index.size()),
    sg_p_map(num_p),
    sg_p_names(num_p),
    num_g(sg_g_index.size()),
    sg_g_map(num_g),
    Cijk(),
    x_dot_sg_blocks(),
    x_sg_blocks(),
    p_sg_blocks(num_p),
    f_sg_blocks(),
    W_sg_blocks(),
    g_sg_blocks(num_g),
    jacobianMethod(MATRIX_FREE),
    sg_p_init(num_p),
    eval_W_with_f(false)
{
  if (x_map != Teuchos::null)
    supports_x = true;

#ifdef HAVE_MPI
  // No parallelism over blocks, so spatial partition is unchanged 
  // as comm->NumProc()
  Teuchos::RCP<EpetraExt::MultiMpiComm> multiComm =
    Teuchos::rcp(new EpetraExt::MultiMpiComm(MPI_COMM_WORLD, 
					     comm->NumProc(), 
					     num_sg_blocks));
  int numBlockRows =  multiComm->NumTimeSteps();
  int myBlockRows  =  multiComm->NumTimeStepsOnDomain();
  int myFirstBlockRow = multiComm->FirstTimeStepOnDomain();
  sg_comm = multiComm;
#else
  int numBlockRows =  1;
  int myBlockRows  =  1;
  int myFirstBlockRow = 0;
  sg_comm = comm;
#endif

  // DENSE STENCIL for Stochastic Galerkin
  // For 3 blocks on 2 procs, this should be:
  // Proc  nBR  mBR  mFBR     Stencil      Index
  //  0     3    2    0       0  1  2        0
  //                         -1  0  1        1
  //  1     3    1    2      -2 -1  0        2
  //
  rowStencil.resize(myBlockRows);
  rowIndex.resize(myBlockRows);
  for (int i=0; i < myBlockRows; i++) {
    for (int j=0; j < numBlockRows; j++) 
      rowStencil[i].push_back(-myFirstBlockRow - i + j);
    rowIndex[i] = (i + myFirstBlockRow);
  }

  if (supports_x) {

    // Create block SG x and f maps
    sg_x_map = 
      Teuchos::rcp(EpetraExt::BlockUtility::GenerateBlockMap(*x_map,
							     rowIndex,
							     *sg_comm));
    sg_f_map = 
      Teuchos::rcp(EpetraExt::BlockUtility::GenerateBlockMap(*f_map,
							     rowIndex,
							     *sg_comm));
    
    // Create vector blocks
    x_dot_sg_blocks = 
      Teuchos::rcp(new Stokhos::VectorOrthogPoly<Epetra_Vector>(sg_basis));
    x_sg_blocks = 
      Teuchos::rcp(new Stokhos::VectorOrthogPoly<Epetra_Vector>(sg_basis));
    f_sg_blocks = 
      Teuchos::rcp(new Stokhos::VectorOrthogPoly<Epetra_Vector>(sg_basis));
    W_sg_blocks = 
      Teuchos::rcp(new Stokhos::VectorOrthogPoly<Epetra_Operator>(sg_basis));
    for (unsigned int i=0; i<num_sg_blocks; i++)
      W_sg_blocks->setCoeffPtr(i, me->create_W());

    // Get block Jacobian method
    std::string method = params->get("Jacobian Method", "Matrix Free");
    if (method == "Matrix Free")
      jacobianMethod = MATRIX_FREE;
    else if (method == "Fully Assembled")
      jacobianMethod = FULLY_ASSEMBLED;
    else
      TEST_FOR_EXCEPTION(true, 
			 std::logic_error,
			 std::endl << 
			 "Error!  Stokhos::SGModelEvaluator():  " <<
			 "Unknown Jacobian Method " << method << "!" << 
			 std::endl);

    Cijk = sg_basis->getTripleProductTensor();
  }
    
   // Parameters -- The idea here is to replace the parameter vectors
   // specified by sg_p_index with parameter vectors that are the 
   // concatenation of all of SG components of each parameter vector.
   // This way the coefficients of the parameter expansions become
   // parameters.

   // Create parameter maps, names, and initial values
   for (int i=0; i<num_p; i++) {
     Teuchos::RCP<const Epetra_Map> p_map = me->get_p_map(sg_p_index[i]);
     sg_p_map[i] = 
       Teuchos::rcp(EpetraExt::BlockUtility::GenerateBlockMap(*p_map,
							      rowIndex,
							      *sg_comm));

     Teuchos::RCP<const Teuchos::Array<std::string> > p_names = 
       me->get_p_names(sg_p_index[i]);
     if (p_names != Teuchos::null) {
       sg_p_names[i] = 
	 Teuchos::rcp(new Teuchos::Array<std::string>(num_sg_blocks*(p_names->size())));
       for (unsigned int j=0; j<p_names->size(); j++) {
	 std::stringstream ss;
	 ss << (*p_names)[j] << " -- SG Coefficient " << i;
	 (*sg_p_names[i])[j] = ss.str();
       }
     }

     // Create initial p
     sg_p_init[i] = 
       Teuchos::rcp(new EpetraExt::BlockVector(*p_map, *sg_p_map[i]));
     for (unsigned int j=0; j<num_sg_blocks; j++)
       sg_p_init[i]->LoadBlockValues(*(initial_p_sg_coeffs_[i][j]), j);

     // Create p SG blocks
     p_sg_blocks[i] = 
       Teuchos::rcp(new Stokhos::VectorOrthogPoly<Epetra_Vector>(sg_basis));

   }

   // Responses -- The idea here is to replace the response vectors
   // specified by sg_g_index with response vectors that are the 
   // concatenation of all of SG components of each response vector.
   // This way the coefficients of the response expansions become
   // responses.

   // Create response maps
   for (int i=0; i<num_g; i++) {
     Teuchos::RCP<const Epetra_Map> g_map = me->get_g_map(sg_g_index[i]);
     sg_g_map[i] = 
       Teuchos::rcp(EpetraExt::BlockUtility::GenerateBlockMap(*g_map,
							      rowIndex,
							      *sg_comm));

     // Create p SG blocks
     g_sg_blocks[i] = 
       Teuchos::rcp(new Stokhos::VectorOrthogPoly<Epetra_Vector>(sg_basis));

   }

   if (supports_x) {
     // Create initial x
     sg_x_init = Teuchos::rcp(new EpetraExt::BlockVector(*x_map, *sg_x_map));
     Teuchos::RCP<const Epetra_Vector> xinit = me->get_x_init();
     sg_x_init->LoadBlockValues(*xinit, 0);

     // Get preconditioner factory for matrix-free
     if (jacobianMethod == MATRIX_FREE)
       precFactory = params->get< Teuchos::RCP<Stokhos::PreconditionerFactory> >("Preconditioner Factory");
   }

   eval_W_with_f = params->get("Evaluate W with F", false);
}

// Overridden from EpetraExt::ModelEvaluator

Teuchos::RCP<const Epetra_Map>
Stokhos::SGModelEvaluator::get_x_map() const
{
  return sg_x_map;
}

Teuchos::RCP<const Epetra_Map>
Stokhos::SGModelEvaluator::get_f_map() const
{
  return sg_f_map;
}

Teuchos::RCP<const Epetra_Map>
Stokhos::SGModelEvaluator::get_p_map(int l) const
{
  std::vector<int>::const_iterator it
    = std::find(sg_p_index.begin(), sg_p_index.end(), l);
  if (it != sg_p_index.end()) {
    int offset = it - sg_p_index.begin();
    return sg_p_map[offset];
  }
  else
    return me->get_p_map(l);
}

Teuchos::RCP<const Epetra_Map>
Stokhos::SGModelEvaluator::get_g_map(int l) const
{
  std::vector<int>::const_iterator it
    = std::find(sg_g_index.begin(), sg_g_index.end(), l);
  if (it != sg_g_index.end()) {
    int offset = it - sg_g_index.begin();
    return sg_g_map[offset];
  }
  else
    return me->get_g_map(l);
}

Teuchos::RCP<const Teuchos::Array<std::string> >
Stokhos::SGModelEvaluator::get_p_names(int l) const
{
  std::vector<int>::const_iterator it
    = std::find(sg_p_index.begin(), sg_p_index.end(), l);
  if (it != sg_p_index.end()) {
    int offset = it - sg_p_index.begin();
    return sg_p_names[offset];
  }
  else
    return me->get_p_names(l);
}

Teuchos::RCP<const Epetra_Vector>
Stokhos::SGModelEvaluator::get_x_init() const
{
  return sg_x_init;
}

Teuchos::RCP<const Epetra_Vector>
Stokhos::SGModelEvaluator::get_p_init(int l) const
{
  std::vector<int>::const_iterator it
    = std::find(sg_p_index.begin(), sg_p_index.end(), l);
  if (it != sg_p_index.end()) {
    int offset = it - sg_p_index.begin();
    return sg_p_init[offset];
  }
  else
    return me->get_p_init(l);
}

Teuchos::RCP<Epetra_Operator>
Stokhos::SGModelEvaluator::create_W() const
{
  if (supports_x) {
    if (jacobianMethod == MATRIX_FREE)
      my_W = Teuchos::rcp(new Stokhos::MatrixFreeEpetraOp(x_map, sg_x_map, 
							  sg_basis, Cijk, 
							  W_sg_blocks));
    else if (jacobianMethod == FULLY_ASSEMBLED) {
      Teuchos::RCP<Epetra_Operator> W = me->create_W();
      Teuchos::RCP<Epetra_RowMatrix> W_row = 
	Teuchos::rcp_dynamic_cast<Epetra_RowMatrix>(W, true);
      my_W =
	Teuchos::rcp(new EpetraExt::BlockCrsMatrix(*W_row, rowStencil, 
						   rowIndex, *sg_comm));
    }

    return my_W;
  }
  
  return Teuchos::null;
}

Teuchos::RCP<Epetra_Operator>
Stokhos::SGModelEvaluator::create_prec() const
{
  if (supports_x) {
    if (jacobianMethod == MATRIX_FREE)
      return Teuchos::rcp(new Stokhos::MeanEpetraOp(x_map, sg_x_map, 
						    num_sg_blocks, 
						    Teuchos::null));
    else if (jacobianMethod == FULLY_ASSEMBLED) {
      Teuchos::RCP<Epetra_Operator> W = me->create_W();
      Teuchos::RCP<Epetra_RowMatrix> W_row = 
	Teuchos::rcp_dynamic_cast<Epetra_RowMatrix>(W, true);
      return
	Teuchos::rcp(new EpetraExt::BlockCrsMatrix(*W_row, rowStencil, 
						   rowIndex, *sg_comm));
    }
  }
  
  return Teuchos::null;
}

EpetraExt::ModelEvaluator::InArgs
Stokhos::SGModelEvaluator::createInArgs() const
{
  InArgsSetup inArgs;
  InArgs me_inargs = me->createInArgs();

  inArgs.setModelEvalDescription(this->description());
  inArgs.set_Np(me_inargs.Np()); 
  inArgs.set_Np_sg(0);
  inArgs.setSupports(IN_ARG_x_dot, me_inargs.supports(IN_ARG_x_dot));
  inArgs.setSupports(IN_ARG_x, me_inargs.supports(IN_ARG_x));
  inArgs.setSupports(IN_ARG_t, me_inargs.supports(IN_ARG_t));
  inArgs.setSupports(IN_ARG_alpha, me_inargs.supports(IN_ARG_alpha));
  inArgs.setSupports(IN_ARG_beta, me_inargs.supports(IN_ARG_beta));
  
  return inArgs;
}

EpetraExt::ModelEvaluator::OutArgs
Stokhos::SGModelEvaluator::createOutArgs() const
{
  OutArgsSetup outArgs;
  OutArgs me_outargs = me->createOutArgs();

  outArgs.setModelEvalDescription(this->description());
  outArgs.set_Np_Ng(me_outargs.Np(), me_outargs.Ng());
  outArgs.set_Ng_sg(0);
  outArgs.setSupports(OUT_ARG_f, me_outargs.supports(OUT_ARG_f));
  outArgs.setSupports(OUT_ARG_W, me_outargs.supports(OUT_ARG_W));
  
  return outArgs;
}

void 
Stokhos::SGModelEvaluator::evalModel(const InArgs& inArgs,
				     const OutArgs& outArgs) const
{
  // Get the output arguments
  EpetraExt::ModelEvaluator::Evaluation<Epetra_Vector> f_out;
  if (outArgs.supports(OUT_ARG_f))
    f_out = outArgs.get_f();
  Teuchos::RCP<Epetra_Operator> W_out;
  if (outArgs.supports(OUT_ARG_W))
    W_out = outArgs.get_W();

  // Check if we are using the "matrix-free" method for W and we are 
  // computing a preconditioner.  
  bool eval_mean = jacobianMethod == MATRIX_FREE && W_out != Teuchos::null && 
    f_out.getType() == EVAL_TYPE_VERY_APPROX_DERIV;

  // Here we are assuming a full W fill occurred previously which we can use
  // for the preconditioner.  Given the expense of computing the SG W blocks
  // this saves signfifcant computational cost
  if (eval_mean) {
    Teuchos::RCP<Stokhos::MeanEpetraOp> W_mean = 
      Teuchos::rcp_dynamic_cast<Stokhos::MeanEpetraOp>(W_out, true);
    Teuchos::RCP<Epetra_Operator> prec = 
      precFactory->compute(W_sg_blocks->getCoeffPtr(0));    
    W_mean->setMeanOperator(prec);

    // We can now quit unless a fill of f or g was also requested
    bool done = (f_out == Teuchos::null);
    for (int i=0; i<outArgs.Ng(); i++)
      done = (done && (outArgs.get_g(i) == Teuchos::null));
    if (done)
      return;
  }

  // Get the input arguments
  Teuchos::RCP<const Epetra_Vector> x;
  if (inArgs.supports(IN_ARG_x))
    x = inArgs.get_x();
  Teuchos::RCP<const Epetra_Vector> x_dot;
  if (inArgs.supports(IN_ARG_x_dot))
    x_dot = inArgs.get_x_dot();

  // Copy x, x_dot to SG components
  if (x != Teuchos::null) {
    EpetraExt::BlockVector x_sg(View, *x_map, *x);
    for (unsigned int i=0; i<num_sg_blocks; i++)
      x_sg_blocks->setCoeffPtr(i, x_sg.GetBlock(i));
  }
  if (x_dot != Teuchos::null) {
    EpetraExt::BlockVector x_dot_sg(View, *x_map, *x_dot);
    for (unsigned int i=0; i<num_sg_blocks; i++)
      x_dot_sg_blocks->setCoeffPtr(i, x_dot_sg.GetBlock(i));
  }

  // Create underlying inargs
  InArgs me_inargs = me->createInArgs();
  if (x != Teuchos::null)
    me_inargs.set_x_sg(x_sg_blocks);
  if (x_dot != Teuchos::null)
    me_inargs.set_x_dot_sg(x_dot_sg_blocks);
  if (me_inargs.supports(IN_ARG_alpha))
    me_inargs.set_alpha(inArgs.get_alpha());
  if (me_inargs.supports(IN_ARG_beta))
    me_inargs.set_beta(inArgs.get_beta());
  if (me_inargs.supports(IN_ARG_t))
    me_inargs.set_t(inArgs.get_t());

  // Pass parameters
  for (int i=0; i<inArgs.Np(); i++) {
    Teuchos::RCP<const Epetra_Vector> p = inArgs.get_p(i);
    std::vector<int>::const_iterator it = 
      std::find(sg_p_index.begin(), sg_p_index.end(), i);
    if (it != sg_p_index.end()) {
      int offset = it - sg_p_index.begin();

      // We always need to pass in the SG parameters, so just use
      // the initial parameters if it is NULL
      if (p == Teuchos::null)
	p = get_p_init(i);

      EpetraExt::BlockVector p_sg(View, sg_p_init[offset]->GetBaseMap(), *p);
      for (unsigned int j=0; j<num_sg_blocks; j++)
	p_sg_blocks[offset]->setCoeffPtr(j, p_sg.GetBlock(j));
      me_inargs.set_p_sg(offset, p_sg_blocks[offset]);
    }
    else
      me_inargs.set_p(i, p);
  }

  // Create underlying outargs
  OutArgs me_outargs = me->createOutArgs();
  if (f_out != Teuchos::null) {
    EpetraExt::BlockVector f_sg(View, *f_map, *f_out);
    for (unsigned int i=0; i<num_sg_blocks; i++)
      f_sg_blocks->setCoeffPtr(i, f_sg.GetBlock(i));
    me_outargs.set_f_sg(f_sg_blocks);
    if (eval_W_with_f)
      me_outargs.set_W_sg(W_sg_blocks);
  }
  if (W_out != Teuchos::null && !eval_W_with_f && !eval_mean)
     me_outargs.set_W_sg(W_sg_blocks);

  // Pass responses
  for (int i=0; i<outArgs.Ng(); i++) {
    Teuchos::RCP<Epetra_Vector> g = outArgs.get_g(i);
    if (g != Teuchos::null) {
      std::vector<int>::const_iterator it = 
	std::find(sg_g_index.begin(), sg_g_index.end(), i);
      if (it != sg_g_index.end()) {
	int offset = it - sg_g_index.begin();
	EpetraExt::BlockVector g_sg(View, *(me->get_g_map(i)), *g);
	for (unsigned int j=0; j<num_sg_blocks; j++)
	  g_sg_blocks[offset]->setCoeffPtr(j, g_sg.GetBlock(j));
	me_outargs.set_g_sg(offset, g_sg_blocks[offset]);
      }
      else
	me_outargs.set_g(i, g);
    }
  }

  // Compute the functions
  me->evalModel(me_inargs, me_outargs);

  // Copy block SG components for f and W into f and W
  if ((W_out != Teuchos::null || (eval_W_with_f && f_out != Teuchos::null)) 
      && !eval_mean) {
    Teuchos::RCP<Epetra_Operator> W;
    if (W_out != Teuchos::null)
      W = W_out;
    else
      W = my_W;
    if (jacobianMethod == MATRIX_FREE) {
      Teuchos::RCP<Stokhos::MatrixFreeEpetraOp> W_mf = 
	Teuchos::rcp_dynamic_cast<Stokhos::MatrixFreeEpetraOp>(W, true);
      W_mf->reset(W_sg_blocks);
    }
    else if (jacobianMethod == FULLY_ASSEMBLED) {
      Teuchos::RCP<EpetraExt::BlockCrsMatrix> W_sg = 
	Teuchos::rcp_dynamic_cast<EpetraExt::BlockCrsMatrix>(W, true);
      Teuchos::RCP<Epetra_RowMatrix> W_row;
      int i,j;
      double cijk;
      W_sg->PutScalar(0.0);
      for (unsigned int k=0; k<num_sg_blocks; k++) {
	W_row = 
	  Teuchos::rcp_dynamic_cast<Epetra_RowMatrix>(W_sg_blocks->getCoeffPtr(k), 
						      true);
	int nl = Cijk->num_values(k);
	for (int l=0; l<nl; l++) {
	  Cijk->value(k,l,i,j,cijk);
	  W_sg->SumIntoBlock(cijk/sg_basis->norm_squared(i), *W_row, i, j);
	}
      }
    }
  }
}

void 
Stokhos::SGModelEvaluator::set_x_init(const Epetra_Vector& x_in)
{
  sg_x_init->Scale(1.0, x_in);
}
