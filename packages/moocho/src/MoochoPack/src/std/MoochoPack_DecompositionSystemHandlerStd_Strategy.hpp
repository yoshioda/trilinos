// @HEADER
// ***********************************************************************
// 
// Moocho: Multi-functional Object-Oriented arCHitecture for Optimization
//                  Copyright (2003) Sandia Corporation
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

#ifndef DECOMPOSITION_SYSTEM_HANDLER_STD_STRATEGY_H
#define DECOMPOSITION_SYSTEM_HANDLER_STD_STRATEGY_H

#include "MoochoPack_DecompositionSystemHandler_Strategy.hpp"

namespace MoochoPack {

/** \brief Subclass for updating the range/null space decomposition using
 * the base DecompositionSystem interface only.
 *
 * ToDo: Finish documentation!
 */
class DecompositionSystemHandlerStd_Strategy
  : public DecompositionSystemHandler_Strategy
{
public:
  
  /** @name Constructors / initializers */
  //@{

  /** \brief Constructor
   */
  DecompositionSystemHandlerStd_Strategy();

  //@}

  /** @name Overridden from DecompositionSystemHandler_Strategy */
  //@{

  /** \brief . */
  bool update_decomposition(
    NLPAlgo                                &algo
    ,NLPAlgoState                          &s
    ,NLPFirstOrder                         &nlp
    ,EDecompSysTesting                     decomp_sys_testing
    ,EDecompSysPrintLevel                  decomp_sys_testing_print_level
    ,bool                                  *new_decomp_selected
    );
  /** \brief . */
  void print_update_decomposition(
    const NLPAlgo                          &algo
    ,const NLPAlgoState                    &s
    ,std::ostream                          &out
    ,const std::string                     &leading_spaces
    ) const;

  //@}

}; // end class DecompositionSystemHandlerStd_Strategy

} // end namespace MoochoPack

#endif // DECOMPOSITION_SYSTEM_HANDLER_STD_STRATEGY_H
