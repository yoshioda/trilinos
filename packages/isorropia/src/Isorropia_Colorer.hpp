//@HEADER
/*
************************************************************************

              Isorropia: Partitioning and Load Balancing Package
                Copyright (2006) Sandia Corporation

Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
license for use of this work by or on behalf of the U.S. Government.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA

************************************************************************
*/
//@HEADER

#ifndef _Isorropia_Colorer_hpp_
#define _Isorropia_Colorer_hpp_

#include <Isorropia_ConfigDefs.hpp>
#include <Teuchos_ParameterList.hpp>
#include <Isorropia_Operator.hpp>

namespace Isorropia {

/** Interface (abstract base class) for computing a new coloring and
    describing the result.

    If the methods which describe the new partitioning (e.g.,
  newPartitionNumber(), etc.) are called before compute_partitioning()
  has been called, behavior is not well defined. Implementations will
  either return empty/erroneous data, or throw an exception. In most
  cases, implementations will probably call compute_partitioning()
  internally in a constructor or factory method, so this won't usually
  be an issue.
*/
class Colorer : virtual public Operator {
public:

  /** Destructor */
  virtual ~Colorer() {}

  /** Method which does the work of computing a new coloring.

     \param forceColoring Optional argument defaults to false.
       Depending on the implementation, color() should only perform a
       coloring the first time it is called, and subsequent repeated
       calls are no-ops. If the user's intent is to re-compute the
       coloring (e.g., if parameters or other inputs have been
       changed), then setting this flag to true will force a new
       coloring to be computed.
   */
  virtual void color(bool forceColoring=false) = 0;

  /** Method which returns the number (global) of colors used.
   */
  virtual int numColors() const {
      return numProperties(); }

  /** Return the number of elements of a given color.
   */
  virtual int numElemsWithColor(int color) const
  { return numElemsWithProperty(color); }

  /** Fill user-allocated list (of length len) with the
   *  local element ids for LOCAL elements of the given color.
   */
  virtual void elemsWithColor(int color,
			      int* elementList,
			      int len) const {
    return elemsWithProperty(color, elementList, len);}

};//class Colorer

}//namespace Isorropia

#endif

