
INCLUDE(TribitsAddExecutableAndTest)
INCLUDE(TribitsCopyFilesToBinaryDir)

SET(ARGS "--filedir=${CMAKE_CURRENT_BINARY_DIR}/")

IF (SITE STREQUAL "gabriel.sandia.gov")
  SET(ARGS
    "${ARGS} --not-unit-test=CrsMatrix_int_ComplexFloat_FullMatrixComplex_UnitTest")
ENDIF()

JOIN(allnodes   "|" FALSE ${Tpetra_ETI_NODES}  )
TRIBITS_ETI_TYPE_EXPANSION(conversions "SOUT=int" "SIN=double" "LO=int" "GO=int" "N=${allnodes}")
TRIBITS_ADD_ETI_INSTANTIATIONS(Tpetra ${conversions})

IF(Tpetra_ENABLE_Thrust)
  SET(MAXNP NUM_MPI_PROCS 1)
ENDIF()

#
# We break up CrsMatrix's unit tests into several files, to reduce
# compilation time for each file and improve build parallelism.
#

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_UnitTests
  SOURCES
    CrsMatrix_UnitTests
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ${ARGS}
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_UnitTests2
  SOURCES
    CrsMatrix_UnitTests2
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ${ARGS}
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_UnitTests3
  SOURCES
    CrsMatrix_UnitTests3
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ${ARGS}
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_UnitTests4
  SOURCES
    CrsMatrix_UnitTests4
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ${ARGS}
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_NonlocalAfterResume
  SOURCES
    CrsMatrix_NonlocalAfterResume
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ""
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_NodeConversion
  SOURCES
    CrsMatrix_NodeConversion
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ""
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_TriSolve
  SOURCES
    CrsMatrix_TriSolve
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ""
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_LeftRightScale
  SOURCES
    CrsMatrix_LeftRightScale
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ""
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_WithGraph
  SOURCES
    CrsMatrix_WithGraph
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ${ARGS}
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_ReplaceDomainMapAndImporter
  SOURCES
    CrsMatrix_ReplaceDomainMapAndImporter
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS ""
  ${MAXNP}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_BlockExtraction
  SOURCES
    CrsMatrix_BlockExtraction
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  # ARGS ${ARGS}
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_COPY_FILES_TO_BINARY_DIR(CrsMatrixCopyFiles1
  SOURCE_FILES west0067.rua mhd1280b.cua
  EXEDEPS CrsMatrix_UnitTests
  )

ASSERT_DEFINED(Anasazi_SOURCE_DIR)
TRIBITS_COPY_FILES_TO_BINARY_DIR(CrsTests_CopyFiles2
  SOURCE_DIR ${Anasazi_SOURCE_DIR}/testmatrices
  SOURCE_FILES bcsstk14.hb bcsstk15.hb
  )

# This test only makes sense for > 1 processes.
TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_NonlocalSumInto
  SOURCES
    CrsMatrix_NonlocalSumInto
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS
  COMM mpi
  NUM_MPI_PROCS 2-4
  STANDARD_PASS_OUTPUT
  )

# This test only makes sense for > 1 processes.
TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_NonlocalSumInto_Ignore
  SOURCES
    CrsMatrix_NonlocalSumInto_Ignore
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS
  COMM mpi
  NUM_MPI_PROCS 2-4
  STANDARD_PASS_OUTPUT
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_gaussSeidel
  SOURCES
    CrsMatrix_gaussSeidel
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS
  COMM serial mpi
  NUM_MPI_PROCS 1-4
  STANDARD_PASS_OUTPUT
  )

# This test only makes sense for exactly 2 MPI processes.
TRIBITS_ADD_EXECUTABLE_AND_TEST(
  CrsMatrix_Bug5978
  SOURCES
    bug5978
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  ARGS
  COMM mpi
  NUM_MPI_PROCS 2
  STANDARD_PASS_OUTPUT
  )

SET(TIMING_INSTALLS "")

#IF (KokkosClassic_ENABLE_TBB AND Tpetra_ENABLE_MPI)
#  TRIBITS_ADD_EXECUTABLE(
#      CRSTiming_TBB
#      SOURCES CRSTiming_TBB
#      COMM mpi
#      DEPLIBS kokkos kokkoslinalg kokkosnodeapi
#      )
#  APPEND_SET(TIMING_INSTALLS Tpetra_CRSTiming_TBB)
#ENDIF()

#IF (KokkosClassic_ENABLE_ThreadPool AND Tpetra_ENABLE_MPI)
#  TRIBITS_ADD_EXECUTABLE(
#      CRSTiming_TPI
#      SOURCES CRSTiming_TPI
#      COMM mpi
#      DEPLIBS kokkos kokkoslinalg kokkosnodeapi
#      )
#  APPEND_SET(TIMING_INSTALLS Tpetra_CRSTiming_TPI)
#ENDIF()

INSTALL(TARGETS ${TIMING_INSTALLS}
        RUNTIME DESTINATION "${${PROJECT_NAME}_INSTALL_RUNTIME_DIR}")
