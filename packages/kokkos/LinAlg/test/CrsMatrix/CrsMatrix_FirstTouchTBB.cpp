// ************************************************************************
// 
//          Kokkos: A Fast Kernel Package
//              Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
//@HEADER

#include <Teuchos_UnitTestHarness.hpp>
#include <Teuchos_TimeMonitor.hpp>
#include <Teuchos_Time.hpp>
#include <Teuchos_TypeNameTraits.hpp>
#include <Teuchos_ScalarTraits.hpp>

#include "Kokkos_ConfigDefs.hpp"
#include "Kokkos_DefaultArithmetic.hpp"
#include "Kokkos_DefaultKernels.hpp"
#include "Kokkos_CrsGraph.hpp"
#include "Kokkos_CrsMatrix.hpp"
#include "Kokkos_Version.hpp"

#include "Kokkos_TBBNode.hpp"

namespace {

  using Kokkos::MultiVector;
  using Kokkos::CrsMatrixHostCompute;
  using Kokkos::CrsGraphHostCompute;
  using Kokkos::FirstTouchHostCrsMatrix;
  using Kokkos::FirstTouchHostCrsGraph;
  using Kokkos::DefaultArithmetic;
  using Kokkos::SerialNode;
  using Kokkos::DefaultKernels;
  using Teuchos::ArrayRCP;
  using Teuchos::RCP;
  using Teuchos::rcp;
  using Teuchos::null;
  using std::endl;

#include "CrsMatrix_CrsTimingTests.hpp"

  using Kokkos::TBBNode;
  RCP<TBBNode> tbbnode;

  template <>
  RCP<TBBNode> getNode<TBBNode>() {
    if (tbbnode == null) {
      Teuchos::ParameterList pl;
      pl.set<int>("Num Threads",Test::numThreads);
      tbbnode = rcp(new TBBNode(pl));
    }
    return tbbnode;
  }

  //
  // Test
  // 

  typedef TBBNode                                             Node;
  // mfh 23 Feb 2011:
  // You don't need "typename" here, since none of the three 
  // parameters of DefaultKernels are template parameters.
  //typedef typename DefaultKernels<void,int,Node>::SparseOps   DSM;
  typedef DefaultKernels<void,int,Node>::SparseOps            DSM;
  typedef CrsMatrixHostCompute<double,int,Node,DSM>           StandardMat;
  typedef CrsGraphHostCompute<int,Node,DSM>                   StandardGraph;
  typedef FirstTouchHostCrsMatrix<double,int,Node,DSM>        FirstTouchMat;
  typedef FirstTouchHostCrsGraph<int,Node,DSM>                FirstTouchGraph;

  TEUCHOS_UNIT_TEST_TEMPLATE_2_INSTANT( CrsTiming, PowerTriDiag, StandardGraph,   StandardMat   )
  TEUCHOS_UNIT_TEST_TEMPLATE_2_INSTANT( CrsTiming, PowerTriDiag, FirstTouchGraph, FirstTouchMat )

}

