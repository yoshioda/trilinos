/** \HEADER
 *************************************************************************
 *
 *                            Kokkos
 *                 Copyright 2010 Sandia Corporation
 *
 *  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
 *  the U.S. Government retains certain rights in this software.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 *  3. Neither the name of the Corporation nor the names of the
 *  contributors may be used to endorse or promote products derived from
 *  this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************
 */

#include <gtest/gtest.h>

#ifndef KOKKOS_MACRO_DEVICE
#error "KOKKOS_MACRO_DEVICE undefined"
#endif

#include <stdexcept>
#include <sstream>
#include <iostream>

#include <impl/Kokkos_Preprocessing_macros.hpp>

/*--------------------------------------------------------------------------*/

namespace {

template< typename T, class > class TestMultiVector ;

template<typename T>
class TestMultiVector< T, KOKKOS_MACRO_DEVICE >
{
public:
  typedef KOKKOS_MACRO_DEVICE device ;
  typedef Kokkos::Host        host ;

  typedef Kokkos::MultiVector< T , device > dView ;
  typedef typename dView::HostView hView ;

  TestMultiVector() { run_test(); }

  void run_test()
  {
    enum { LENGTH = 1000 };
    dView dx , dy , dz ;
    hView hx , hy , hz ;
    ASSERT_FALSE(dx);
    ASSERT_FALSE(dy);
    ASSERT_FALSE(dz);
    ASSERT_FALSE(hx);
    ASSERT_FALSE(hy);
    ASSERT_FALSE(hz);

    dx = Kokkos::create_labeled_multivector<T,device> ( "dx" , LENGTH , 2 );
    dy = Kokkos::create_labeled_multivector<T,device> ( "dy" , LENGTH , 2 );

    ASSERT_TRUE(dx);
    ASSERT_TRUE(dy);
    ASSERT_NE( dx , dy );

    ASSERT_EQ( dx.length() , LENGTH );
    ASSERT_EQ( dx.count() , (device::size_type) 2 );
    ASSERT_EQ( dy.length() , LENGTH );
    ASSERT_EQ( dy.count() , (device::size_type) 2 );

    hx = Kokkos::create_mirror( dx );
    hy = Kokkos::create_mirror( dy );

    for ( size_t i = 0 ; i < LENGTH ; ++i ) {
      hx(i,0) = i ;
      hx(i,1) = 2 * i ;
    }

    Kokkos::deep_copy( dx , hx );
    Kokkos::deep_copy( dy , dx );
    Kokkos::deep_copy( hy , dy );

    for ( size_t i = 0 ; i < LENGTH ; ++i ) {
      ASSERT_EQ( hx(i,0) , hy(i,0) );
      ASSERT_EQ( hx(i,1) , hy(i,1) );
    }

    dz = dx ; ASSERT_EQ( dx, dz); ASSERT_NE( dy, dz);
    dz = dy ; ASSERT_EQ( dy, dz); ASSERT_NE( dx, dz);

    dx = dView();
    ASSERT_FALSE(dx);
    ASSERT_TRUE( dy);
    ASSERT_TRUE( dz);
    dy = dView();
    ASSERT_FALSE(dx);
    ASSERT_FALSE(dy);
    ASSERT_TRUE( dz);
    dz = dView();
    ASSERT_FALSE(dx);
    ASSERT_FALSE(dy);
    ASSERT_FALSE(dz);
  }
};

}

/*--------------------------------------------------------------------------*/
