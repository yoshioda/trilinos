/*
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
*/

#ifndef RTOP_REDUCT_MAX_VALUE_H
#define RTOP_REDUCT_MAX_VALUE_H

#include "RTOp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @name Definitions of reduction functions for max(.,.) scalar values.
 *
 * These functions perform a simple <tt>max()</tt> of a scalar reduction objects
 * as defined by the virtual function table \Ref{RTOp_obj_value_vtbl}.
 */
/*@{ */

/* */
/** Use this function for <tt>reduce_reduct_objs</tt> in the RTOp_RTOp_vtbl_t virtual
 * function table.
 */
int RTOp_reduct_max_value(
	const struct RTOp_RTOp_vtbl_t* vtbl, const void* obj_data
	, RTOp_ReductTarget in_targ_obj, RTOp_ReductTarget inout_targ_obj );

/* */
/** Use this function for <tt>get_reduct_op</tt> in the RTOp_RTOp_vtbl_t virtual
 * function table.
 */
int RTOp_get_reduct_max_value_op(
	const struct RTOp_RTOp_vtbl_t* vtbl, const void* obj_data
	, RTOp_reduct_op_func_ptr_t* reduct_op_func_ptr );

/*@} */

#ifdef __cplusplus
}
#endif

#endif /* RTOP_REDUCT_MAX_VALUE_H */
