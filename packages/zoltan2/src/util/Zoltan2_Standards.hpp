// @HEADER
// ***********************************************************************
//
//                Copyright message goes here.   TODO
//
// ***********************************************************************
// @HEADER

/*! \file Zoltan2_Standards.hpp
    \brief Gathering definitions used in software development.

     \todo Should we allow data types for part ID to be set as
         cmake configure options?  Part ID lists in the PartitioningSolution
         are of length "number of objects".  If part ID could be short
         or int, we save significant memory.  For now - typedef'd to int
         so it is easy to change.  It seems data type for proc should
         be int - since it is int in the rest of Trilinos.
*/

#ifndef _ZOLTAN2_STANDARDS_HPP_
#define _ZOLTAN2_STANDARDS_HPP_

#include <Zoltan2_Version.hpp>

//////////////////////////////////////////
// Generated by CMake
#include <Zoltan2_config.h>

//////////////////////////////////////////
// Frequently used Trilinos symbols

#define global_size_t size_t

#include <Teuchos_RCP.hpp>
#include <Teuchos_Array.hpp>
#include <Teuchos_Tuple.hpp>
#include <Teuchos_ArrayRCP.hpp>
#include <Teuchos_ArrayView.hpp>
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_ParameterEntry.hpp>
#include <Teuchos_DefaultComm.hpp>
#include <Teuchos_Comm.hpp>
#include <Teuchos_CommHelpers.hpp>

namespace Zoltan2{
using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcp_const_cast;
using Teuchos::rcp_implicit_cast;
using Teuchos::rcp_dynamic_cast;
using Teuchos::Array;
using Teuchos::Tuple;
using Teuchos::ArrayRCP;
using Teuchos::arcp_const_cast;
using Teuchos::arcp_reinterpret_cast;
using Teuchos::arcp;
using Teuchos::ArrayView;
using Teuchos::av_const_cast;
#ifdef HAVE_ZOLTAN2_MPI
using Teuchos::MpiComm;
#endif
using Teuchos::Comm;
using Teuchos::SerialComm;
using Teuchos::DefaultComm;
using Teuchos::CommRequest;
using Teuchos::ParameterList;
using Teuchos::ParameterEntry;
using Teuchos::reduceAll;
using Teuchos::gatherAll;
}

//////////////////////////////////////////////////////
// Our data types
//   Prepend API types with zoltan2_.
//////////////////////////////////////////////////////

typedef int zoltan2_partId_t;

namespace Zoltan2{
  typedef zoltan2_partId_t partId_t;
}

//////////////////////////////////////////////////////
// For debugging
//////////////////////////////////////////////////////

#define HELLO

//////////////////////////////////////////////////////
// Internal macros and methods
//////////////////////////////////////////////////////

#include <Zoltan2_Exceptions.hpp>

//////////////////////////////////////////////////////
// Until Kokkos node types are supported, use default
//////////////////////////////////////////////////////

#include <Kokkos_DefaultNode.hpp>

namespace Zoltan2{
typedef Kokkos::DefaultNode::DefaultNodeType default_node_t;
}

#endif
