/*
// @HEADER
// ***********************************************************************
// 
//    OptiPack: Collection of simple Thyra-based Optimization ANAs
//                 Copyright (2009) Sandia Corporation
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
// Questions? Contact Roscoe A. Bartlett (rabartl@sandia.gov) 
// 
// ***********************************************************************
// @HEADER
*/


#include "OptiPack_DiagonalQuadraticResponseOnlyModelEvaluator_decl.hpp"
#include "Thyra_DefaultSpmdVectorSpace.hpp"
#include "Teuchos_DefaultComm.hpp"


#ifndef OPTIPACK_DIAGONAL_QUADRATIC_RESPONSE_ONLY_MODEL_EVALUATOR_DEF_HPP
#define OPTIPACK_DIAGONAL_QUADRATIC_RESPONSE_ONLY_MODEL_EVALUATOR_DEF_HPP


namespace OptiPack {


//
// Implementation
//


// Constructors, Initialization, Misc.


template<class Scalar>
DiagonalQuadraticResponseOnlyModelEvaluator<Scalar>::DiagonalQuadraticResponseOnlyModelEvaluator(
  const int localDim
  )
  :Np_(1), Ng_(1)
{

  const RCP<const Teuchos::Comm<Thyra::Ordinal> > comm =
    Teuchos::DefaultComm<Thyra::Ordinal>::getComm();

  // Parallel space for p
  p_space_ = Thyra::defaultSpmdVectorSpace<Scalar>(comm, localDim, -1);

  // Locally replicated space for g
  g_space_ = Thyra::defaultSpmdVectorSpace<Scalar>(comm, 1, 1);
}

// Public functions overridden from ModelEvaulator


template<class Scalar>
int DiagonalQuadraticResponseOnlyModelEvaluator<Scalar>::Np() const
{
  return Np_;
}


template<class Scalar>
int DiagonalQuadraticResponseOnlyModelEvaluator<Scalar>::Ng() const
{
  return Ng_;
}


template<class Scalar>
RCP<const Thyra::VectorSpaceBase<Scalar> >
DiagonalQuadraticResponseOnlyModelEvaluator<Scalar>::get_p_space(int l) const
{
#ifdef TEUCHOS_DEBUG
  TEUCHOS_ASSERT_IN_RANGE_UPPER_EXCLUSIVE( l, 0, Np_ );
#endif
  return p_space_;
}


template<class Scalar>
RCP<const Thyra::VectorSpaceBase<Scalar> >
DiagonalQuadraticResponseOnlyModelEvaluator<Scalar>::get_g_space(int j) const
{
#ifdef TEUCHOS_DEBUG
  TEUCHOS_ASSERT_IN_RANGE_UPPER_EXCLUSIVE( j, 0, Ng_ );
#endif
  return g_space_;
}


template<class Scalar>
Thyra::ModelEvaluatorBase::InArgs<Scalar>
DiagonalQuadraticResponseOnlyModelEvaluator<Scalar>::createInArgs() const
{
  typedef Thyra::ModelEvaluatorBase MEB;
  MEB::InArgsSetup<Scalar> inArgs;
  inArgs.setModelEvalDescription(this->description());
  inArgs.set_Np(Np_);
  return inArgs;
}


// Private functions overridden from ModelEvaulatorDefaultBase


template<class Scalar>
Thyra::ModelEvaluatorBase::OutArgs<Scalar>
DiagonalQuadraticResponseOnlyModelEvaluator<Scalar>::createOutArgsImpl() const
{
  typedef Thyra::ModelEvaluatorBase MEB;
  typedef MEB::DerivativeSupport DS;
  MEB::OutArgsSetup<Scalar> outArgs;
  outArgs.setModelEvalDescription(this->description());
  outArgs.set_Np_Ng(Np_,Ng_);
  outArgs.setSupports(MEB::OUT_ARG_DgDp, 0 ,0, MEB::DERIV_MV_BY_COL);
  return outArgs;
}


template<class Scalar>
void DiagonalQuadraticResponseOnlyModelEvaluator<Scalar>::evalModelImpl(
  const Thyra::ModelEvaluatorBase::InArgs<Scalar>& inArgs,
  const Thyra::ModelEvaluatorBase::OutArgs<Scalar>& outArgs
  ) const
{
  TEST_FOR_EXCEPT(true);
}


} // namespace OptiPack


#endif // OPTIPACK_DIAGONAL_QUADRATIC_RESPONSE_ONLY_MODEL_EVALUATOR_DEF_HPP
