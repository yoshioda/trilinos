#
# CI testing for CASL VRI add-on Trilnos packages with GCC 4.5.1 compiler
#

INCLUDE("${CTEST_SCRIPT_DIRECTORY}/hybrid-build-11.cmake")
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/TrilinosCTestDriverCore.pu241.gcc.4.5.1.cmake")
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/SubmitToCaslDev.cmake")
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/casl-vri-packages-coupled.cmake")

SET(COMM_TYPE MPI)
SET(BUILD_TYPE DEBUG)
SET(BUILD_DIR_NAME MPI_DEBUG_GCC_CI_CASLDEV)
SET(CTEST_TEST_TYPE Continuous)
#SET(CTEST_TEST_TIMEOUT 900)
SET(Trilinos_ENABLE_SECONDARY_STABLE_CODE ON)
SET(EXTRA_CONFIGURE_OPTIONS
  ${EXTRA_CONFIGURE_OPTIONS}
  "-DTrilinos_CONFIGURE_OPTIONS_FILE:FILEPATH=${CTEST_SCRIPT_DIRECTORY}/gcc-4.5.1-mpi-debug-ss-options.cmake"
  "-DSTK_ENABLE_BoostLib:BOOL=OFF"
  "-DTrilinos_TEST_CATEGORIES:STRING=CONTINUOUS"
  )

TRILINOS_SYSTEM_SPECIFIC_CTEST_DRIVER()
