/*------------------------------------------------------------------------*/
/*                 Copyright 2010 Sandia Corporation.                     */
/*  Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive   */
/*  license for use of this work by or on behalf of the U.S. Government.  */
/*  Export of this program may require a license from the                 */
/*  United States Government.                                             */
/*------------------------------------------------------------------------*/

#ifndef STK_MESH_UNITTEST_GRID_MESH_WITH_SHELL_FIXTURE_HPP
#define STK_MESH_UNITTEST_GRID_MESH_WITH_SHELL_FIXTURE_HPP

#include <stk_util/parallel/Parallel.hpp>
#include <stk_mesh/base/Types.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/BulkData.hpp>

class GridMeshWithShellFixture
{
private:

  stk::mesh::MetaData m_meta_data;
  stk::mesh::BulkData m_bulk_data;
  stk::mesh::Part   & m_quad_part;
  stk::mesh::Part   & m_shell_part;

public:
  // intentionally exposed to the public
  std::vector<stk::mesh::EntityId> m_node_ids;
  std::vector<stk::mesh::EntityId> m_quad_face_ids;
  std::vector<stk::mesh::EntityId> m_shell_face_ids;

  GridMeshWithShellFixture(stk::ParallelMachine pm);

  ~GridMeshWithShellFixture();

  stk::mesh::MetaData& meta_data() { return m_meta_data; }
  stk::mesh::BulkData& bulk_data() { return m_bulk_data; }

  void generate_grid();
};

#endif

