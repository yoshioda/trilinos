//----------------------------------------------------------------------

#include <algorithm>
#include <stk_mesh/base/Types.hpp>
#include <stk_mesh/base/GetBuckets.hpp>
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Bucket.hpp>
#include <stk_mesh/base/Part.hpp>

//----------------------------------------------------------------------

namespace stk {
namespace mesh {

//----------------------------------------------------------------------

void get_buckets( const Selector & selector ,
                  const std::vector< Bucket * > & input ,
                        std::vector< Bucket * > & output )
{
  for ( std::vector< Bucket * >::const_iterator
        i = input.begin() ; i != input.end() ; ++i ) {
    Bucket * const b = *i ;
    if ( selector( *b ) ) { output.push_back( b ); }
  }
}


void copy_ids( std::vector<unsigned> & v , const PartVector & p )
{
  {
    const size_t n = p.size();
    v.resize( n );
    for ( size_t k = 0 ; k < n ; ++k ) {
      v[k] = p[k]->mesh_meta_data_ordinal();
    }
  }

  {
    std::vector<unsigned>::iterator i = v.begin() , j = v.end();
    std::sort( i , j );
    i = std::unique( i , j );
    v.erase( i , j );
  }
}

void get_involved_parts( 
    const PartVector & union_parts,
    const Bucket & candidate,
    PartVector & involved_parts
    )
{
  involved_parts.clear();
  if (union_parts.size() == 0) {
    return;
  }

  // Used to convert part ordinals to part pointers:
  const PartVector & all_parts = union_parts[0]->mesh_meta_data().get_parts(); 

  const std::pair<const unsigned *,const unsigned *>
    bucket_part_begin_end_iterators = candidate.superset_part_ordinals(); // sorted and unique

  std::vector<unsigned> union_parts_ids;
  copy_ids( union_parts_ids , union_parts ); // sorted and unique
  std::vector<unsigned>::const_iterator union_part_id_it = union_parts_ids.begin();
  const unsigned * bucket_part_id_it = bucket_part_begin_end_iterators.first ;

  while ( union_part_id_it != union_parts_ids.end() && 
          bucket_part_id_it != bucket_part_begin_end_iterators.second ) 
  {
    if      ( *union_part_id_it  < *bucket_part_id_it ) { 
      ++union_part_id_it ; 
    }
    else if ( *bucket_part_id_it < *union_part_id_it )  { 
      ++bucket_part_id_it ; 
    }
    else {
      // Find every match:
      Part * const part = all_parts[ *union_part_id_it ];
      involved_parts.push_back( part );
      ++union_part_id_it; 
      ++bucket_part_id_it;
    }
  }

}

//----------------------------------------------------------------------

} // namespace mesh
} // namespace stk


