/*------------------------------------------------------------------------*/
/*      phdMesh : Parallel Heterogneous Dynamic unstructured Mesh         */
/*                Copyright (2007) Sandia Corporation                     */
/*                                                                        */
/*  Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive   */
/*  license for use of this work by or on behalf of the U.S. Government.  */
/*                                                                        */
/*  This library is free software; you can redistribute it and/or modify  */
/*  it under the terms of the GNU Lesser General Public License as        */
/*  published by the Free Software Foundation; either version 2.1 of the  */
/*  License, or (at your option) any later version.                       */
/*                                                                        */
/*  This library is distributed in the hope that it will be useful,       */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     */
/*  Lesser General Public License for more details.                       */
/*                                                                        */
/*  You should have received a copy of the GNU Lesser General Public      */
/*  License along with this library; if not, write to the Free Software   */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307   */
/*  USA                                                                   */
/*------------------------------------------------------------------------*/
/**
 * @author H. Carter Edwards
 */

#include <stddef.h>
#include <math.h>
#include <util/taskpool.h>
#include <txblas/reduction.h>

/*--------------------------------------------------------------------*/


#define SUM_ADD( v , a ) \
  { \
    const double VpA = v[0] + a ; \
    v[1] += a < v[0] ? ( a - ( VpA - v[0] ) ) : ( v[0] - ( VpA - a ) ); \
    v[0]  = VpA + v[1] ; \
    v[1] += VpA - v[0] ; \
  }

#define SUM_SUB( v , a ) \
  { \
    const double VmA = v[0] - a ; \
    v[1] += a < fabs(v[0]) ? ( -a - ( VmA - v[0] ) ) : ( v[0] - ( VmA + a ) ); \
    v[0]  = VmA + v[1] ; \
    v[1] += VmA - v[0] ; \
  }

/*--------------------------------------------------------------------*/

void xdsum_add_value( double * v , double a )
{
  if ( a < 0 ) { a = -a ; v += 2 ; }
  SUM_ADD( v , a );
}

void xdsum_add_dsum( double * v , const double * const a )
{
  SUM_ADD( v , a[0] );
  SUM_ADD( v , a[1] );
  v += 2 ;
  SUM_ADD( v , a[2] );
  SUM_ADD( v , a[3] );
}

void xdsum_get_value( double * const y , const double * const v )
{
  y[0] = v[0] ;
  y[1] = v[1] ;
  SUM_SUB( y , v[2] );
  SUM_SUB( y , v[3] );
}

/*--------------------------------------------------------------------*/

struct TaskX {
        double * x_sum ;
  const double * x_beg ;
        unsigned number ;
};

struct TaskXY {
        double * xy_sum ;
  const double * x_beg ;
  const double * y_beg ;
        unsigned number ;
};

/*--------------------------------------------------------------------*/

static
void add_array( double * const s , const double * x , const double * const xe )
{
  while ( xe != x ) {
    double   a = *x ; ++x ;
    double * p = s ;
    if ( a < 0 ) { a = -a ; p += 2 ; }
    SUM_ADD( p , a );
  }
}

void xdsum_add_array( double * s , unsigned n , const double * x )
{ add_array( s , x , x + n ); }

static int task_sum_work( void * arg , unsigned p_size , unsigned p_rank )
{
  struct TaskX * const t  = (struct TaskX *) arg ;

  const unsigned p_next = p_rank + 1 ;
  const unsigned n = t->number ;
  const double * const xb = t->x_beg + ( n * p_rank ) / p_size ;
  const double * const xe = t->x_beg + ( n * p_next ) / p_size ;
        double * const v  = t->x_sum ;

  double partial[4] = { 0 , 0 , 0 , 0 };

  add_array( partial , xb , xe );

  phdmesh_taskpool_lock(0,NULL);
  xdsum_add_dsum( v , partial );
  phdmesh_taskpool_unlock(0,NULL);

  return 0 ;
}

void txdsum_add_array( double * s , unsigned n , const double * x )
{
  struct TaskX data ;
  data.x_sum  = s ;
  data.x_beg  = x ;
  data.number = n ;
  phdmesh_taskpool_run( & task_sum_work , & data , 1 );
}

/*--------------------------------------------------------------------*/

static void norm1( double * s , const double * x , const double * const xe )
{
  while ( xe != x ) { const double a = fabs(*x); ++x ; SUM_ADD( s , a ); }
}

void xdnorm1( double * s2 , unsigned n , const double * x )
{ norm1( s2 , x , x + n ); }

static int task_norm1_work( void * arg , unsigned p_size , unsigned p_rank )
{
  struct TaskX * const t  = (struct TaskX *) arg ;

  const unsigned p_next = p_rank + 1 ;
  const unsigned n = t->number ;
  const double * const xb = t->x_beg + ( n * p_rank ) / p_size ;
  const double * const xe = t->x_beg + ( n * p_next ) / p_size ;
        double * const v  = t->x_sum ;

  double partial[2] = { 0 , 0 };

  norm1( partial , xb , xe );

  phdmesh_taskpool_lock(0,NULL);
  SUM_ADD( v , partial[0] );
  SUM_ADD( v , partial[1] );
  phdmesh_taskpool_unlock(0,NULL);

  return 0 ;
}

void txdnorm1( double * s , unsigned n , const double * x )
{
  struct TaskX data ;
  data.x_sum  = s ;
  data.x_beg  = x ;
  data.number = n ;
  phdmesh_taskpool_run( & task_norm1_work , & data , 1 );
}

/*--------------------------------------------------------------------*/

