//@HEADER
// ************************************************************************
// 
//          Kokkos: Node API and Parallel Node Kernels
//              Copyright (2008) Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
//@HEADER

#ifndef __Kokkos_Raw_SparseMatVec_decl_hpp
#define __Kokkos_Raw_SparseMatVec_decl_hpp

/// \file Kokkos_Raw_SparseMatVec_decl.hpp
/// \brief Declarations of "raw" sequential sparse matrix-vector multiply routines.
/// \warning This code was generated by the SparseMatVec.py script.  
///   If you edit this header by hand, your edits will disappear the 
///   next time you run the generator script.

namespace Kokkos {
namespace Raw {

/// CSC sparse matrix-(multi)vector multiply
///   with column-major input / output (multi)vectors
///
/// \tparam Ordinal The type of indices used to access the entries of
///   the sparse and dense matrices.  Any signed or unsigned integer
///   type which can be used in pointer arithmetic with raw arrays 
///   will do.
/// \tparam MatrixScalar The type of entries in the sparse matrix.
///   This may differ from the type of entries in the input/output
///   matrices.
/// \tparam DomainScalar The type of entries in the input multivector Y.
///   This may differ from the type of entries in the output multivector X.
/// \tparam RangeScalar The type of entries in the output multivector X.
///
/// \param startCol [in] The least (zero-based) column index of 
///   the sparse matrix matrix over which to iterate.  For iterating over 
///   the whole sparse matrix, this should be 0.
/// \param endColPlusOne [in] The largest (zero-based) column index of 
///   the sparse matrix over which to iterate, plus one.  Adding one means 
///   that startCol, endColPlusOne makes an exclusive index range.  For 
///   iterating over the whole sparse matrix, this should be the total
///   number of columns in the sparse matrix (on the calling process).
/// \param numVecs [in] Number of columns in X or Y 
///   (must be the same for both).
/// \param X [out] Output dense multivector, stored in column-major order.
/// \param LDX [in] Stride between columns of X.  We assume unit
///   stride between rows of X.
/// \param ptr [in] Length (numCols+1) array of index offsets 
///   between columns of the sparse matrix.
/// \param ind [in] Array of row indices of the sparse matrix.
///   ind[ptr[i] .. ptr[i+1]-1] are the row indices of
///   column i (zero-based) of the sparse matrix.
/// \param val [in] Array of entries of the sparse matrix.
///   val[ptr[i] .. ptr[i+1]-1] are the entries of column i
///   (zero-based) of the sparse matrix.
/// \param Y [in] Input dense matrix, stored in column-major order.
/// \param LDY [in] Stride between columns of Y.  We assume unit
///   stride between rows of Y.

template<class Ordinal,
         class MatrixScalar,
         class DomainScalar,
         class RangeScalar>
void
matVecCscColMajor (
  const Ordinal numRows,
  const Ordinal startCol,
  const Ordinal endColPlusOne,
  const Ordinal numVecs,
  const RangeScalar& beta,
  RangeScalar Y[],
  const Ordinal colStrideY,
  const RangeScalar alpha,
  const Ordinal ptr[],
  const Ordinal ind[],
  const MatrixScalar val[],
  const DomainScalar X[],
  const Ordinal colStrideX);

/// CSR sparse matrix-(multi)vector multiply
///   with column-major input / output (multi)vectors
///
/// \tparam Ordinal The type of indices used to access the entries of
///   the sparse and dense matrices.  Any signed or unsigned integer
///   type which can be used in pointer arithmetic with raw arrays 
///   will do.
/// \tparam MatrixScalar The type of entries in the sparse matrix.
///   This may differ from the type of entries in the input/output
///   matrices.
/// \tparam DomainScalar The type of entries in the input multivector Y.
///   This may differ from the type of entries in the output multivector X.
/// \tparam RangeScalar The type of entries in the output multivector X.
///
/// \param startRow [in] The least (zero-based) row index of 
///   the sparse matrix matrix over which to iterate.  For iterating over 
///   the whole sparse matrix, this should be 0.
/// \param endRowPlusOne [in] The largest (zero-based) row index of 
///   the sparse matrix over which to iterate, plus one.  Adding one means 
///   that startRow, endRowPlusOne makes an exclusive index range.  For 
///   iterating over the whole sparse matrix, this should be the total
///   number of rows in the sparse matrix (on the calling process).
/// \param numVecs [in] Number of columns in X or Y 
///   (must be the same for both).
/// \param X [out] Output dense multivector, stored in column-major order.
/// \param LDX [in] Stride between columns of X.  We assume unit
///   stride between rows of X.
/// \param ptr [in] Length (numRows+1) array of index offsets 
///   between rows of the sparse matrix.
/// \param ind [in] Array of column indices of the sparse matrix.
///   ind[ptr[i] .. ptr[i+1]-1] are the column indices of
///   row i (zero-based) of the sparse matrix.
/// \param val [in] Array of entries of the sparse matrix.
///   val[ptr[i] .. ptr[i+1]-1] are the entries of row i
///   (zero-based) of the sparse matrix.
/// \param Y [in] Input dense matrix, stored in column-major order.
/// \param LDY [in] Stride between columns of Y.  We assume unit
///   stride between rows of Y.

template<class Ordinal,
         class MatrixScalar,
         class DomainScalar,
         class RangeScalar>
void
matVecCsrColMajor (
  const Ordinal numRows,
  const Ordinal startRow,
  const Ordinal endRowPlusOne,
  const Ordinal numVecs,
  const RangeScalar& beta,
  RangeScalar Y[],
  const Ordinal colStrideY,
  const RangeScalar alpha,
  const Ordinal ptr[],
  const Ordinal ind[],
  const MatrixScalar val[],
  const DomainScalar X[],
  const Ordinal colStrideX);

/// CSC sparse matrix-(multi)vector multiply
///   with row-major input / output (multi)vectors
///
/// \tparam Ordinal The type of indices used to access the entries of
///   the sparse and dense matrices.  Any signed or unsigned integer
///   type which can be used in pointer arithmetic with raw arrays 
///   will do.
/// \tparam MatrixScalar The type of entries in the sparse matrix.
///   This may differ from the type of entries in the input/output
///   matrices.
/// \tparam DomainScalar The type of entries in the input multivector Y.
///   This may differ from the type of entries in the output multivector X.
/// \tparam RangeScalar The type of entries in the output multivector X.
///
/// \param startCol [in] The least (zero-based) column index of 
///   the sparse matrix matrix over which to iterate.  For iterating over 
///   the whole sparse matrix, this should be 0.
/// \param endColPlusOne [in] The largest (zero-based) column index of 
///   the sparse matrix over which to iterate, plus one.  Adding one means 
///   that startCol, endColPlusOne makes an exclusive index range.  For 
///   iterating over the whole sparse matrix, this should be the total
///   number of columns in the sparse matrix (on the calling process).
/// \param numVecs [in] Number of columns in X or Y 
///   (must be the same for both).
/// \param X [out] Output dense multivector, stored in row-major order.
/// \param LDX [in] Stride between rows of X.  We assume unit
///   stride between columns of X.
/// \param ptr [in] Length (numCols+1) array of index offsets 
///   between columns of the sparse matrix.
/// \param ind [in] Array of row indices of the sparse matrix.
///   ind[ptr[i] .. ptr[i+1]-1] are the row indices of
///   column i (zero-based) of the sparse matrix.
/// \param val [in] Array of entries of the sparse matrix.
///   val[ptr[i] .. ptr[i+1]-1] are the entries of column i
///   (zero-based) of the sparse matrix.
/// \param Y [in] Input dense matrix, stored in row-major order.
/// \param LDY [in] Stride between rows of Y.  We assume unit
///   stride between columns of Y.

template<class Ordinal,
         class MatrixScalar,
         class DomainScalar,
         class RangeScalar>
void
matVecCscRowMajor (
  const Ordinal numRows,
  const Ordinal startCol,
  const Ordinal endColPlusOne,
  const Ordinal numVecs,
  const RangeScalar& beta,
  RangeScalar Y[],
  const Ordinal rowStrideY,
  const RangeScalar alpha,
  const Ordinal ptr[],
  const Ordinal ind[],
  const MatrixScalar val[],
  const DomainScalar X[],
  const Ordinal rowStrideX);

/// CSR sparse matrix-(multi)vector multiply
///   with row-major input / output (multi)vectors
///
/// \tparam Ordinal The type of indices used to access the entries of
///   the sparse and dense matrices.  Any signed or unsigned integer
///   type which can be used in pointer arithmetic with raw arrays 
///   will do.
/// \tparam MatrixScalar The type of entries in the sparse matrix.
///   This may differ from the type of entries in the input/output
///   matrices.
/// \tparam DomainScalar The type of entries in the input multivector Y.
///   This may differ from the type of entries in the output multivector X.
/// \tparam RangeScalar The type of entries in the output multivector X.
///
/// \param startRow [in] The least (zero-based) row index of 
///   the sparse matrix matrix over which to iterate.  For iterating over 
///   the whole sparse matrix, this should be 0.
/// \param endRowPlusOne [in] The largest (zero-based) row index of 
///   the sparse matrix over which to iterate, plus one.  Adding one means 
///   that startRow, endRowPlusOne makes an exclusive index range.  For 
///   iterating over the whole sparse matrix, this should be the total
///   number of rows in the sparse matrix (on the calling process).
/// \param numVecs [in] Number of columns in X or Y 
///   (must be the same for both).
/// \param X [out] Output dense multivector, stored in row-major order.
/// \param LDX [in] Stride between rows of X.  We assume unit
///   stride between columns of X.
/// \param ptr [in] Length (numRows+1) array of index offsets 
///   between rows of the sparse matrix.
/// \param ind [in] Array of column indices of the sparse matrix.
///   ind[ptr[i] .. ptr[i+1]-1] are the column indices of
///   row i (zero-based) of the sparse matrix.
/// \param val [in] Array of entries of the sparse matrix.
///   val[ptr[i] .. ptr[i+1]-1] are the entries of row i
///   (zero-based) of the sparse matrix.
/// \param Y [in] Input dense matrix, stored in row-major order.
/// \param LDY [in] Stride between rows of Y.  We assume unit
///   stride between columns of Y.

template<class Ordinal,
         class MatrixScalar,
         class DomainScalar,
         class RangeScalar>
void
matVecCsrRowMajor (
  const Ordinal numRows,
  const Ordinal startRow,
  const Ordinal endRowPlusOne,
  const Ordinal numVecs,
  const RangeScalar& beta,
  RangeScalar Y[],
  const Ordinal rowStrideY,
  const RangeScalar alpha,
  const Ordinal ptr[],
  const Ordinal ind[],
  const MatrixScalar val[],
  const DomainScalar X[],
  const Ordinal rowStrideX);

/// CSC sparse matrix-(multi)vector multiply
///   with column-major input / output (multi)vectors
///   using conjugate of sparse matrix entries
///
/// \tparam Ordinal The type of indices used to access the entries of
///   the sparse and dense matrices.  Any signed or unsigned integer
///   type which can be used in pointer arithmetic with raw arrays 
///   will do.
/// \tparam MatrixScalar The type of entries in the sparse matrix.
///   This may differ from the type of entries in the input/output
///   matrices.
/// \tparam DomainScalar The type of entries in the input multivector Y.
///   This may differ from the type of entries in the output multivector X.
/// \tparam RangeScalar The type of entries in the output multivector X.
///
/// \param startCol [in] The least (zero-based) column index of 
///   the sparse matrix matrix over which to iterate.  For iterating over 
///   the whole sparse matrix, this should be 0.
/// \param endColPlusOne [in] The largest (zero-based) column index of 
///   the sparse matrix over which to iterate, plus one.  Adding one means 
///   that startCol, endColPlusOne makes an exclusive index range.  For 
///   iterating over the whole sparse matrix, this should be the total
///   number of columns in the sparse matrix (on the calling process).
/// \param numVecs [in] Number of columns in X or Y 
///   (must be the same for both).
/// \param X [out] Output dense multivector, stored in column-major order.
/// \param LDX [in] Stride between columns of X.  We assume unit
///   stride between rows of X.
/// \param ptr [in] Length (numCols+1) array of index offsets 
///   between columns of the sparse matrix.
/// \param ind [in] Array of row indices of the sparse matrix.
///   ind[ptr[i] .. ptr[i+1]-1] are the row indices of
///   column i (zero-based) of the sparse matrix.
/// \param val [in] Array of entries of the sparse matrix.
///   val[ptr[i] .. ptr[i+1]-1] are the entries of column i
///   (zero-based) of the sparse matrix.
/// \param Y [in] Input dense matrix, stored in column-major order.
/// \param LDY [in] Stride between columns of Y.  We assume unit
///   stride between rows of Y.

template<class Ordinal,
         class MatrixScalar,
         class DomainScalar,
         class RangeScalar>
void
matVecCscColMajorConj (
  const Ordinal numRows,
  const Ordinal startCol,
  const Ordinal endColPlusOne,
  const Ordinal numVecs,
  const RangeScalar& beta,
  RangeScalar Y[],
  const Ordinal colStrideY,
  const RangeScalar alpha,
  const Ordinal ptr[],
  const Ordinal ind[],
  const MatrixScalar val[],
  const DomainScalar X[],
  const Ordinal colStrideX);

/// CSR sparse matrix-(multi)vector multiply
///   with column-major input / output (multi)vectors
///   using conjugate of sparse matrix entries
///
/// \tparam Ordinal The type of indices used to access the entries of
///   the sparse and dense matrices.  Any signed or unsigned integer
///   type which can be used in pointer arithmetic with raw arrays 
///   will do.
/// \tparam MatrixScalar The type of entries in the sparse matrix.
///   This may differ from the type of entries in the input/output
///   matrices.
/// \tparam DomainScalar The type of entries in the input multivector Y.
///   This may differ from the type of entries in the output multivector X.
/// \tparam RangeScalar The type of entries in the output multivector X.
///
/// \param startRow [in] The least (zero-based) row index of 
///   the sparse matrix matrix over which to iterate.  For iterating over 
///   the whole sparse matrix, this should be 0.
/// \param endRowPlusOne [in] The largest (zero-based) row index of 
///   the sparse matrix over which to iterate, plus one.  Adding one means 
///   that startRow, endRowPlusOne makes an exclusive index range.  For 
///   iterating over the whole sparse matrix, this should be the total
///   number of rows in the sparse matrix (on the calling process).
/// \param numVecs [in] Number of columns in X or Y 
///   (must be the same for both).
/// \param X [out] Output dense multivector, stored in column-major order.
/// \param LDX [in] Stride between columns of X.  We assume unit
///   stride between rows of X.
/// \param ptr [in] Length (numRows+1) array of index offsets 
///   between rows of the sparse matrix.
/// \param ind [in] Array of column indices of the sparse matrix.
///   ind[ptr[i] .. ptr[i+1]-1] are the column indices of
///   row i (zero-based) of the sparse matrix.
/// \param val [in] Array of entries of the sparse matrix.
///   val[ptr[i] .. ptr[i+1]-1] are the entries of row i
///   (zero-based) of the sparse matrix.
/// \param Y [in] Input dense matrix, stored in column-major order.
/// \param LDY [in] Stride between columns of Y.  We assume unit
///   stride between rows of Y.

template<class Ordinal,
         class MatrixScalar,
         class DomainScalar,
         class RangeScalar>
void
matVecCsrColMajorConj (
  const Ordinal numRows,
  const Ordinal startRow,
  const Ordinal endRowPlusOne,
  const Ordinal numVecs,
  const RangeScalar& beta,
  RangeScalar Y[],
  const Ordinal colStrideY,
  const RangeScalar alpha,
  const Ordinal ptr[],
  const Ordinal ind[],
  const MatrixScalar val[],
  const DomainScalar X[],
  const Ordinal colStrideX);

/// CSC sparse matrix-(multi)vector multiply
///   with row-major input / output (multi)vectors
///   using conjugate of sparse matrix entries
///
/// \tparam Ordinal The type of indices used to access the entries of
///   the sparse and dense matrices.  Any signed or unsigned integer
///   type which can be used in pointer arithmetic with raw arrays 
///   will do.
/// \tparam MatrixScalar The type of entries in the sparse matrix.
///   This may differ from the type of entries in the input/output
///   matrices.
/// \tparam DomainScalar The type of entries in the input multivector Y.
///   This may differ from the type of entries in the output multivector X.
/// \tparam RangeScalar The type of entries in the output multivector X.
///
/// \param startCol [in] The least (zero-based) column index of 
///   the sparse matrix matrix over which to iterate.  For iterating over 
///   the whole sparse matrix, this should be 0.
/// \param endColPlusOne [in] The largest (zero-based) column index of 
///   the sparse matrix over which to iterate, plus one.  Adding one means 
///   that startCol, endColPlusOne makes an exclusive index range.  For 
///   iterating over the whole sparse matrix, this should be the total
///   number of columns in the sparse matrix (on the calling process).
/// \param numVecs [in] Number of columns in X or Y 
///   (must be the same for both).
/// \param X [out] Output dense multivector, stored in row-major order.
/// \param LDX [in] Stride between rows of X.  We assume unit
///   stride between columns of X.
/// \param ptr [in] Length (numCols+1) array of index offsets 
///   between columns of the sparse matrix.
/// \param ind [in] Array of row indices of the sparse matrix.
///   ind[ptr[i] .. ptr[i+1]-1] are the row indices of
///   column i (zero-based) of the sparse matrix.
/// \param val [in] Array of entries of the sparse matrix.
///   val[ptr[i] .. ptr[i+1]-1] are the entries of column i
///   (zero-based) of the sparse matrix.
/// \param Y [in] Input dense matrix, stored in row-major order.
/// \param LDY [in] Stride between rows of Y.  We assume unit
///   stride between columns of Y.

template<class Ordinal,
         class MatrixScalar,
         class DomainScalar,
         class RangeScalar>
void
matVecCscRowMajorConj (
  const Ordinal numRows,
  const Ordinal startCol,
  const Ordinal endColPlusOne,
  const Ordinal numVecs,
  const RangeScalar& beta,
  RangeScalar Y[],
  const Ordinal rowStrideY,
  const RangeScalar alpha,
  const Ordinal ptr[],
  const Ordinal ind[],
  const MatrixScalar val[],
  const DomainScalar X[],
  const Ordinal rowStrideX);

/// CSR sparse matrix-(multi)vector multiply
///   with row-major input / output (multi)vectors
///   using conjugate of sparse matrix entries
///
/// \tparam Ordinal The type of indices used to access the entries of
///   the sparse and dense matrices.  Any signed or unsigned integer
///   type which can be used in pointer arithmetic with raw arrays 
///   will do.
/// \tparam MatrixScalar The type of entries in the sparse matrix.
///   This may differ from the type of entries in the input/output
///   matrices.
/// \tparam DomainScalar The type of entries in the input multivector Y.
///   This may differ from the type of entries in the output multivector X.
/// \tparam RangeScalar The type of entries in the output multivector X.
///
/// \param startRow [in] The least (zero-based) row index of 
///   the sparse matrix matrix over which to iterate.  For iterating over 
///   the whole sparse matrix, this should be 0.
/// \param endRowPlusOne [in] The largest (zero-based) row index of 
///   the sparse matrix over which to iterate, plus one.  Adding one means 
///   that startRow, endRowPlusOne makes an exclusive index range.  For 
///   iterating over the whole sparse matrix, this should be the total
///   number of rows in the sparse matrix (on the calling process).
/// \param numVecs [in] Number of columns in X or Y 
///   (must be the same for both).
/// \param X [out] Output dense multivector, stored in row-major order.
/// \param LDX [in] Stride between rows of X.  We assume unit
///   stride between columns of X.
/// \param ptr [in] Length (numRows+1) array of index offsets 
///   between rows of the sparse matrix.
/// \param ind [in] Array of column indices of the sparse matrix.
///   ind[ptr[i] .. ptr[i+1]-1] are the column indices of
///   row i (zero-based) of the sparse matrix.
/// \param val [in] Array of entries of the sparse matrix.
///   val[ptr[i] .. ptr[i+1]-1] are the entries of row i
///   (zero-based) of the sparse matrix.
/// \param Y [in] Input dense matrix, stored in row-major order.
/// \param LDY [in] Stride between rows of Y.  We assume unit
///   stride between columns of Y.

template<class Ordinal,
         class MatrixScalar,
         class DomainScalar,
         class RangeScalar>
void
matVecCsrRowMajorConj (
  const Ordinal numRows,
  const Ordinal startRow,
  const Ordinal endRowPlusOne,
  const Ordinal numVecs,
  const RangeScalar& beta,
  RangeScalar Y[],
  const Ordinal rowStrideY,
  const RangeScalar alpha,
  const Ordinal ptr[],
  const Ordinal ind[],
  const MatrixScalar val[],
  const DomainScalar X[],
  const Ordinal rowStrideX);

} // namespace Raw
} // namespace Kokkos

#endif // #ifndef __Kokkos_Raw_SparseMatVec_decl_hpp
