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

#ifndef NLP_FIRST_ORDER_DIRECT_TESTER_SET_OPTIONS_H
#define NLP_FIRST_ORDER_DIRECT_TESTER_SET_OPTIONS_H

#include "NLPInterfacePack_NLPDirectTester.hpp"
#include "OptionsFromStreamPack_SetOptionsFromStreamNode.hpp"
#include "OptionsFromStreamPack_SetOptionsToTargetBase.hpp"

namespace NLPInterfacePack {

/** \brief Set options for <tt>NLPDirectTester</tt> from an
  * <tt>\ref OptionsFromStreamPack::OptionsFromStream "OptionsFromStream"</tt> object.
  *
  * The default options group name is 'NLPDirectTester'.
  *
  * The options group is:
  \verbatim

  options_group NLPDirectTester {
  *    Gf_testing_method = FD_COMPUTE_ALL;
      Gf_testing_method = FD_DIRECTIONAL;
      Gf_warning_tol    = 1e-6;
      Gf_error_tol      = 1e-1;
  *    Gc_testing_method = FD_COMPUTE_ALL;
      Gc_testing_method = FD_DIRECTIONAL;
      Gc_warning_tol    = 1e-6;
      Gc_error_tol      = 1e-1;
    num_fd_directions = 3;  *** [testing_method == FD_DIRECTIONAL]
    dump_all = false;
  }
  \endverbatim
  */
class NLPDirectTesterSetOptions
  : public OptionsFromStreamPack::SetOptionsFromStreamNode 
    , public OptionsFromStreamPack::SetOptionsToTargetBase<
      NLPDirectTester >
{
public:

  /** \brief . */
  NLPDirectTesterSetOptions(
      NLPDirectTester* target = 0
    , const char opt_grp_name[] = "NLPDirectTester" );

protected:

  /// Overridden from SetOptionsFromStreamNode
  void setOption( int option_num, const std::string& option_value );

};	// end class NLPDirectTesterSetOptions

}	// end namespace NLPInterfacePack

#endif	// NLP_FIRST_ORDER_DIRECT_TESTER_SET_OPTIONS_H
