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

#if ! defined(KOKKOS_MACRO_DEVICE_TEMPLATE_SPECIALIZATION) || \
    ! defined(KOKKOS_MACRO_DEVICE)                  || \
    ! defined(KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION)

#error "Including <Kokkos_MultiVector_macros.hpp> without macros defined"

#else

namespace Kokkos {

//----------------------------------------------------------------------------
/** \brief  Plain-old-data value allocated on a compute device.
 */
template< typename ValueType >
class MultiVector< ValueType , KOKKOS_MACRO_DEVICE > {
public:
  typedef ValueType                  value_type ;
  typedef KOKKOS_MACRO_DEVICE        device_type ;
  typedef device_type::size_type     size_type ;

  typedef MultiVector< value_type , Host > HostView ;

public:

  /*------------------------------------------------------------------*/
  /** \brief  Query length of vectors */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  size_type length() const { return m_length ; }
  
  /** \brief  Query count of vectors */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  size_type count()  const { return m_count ; }
  
  /** \brief  Query if NULL view */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  operator bool ()  const { return 0 != m_ptr_on_device ; }
  
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  bool operator == ( const MultiVector & rhs ) const
  {
    return m_ptr_on_device == rhs.m_ptr_on_device &&
           m_count         == rhs.m_count ;
  }
  
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  bool operator != ( const MultiVector & rhs ) const
  {
    return m_ptr_on_device != rhs.m_ptr_on_device ||
           m_count         != rhs.m_count ;
  }

  template< class DeviceRHS >
  inline
  bool operator == ( const MultiVector< value_type , DeviceRHS > & rhs ) const
  {
    return Impl::SameType< typename device_type::memory_space ,
                           typename DeviceRHS  ::memory_space >::value &&
           m_ptr_on_device == rhs.m_ptr_on_device &&
           m_count         == rhs.m_count ;
  }

  template< class DeviceRHS >
  inline
  bool operator != ( const MultiVector< value_type , DeviceRHS > & rhs ) const
  {
    return ( ! Impl::SameType< typename device_type::memory_space ,
                               typename DeviceRHS  ::memory_space >::value ) ||
           m_ptr_on_device != rhs.m_ptr_on_device ||
           m_count         != rhs.m_count ;
  }

  /*------------------------------------------------------------------*/
  /** \brief  Because memory is contiguous this is exposed */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  value_type * ptr_on_device() const { return m_ptr_on_device ; }
  
  /*------------------------------------------------------------------*/

#if defined(KOKKOS_MACRO_DEVICE_FUNCTION)

  /** \brief  Query value */
  template< typename iTypeP , typename iTypeV >
  KOKKOS_MACRO_DEVICE_FUNCTION
  value_type & operator()( const iTypeP & iP , const iTypeV & iV ) const
    { return m_ptr_on_device[ iP + m_stride * iV ]; }
  
  template< typename iTypeP >
  KOKKOS_MACRO_DEVICE_FUNCTION
  value_type & operator()( const iTypeP & iP ) const
    { return m_ptr_on_device[ iP ]; }

#endif /* defined(KOKKOS_MACRO_DEVICE_FUNCTION) */

  /*------------------------------------------------------------------*/
  /** \brief  Construct a NULL view */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  MultiVector()
    : m_memory()
    , m_ptr_on_device(0)
    , m_stride(0)
    , m_length(0)
    , m_count(0)
    {}

  /** \brief  Construct a view of the array */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  MultiVector( const MultiVector & rhs )
    : m_memory(        rhs.m_memory )
    , m_ptr_on_device( rhs.m_ptr_on_device )
    , m_stride(        rhs.m_stride )
    , m_length(        rhs.m_length )
    , m_count(         rhs.m_count )
    {}

  /** \brief  Assign to a view of the rhs.
   *          If the old view is the last view
   *          then allocated memory is deallocated.
   */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  MultiVector & operator = ( const MultiVector & rhs )
    {
      m_memory.operator=( rhs.m_memory );
      m_ptr_on_device  = rhs.m_ptr_on_device ;
      m_stride         = rhs.m_stride ;
      m_length         = rhs.m_length ;
      m_count          = rhs.m_count ;
      return *this ;
    }
  
  /**  \brief  Destroy this view of the value.
   *           If the last view then allocated memory is deallocated.
   */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  ~MultiVector()
    {
      m_ptr_on_device = 0 ;
      m_stride        = 0 ;
      m_length        = 0 ;
      m_count         = 0 ;
    }

  /*------------------------------------------------------------------*/
  /* \brief  Construct a view to a range of vectors */
  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  MultiVector( const MultiVector & rhs , size_type iBeg ,
                                         size_type iEnd )
    : m_memory(        rhs.m_memory )
    , m_ptr_on_device( rhs.m_ptr_on_device + rhs.m_stride * iBeg )
    , m_stride(        rhs.m_stride )
    , m_length(        rhs.m_length )
    , m_count(         iEnd - iBeg )
    { KOKKOS_MACRO_DEVICE_CAN_THROW( Impl::multivector_require_range( iBeg , iEnd , rhs.m_count ) ); }

  inline
  KOKKOS_MACRO_DEVICE_AND_HOST_FUNCTION
  MultiVector( const MultiVector & rhs , size_type iBeg )
    : m_memory(        rhs.m_memory )
    , m_ptr_on_device( rhs.m_ptr_on_device + rhs.m_stride * iBeg )
    , m_stride(        rhs.m_stride )
    , m_length(        rhs.m_length )
    , m_count(         1 )
    { KOKKOS_MACRO_DEVICE_CAN_THROW( Impl::multivector_require_range( iBeg , iBeg+1 , rhs.m_count ) ); }

private:

  typedef device_type::memory_space                     memory_space ;
  typedef Impl::MemoryManager< memory_space >           memory_manager ;
  typedef Impl::MemoryView< value_type , memory_space > memory_view ;

  memory_view m_memory ;
  ValueType * m_ptr_on_device ;
  size_type   m_stride ;
  size_type   m_length ;
  size_type   m_count ;

  inline
  MultiVector( const std::string & label ,
               size_type arg_length , size_type arg_count )
    : m_memory()
    , m_ptr_on_device( 0 )
    , m_stride( arg_length )
    , m_length( arg_length )
    , m_count(  arg_count )
    {
      if ( 1 < arg_count ) {
        m_stride = memory_manager::preferred_alignment<value_type>( m_stride );
      }
      m_memory.allocate( m_count * m_stride , label );
      m_ptr_on_device = m_memory.ptr_on_device();

      Impl::Initialize< MultiVector >::run( *this );
    }

  template< typename V , class M >
  friend
  MultiVector< V , M >
  create_labeled_multivector( const std::string & label ,
                              size_t length , size_t count );

  template < typename V , class M > friend class MultiVector ;

  template < class DstType > friend class Impl::Initialize ;

  template < class DstType , class SrcType > friend class Impl::DeepCopy ;
};

} // namespace Kokkos

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif /* template specialization macros defined */

