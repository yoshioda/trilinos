#include "Teuchos_UnitTestHarness.hpp"
#include "MueLu_ConfigDefs.hpp"
#include "MueLu_Version.hpp"

#include "MueLu_TestHelpers.hpp"

#include "MueLu_Level.hpp"

#include "MueLu_UseDefaultTypes.hpp"
#include "MueLu_UseShortNames.hpp"

namespace MueLuTests {

  TEUCHOS_UNIT_TEST(Level, SetCoreData)
  {
    out << "version: " << MueLu::Version() << std::endl;

    Level aLevel;
    MueLu::TestHelpers::Factory<SC, LO, GO, NO, LMO>::Factory<SC, LO, GO, NO, LMO>::createSingleLevelHierarchy(aLevel);

    RCP<Operator> A = MueLu::TestHelpers::Factory<SC, LO, GO, NO, LMO>::Build1DPoisson(2); //can be an empty operator

    aLevel.Set("A",A);
    RCP<Operator> newA = aLevel.Get< RCP<Operator> >("A");
    TEUCHOS_TEST_EQUALITY(newA, A, out, success);
    aLevel.Set("R", A);
    TEUCHOS_TEST_EQUALITY(aLevel.Get< RCP<Operator> >("R"), A, out, success); //TODO from JG: must be tested using another matrix !
    aLevel.Set("P", A);
    TEUCHOS_TEST_EQUALITY(aLevel.Get< RCP<Operator> >("P"), A, out, success);
    aLevel.SetLevelID(42);
    TEUCHOS_TEST_EQUALITY(aLevel.GetLevelID(), 42, out, success); //TODO: test default value of LevelID

    /*
      RCP<Smoother> preSmoo = Smoother<Scalar, LO, GO, Node, LMO>();
      TEUCHOS_TEST_EQUALITY(aLevel.Get< RCP<SmootherPrototype> >("PreSmoother"), preSmoo, out, success);
      //RCP<Smoother> postSmoo = Smoother<Scalar, LO, GO, Map, Operator>();
      */


    //out << aLevel << std::endl;
    /*
      out << "Testing copy ctor" << std::endl;
      Level secondLevel(aLevel);
      //out << secondLevel << std::endl;
      */
  }

}//namespace MueLuTests

