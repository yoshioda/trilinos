// @HEADER
/*
************************************************************************

           NOX: An Object-Oriented Nonlinear Solver Package
                Copyright (2002) Sandia Corporation

           LOCA: Library of Continuation Algorithms Package
                Copyright (2005) Sandia Corporation

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

Questions? Contact Roger Pawlowski (rppawlo@sandia.gov) or 
Eric Phipps (etphipp@sandia.gov), Sandia National Laboratories.
************************************************************************
 CVS Information
 $Source$
 $Author$
 $Date$
 $Revision$
************************************************************************
*/
// @HEADER

#ifndef EPETRA_VECTORHELPER_H
#define EPETRA_VECTORHELPER_H

#include "Python.h"

// Forward declarations
class Epetra_Vector  ;
class NumPyArrayBase ;
class Epetra_BlockMap;

// Class to hold static functions used by the swig Epetra interface
class Epetra_VectorHelper
{
public:
  static Epetra_Vector * new_Epetra_Vector(Epetra_BlockMap & map ,
					   PyObject        * p_pyObject);

  static void            loadViaCopy      (Epetra_Vector * p_epetraVector,
					   PyObject      * p_pyObject);
//   static void            loadViaCopy      (Epetra_Vector & epetraVector,
// 					   const NumPyArrayBase & numPyArray);
  static void            unloadViaCopy    (const Epetra_Vector * p_epetraVector,
					   PyObject            * p_pyObject);
//   static void            unloadViaCopy    (const Epetra_Vector & epetraVector,
// 					   NumPyArrayBase      & numPyArray);

protected:
  // Class should never be instantiated, it just holds
  // static functions
  Epetra_VectorHelper(PyObject * p_pyObject);
  ~Epetra_VectorHelper();

private:
  // Private and not implemented
  Epetra_VectorHelper();
  Epetra_VectorHelper(const Epetra_VectorHelper &);
  Epetra_VectorHelper & operator=(const Epetra_VectorHelper &);
};


#endif // EPETRA_VECTORHELPER_H
