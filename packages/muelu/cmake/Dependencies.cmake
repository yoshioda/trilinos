SET(LIB_REQUIRED_DEP_PACKAGES Teuchos Kokkos) #TODO: remove Kokkos
SET(LIB_OPTIONAL_DEP_PACKAGES Epetra Tpetra Ifpack Ifpack2 EpetraExt)
SET(TEST_REQUIRED_DEP_PACKAGES)
SET(TEST_OPTIONAL_DEP_PACKAGES Epetra EpetraExt Tpetra Kokkos) #TODO: clean up this line
SET(LIB_REQUIRED_DEP_TPLS)
SET(LIB_OPTIONAL_DEP_TPLS)
SET(TEST_REQUIRED_DEP_TPLS)
SET(TEST_OPTIONAL_DEP_TPLS)

#TODO: why there is no EXAMPLE_*_DEP_* ?