static void dot1_unroll( double * s , const double * x , const size_t n )
{
  enum { NB = 8 };

  for ( const double * const x_blk = x + n % NB ; x_blk != x ; ++x ) {
    double a = *x ; a *= a ; SUM_ADD( s , a );
  }

  for ( const double * const x_end = x + n ; x_end != x ; x += NB ) {
    double a0 = x[0] ;
    double a1 = x[1] ;
    double a2 = x[2] ;
    double a3 = x[3] ;
    double a4 = x[4] ;
    double a5 = x[5] ;
    double a6 = x[6] ;
    double a7 = x[7] ;
    a0 *= a0 ;
    a1 *= a1 ;
    a2 *= a2 ;
    a3 *= a3 ;
    a4 *= a4 ;
    a5 *= a5 ;
    a6 *= a6 ;
    a7 *= a7 ;
    SUM_ADD( s , a0 );
    SUM_ADD( s , a1 );
    SUM_ADD( s , a2 );
    SUM_ADD( s , a3 );
    SUM_ADD( s , a4 );
    SUM_ADD( s , a5 );
    SUM_ADD( s , a6 );
    SUM_ADD( s , a7 );
  }
}

void xddot1( double * s2 , unsigned n , const double * x )
{ dot1_unroll( s2 , x , n ); }

static int task_dot_x_work( void * arg , unsigned p_size , unsigned p_rank )
{
  double partial[2] = { 0 , 0 };
  struct TaskX * const t  = (struct TaskX *) arg ;

  {
    const unsigned p_next   = p_rank + 1 ;
    const unsigned n_global = t->number ;
    const unsigned n_begin  = ( ( n_global * p_rank ) / p_size );
    const unsigned n_local  = ( ( n_global * p_next ) / p_size ) - n_begin ;

    dot1_unroll( partial , t->x_beg + n_begin , n_local );
  }

  {
    phdmesh_taskpool_lock(0,NULL);
    double * const v = t->x_sum ;
    SUM_ADD( v , partial[0] );
    SUM_ADD( v , partial[1] );
    phdmesh_taskpool_unlock(0,NULL);
  }

  return 0 ;
}

void txddot1( double * s , unsigned n , const double * x )
{
  struct TaskX data ;
  data.x_sum  = s ;
  data.x_beg  = x ;
  data.number = n ;
  phdmesh_taskpool_run( & task_dot_x_work , & data , 1 );
}

/*--------------------------------------------------------------------*/

static
void dot_unroll( double * const s ,
                 const double * x ,
                 const double * y ,
                 const size_t n )
{
  enum { NB = 8 };

  for ( const double * const y_blk = y + n % NB ; y_blk != y ; ++x , ++y ) {
    double a = *x * *y ;
    double * const p = a < 0 ? ( a = -a , s + 2 ) : s ;
    SUM_ADD( p , a );
  }

  for ( const double * const y_end = y + n ; y_end != y ; x += NB , y += NB ) {
    double a0 = x[0] * y[0] ;
    double a1 = x[1] * y[1] ;
    double a2 = x[2] * y[2] ;
    double a3 = x[3] * y[3] ;
    double a4 = x[4] * y[4] ;
    double a5 = x[5] * y[5] ;
    double a6 = x[6] * y[6] ;
    double a7 = x[7] * y[7] ;
    double * const p0 = a0 < 0 ? ( a0 = -a0 , s + 2 ) : s ;
    double * const p1 = a1 < 0 ? ( a1 = -a1 , s + 2 ) : s ;
    double * const p2 = a2 < 0 ? ( a2 = -a2 , s + 2 ) : s ;
    double * const p3 = a3 < 0 ? ( a3 = -a3 , s + 2 ) : s ;
    double * const p4 = a4 < 0 ? ( a4 = -a4 , s + 2 ) : s ;
    double * const p5 = a5 < 0 ? ( a5 = -a5 , s + 2 ) : s ;
    double * const p6 = a6 < 0 ? ( a6 = -a6 , s + 2 ) : s ;
    double * const p7 = a7 < 0 ? ( a7 = -a7 , s + 2 ) : s ;
    SUM_ADD( p0 , a0 );
    SUM_ADD( p1 , a1 );
    SUM_ADD( p2 , a2 );
    SUM_ADD( p3 , a3 );
    SUM_ADD( p4 , a4 );
    SUM_ADD( p5 , a5 );
    SUM_ADD( p6 , a6 );
    SUM_ADD( p7 , a7 );
  }
}

void xddot( double * s4 , unsigned n , const double * x , const double * y )
{ dot_unroll( s4 , x , y , n ); }

static int task_dot_xy_work( void * arg , unsigned p_size , unsigned p_rank )
{
  struct TaskXY * const t  = (struct TaskXY *) arg ;

  double partial[4] = { 0 , 0 , 0 , 0 };

  {
    const unsigned p_next = p_rank + 1 ;
    const unsigned n_global = t->number ;
    const unsigned n_begin = ( n_global * p_rank ) / p_size ;
    const unsigned n_local = ( n_global * p_next ) / p_size - n_begin ;

    const double * const xb = t->x_beg + n_begin ;
    const double * const yb = t->y_beg + n_begin ;

    dot_unroll( partial , xb , yb , n_local );
  }

  phdmesh_taskpool_lock(0,NULL);
  {
    double * const s = t->xy_sum ;
    xdsum_add_dsum( s , partial );
  }
  phdmesh_taskpool_unlock(0,NULL);

  return 0 ;
}

void txddot( double * s , unsigned n , const double * x , const double * y )
{
  struct TaskXY data ;
  data.xy_sum = s ;
  data.x_beg  = x ;
  data.x_beg  = y ;
  data.number = n ;
  phdmesh_taskpool_run( & task_dot_xy_work , & data , 1 );
}

/*--------------------------------------------------------------------*/

