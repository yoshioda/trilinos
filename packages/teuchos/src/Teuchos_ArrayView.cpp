// @HEADER
// ***********************************************************************
//
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
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
// ***********************************************************************
// @HEADER

#include <Teuchos_ArrayView.hpp>

namespace Teuchos {

// Specialization for float.  We use sufficient precision that no
// digits are lost after writing to string and reading back in again.
template<>
TEUCHOS_LIB_DLL_EXPORT std::string
ArrayView<float>::toString() const
{
  using Teuchos::as;
  std::ostringstream ss;

  debug_assert_valid_ptr();

  ss.setf (std::ios::scientific);
  // 8 = round(23 * log10(2)) + 1.  That's one decimal digit more
  // than the binary precision justifies, which should be plenty.
  // Guy Steele et al. have a better algorithm for floating-point
  // I/O, but using a lot of digits is the lazy approach.
  ss.precision (8);
  ss << "{";
  for (size_type i = 0; i < size (); ++i) {
    ss << operator[] (i);
    if (i + 1 < size ()) {
      ss << ", ";
    }
  }
  ss << "}";
  return ss.str ();
}

// Specialization for (const) float.  We use sufficient precision that no
// digits are lost after writing to string and reading back in again.
template<>
TEUCHOS_LIB_DLL_EXPORT std::string
ArrayView<const float>::toString() const
{
  using Teuchos::as;
  std::ostringstream ss;

  debug_assert_valid_ptr();

  ss.setf (std::ios::scientific);
  // 8 = round(23 * log10(2)) + 1.  That's one decimal digit more
  // than the binary precision justifies, which should be plenty.
  // Guy Steele et al. have a better algorithm for floating-point
  // I/O, but using a lot of digits is the lazy approach.
  ss.precision (8);
  ss << "{";
  for (size_type i = 0; i < size (); ++i) {
    ss << operator[] (i);
    if (i + 1 < size ()) {
      ss << ", ";
    }
  }
  ss << "}";
  return ss.str ();
}

// Specialization for double.  We use sufficient precision that no
// digits are lost after writing to string and reading back in again.
template<>
TEUCHOS_LIB_DLL_EXPORT std::string
ArrayView<double>::toString() const
{
  using Teuchos::as;
  std::ostringstream ss;

  debug_assert_valid_ptr();

  ss.setf (std::ios::scientific);
  // 17 = round(52 * log10(2)) + 1.  That's one decimal digit more
  // than the binary precision justifies, which should be plenty.  Guy
  // Steele et al. have a better algorithm for floating-point I/O, but
  // using a lot of digits is the lazy approach.
  ss.precision (17);
  ss << "{";
  for (size_type i = 0; i < size (); ++i) {
    ss << operator[] (i);
    if (i + 1 < size ()) {
      ss << ", ";
    }
  }
  ss << "}";
  return ss.str ();
}

// Specialization for (const) double.  We use sufficient precision that no
// digits are lost after writing to string and reading back in again.
template<>
TEUCHOS_LIB_DLL_EXPORT std::string
ArrayView<const double>::toString() const
{
  using Teuchos::as;
  std::ostringstream ss;

  debug_assert_valid_ptr();

  ss.setf (std::ios::scientific);
  // 17 = round(52 * log10(2)) + 1.  That's one decimal digit more
  // than the binary precision justifies, which should be plenty.  Guy
  // Steele et al. have a better algorithm for floating-point I/O, but
  // using a lot of digits is the lazy approach.
  ss.precision (17);
  ss << "{";
  for (size_type i = 0; i < size (); ++i) {
    ss << operator[] (i);
    if (i + 1 < size ()) {
      ss << ", ";
    }
  }
  ss << "}";
  return ss.str ();
}

} // namespace Teuchos
