#ifndef MUELU_HIERARCHYFACTORY_HPP
#define MUELU_HIERARCHYFACTORY_HPP

#include "Teuchos_RCP.hpp"

#include "MueLu_ConfigDefs.hpp"
#include "MueLu_BaseClass.hpp"

#include "MueLu_Hierarchy_fwd.hpp"

namespace MueLu {

  //!
  template <class Scalar = double, class LocalOrdinal = int, class GlobalOrdinal = LocalOrdinal, class Node = Kokkos::DefaultNode::DefaultNodeType, class LocalMatOps = typename Kokkos::DefaultKernels<void, LocalOrdinal, Node>::SparseOps>
  class HierarchyFactory : public BaseClass {
#undef MUELU_HIERARCHYFACTORY_SHORT
#include "MueLu_UseShortNames.hpp"

  public:
    //@{ Constructors/Destructors.

    //! Destructor.
    virtual ~HierarchyFactory() { }

    //@}

    //! Create an empty Hierarchy object
    // Note: This function is not very useful at the moment as MueLu only have on Hierarchy class.
    //       In the future, we might have an abstract Hierarchy class and several derived Hierarchy classes.
    //       Using this function will then be the recommended way to generate a Hierarchy.
    //
    // This method is called Create() instead of Build(), because it return an non-initialized 
    // object (ie: MG setup is not done). 
    // Build() function in MueLu returns initialized objects.
    virtual RCP<Hierarchy> CreateHierarchy() const = 0;
    
    //! Setup Hierarchy object
    virtual void SetupHierarchy(Hierarchy & H) const = 0;

  }; // class HierarchyFactoryBase

} // namespace MueLu

#define MUELU_HIERARCHYFACTORY_SHORT
#endif //ifndef MUELU_HIERARCHYFACTORY_HPP
