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

#ifndef RELEASE_RESOURCE_H
#define RELEASE_RESOURCE_H

namespace MemMngPack {

///
/** Abstract interface for releasing an object when it is
 * not needed anymore {abstract}.
 *
 * The purpose of this object is so that a client can give another
 * peer a means to release needed resources when that object is
 * not bound anymore.
 */
class ReleaseResource {
public:

	///
	/** When object is deleted so will the resource if it is not
	 * needed anymore.
	 */
	virtual ~ReleaseResource() {}

	///
	/** Returns true if a resource is bound to this object.
	 */
	virtual bool resource_is_bound() const = 0;

};

}  // end namespace MemMngPack

#endif // RELEASE_RESOURCE_H
