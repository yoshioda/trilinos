// @HEADER
// ***********************************************************************
// 
//          Tpetra: Templated Linear Algebra Services Package
//                 Copyright (2008) Sandia Corporation
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#ifndef TPETRA_MPIPLATFORM_HPP
#define TPETRA_MPIPLATFORM_HPP

#include <Teuchos_DefaultMpiComm.hpp>
#include <Teuchos_Describable.hpp>
#include <Kokkos_DefaultNode.hpp>

namespace Tpetra {

  /// \brief Implementation of the Platform concept for MPI-based platforms.
  ///
  /// MpiPlatform is an implementation of Tpetra's Platform concept.
  /// Classes implementing Tpetra's Platform concept are templated on
  /// the Kokkos Node type.  They have at least the following public
  /// interface:
  /// \code
  /// template<class Node>
  /// class Platform {
  /// public:
  ///   typedef Node NodeType;
  ///
  ///   explicit Platform (const RCP<Node>& node);
  ///
  ///   RCP<const Comm<int> > getComm() const;
  ///
  ///   RCP<Node> getNode() const;
  /// };
  /// \endcode
  /// MpiPlatform also has a constructor that accepts an MPI
  /// communicator, over which the application using the platform
  /// should perform communication.  The default communicator is
  /// MPI_COMM_WORLD.  MpiPlatform is only available if Trilinos was
  /// built with MPI.
  ///
  template <class Node>
  class MpiPlatform : public Teuchos::Describable {
  public:
    /// \typedef NodeType 
    /// \brief Kokkos Node type over which the platform is templated.
    typedef Node NodeType;

    //! @name Constructor/Destructor Methods
    //@{ 

    /// \brief Constructor that accepts a Kokkos Node.
    ///
    /// This version of the constructor uses MPI_COMM_WORLD as the
    /// communicator.  It is declared "explicit" to forbid silent type
    /// conversions from the Node instance to an MpiPlatform.  (A
    /// single-argument constructor that is not declared "explicit"
    /// defines a type conversion method from the input type to the
    /// constructor's class's type.)  The "explicit" declaration does
    /// not affect typical use of this constructor.
    ///
    /// \param node [in/out] The Kokkos Node instance.
    ///
    explicit MpiPlatform(const RCP<Node> &node) :
      node_ (node),
      comm_ (Teuchos::createMpiComm<int> (Teuchos::opaqueWrapper<MPI_Comm> (MPI_COMM_WORLD)))
    {}

    /// \brief Constructor that accepts a Kokkos Node and MPI communicator.
    ///
    /// This version of constructor accepts an arbitrary MPI
    /// communicator.  You first have to wrap the MPI communicator in
    /// a Teuchos::OpaqueWrapper, for example as follows:
    /// \code
    /// MPI_Comm myComm;
    /// // ... set myComm to some MPI communicator ...
    /// MpiPlatform<NodeType> platform (node, Teuchos::opaqueWrapper (myComm));
    /// \endcode
    /// (The lower-case o is correct in \c Teuchos::opaqueWrapper.
    /// That is a templated nonmember constructor that returns a
    /// Teuchos::RCP<Teuchos::OpaqueWrapper<MPI_Comm> in this case.)
    ///
    /// The reason you need to wrap the MPI communicator in a \c
    /// Teuchos::OpaqueWrapper is because that tells RCP to handle
    /// destruction in a special way.  MPI_Comm isn't really an
    /// object, it's a handle to an object.  RCP needs to know this so
    /// that it can do the right thing when the reference count goes
    /// to zero.
    ///
    /// \param node [in/out] The Kokkos Node instance.
    /// \param rawMpiComm [in] The MPI communicator, wrapped in a \c
    ///   Teuchos::OpaqueWrapper.
    ///
    MpiPlatform (const RCP<Node> &node, 
		 const RCP<const Teuchos::OpaqueWrapper<MPI_Comm> > &rawMpiComm)
      : node_ (node),
	comm_ (Teuchos::createMpiComm<int> (rawMpiComm))
    {}

    //! Destructor (virtual for memory safety of derived classes).
    virtual ~MpiPlatform() {}

    //@}

    //! @name Class Creation and Accessor Methods
    //@{ 

    //! The \c Teuchos::Comm instance with which this object was created.
    RCP<const Comm<int> > getComm() const {
      return comm_; 
    }

    //! The Kokkos Node instance with which this object was created.
    RCP<Node> getNode() const {
      return node_;
    }

    //@}

  protected: 
    //! Teuchos::Comm object instantiated for the platform.
    RCP<Teuchos::MpiComm<int> > comm_;

    //! Kokkos Node object instantiated for the platform.
    RCP<Node> node_;

  private:
    //! Unimplemented copy constructor (syntactically forbidden).
    MpiPlatform (const MpiPlatform<Node> &platform);

    //! Unimplemented assignment operator (syntactically forbidden).
    MpiPlatform& operator= (const MpiPlatform<Node> &platform);
  };


  /// \class MpiPlatform<Kokkos::DefaultNode::DefaultNodeType>
  /// \brief MpiPlatform specialization for \c Kokkos::DefaultNode::DefaultNodeType.
  ///
  /// \warning \c Kokkos::DefaultNode::DefaultNodeType is a typedef,
  ///   and may have a different type, depending on Trilinos' build
  ///   options.  For example, it may be Kokkos::SerialNode if
  ///   Trilinos was built without a threading library, or
  ///   Kokkos::TPINode if Trilinos was built with Pthreads.
  ///
  /// \note In the past (up to and including the 10.8 Trilinos
  ///   release), the specialization of MpiPlatform for the default
  ///   Node type delayed instantiation of the default Node instance
  ///   until getNode() was called.  We have changed this behavior to
  ///   simplify the code and make the specialization of MpiPlatform
  ///   conform more closely to the generic version of MpiPlatform.
  template <>
  class MpiPlatform<Kokkos::DefaultNode::DefaultNodeType> : 
    public Teuchos::Describable {
  public:
    /// \typedef NodeType 
    /// \brief Kokkos Node type over which the platform is templated.
    typedef Kokkos::DefaultNode::DefaultNodeType NodeType;

    //! @name Constructor/Destructor Methods
    //@{ 

    /// \brief Default constructor: uses Kokkos default node and MPI_COMM_WORLD.
    ///
    /// The specialization of MpiPlatform for the default node type
    /// includes a default constructor.  It instantiates a default
    /// Node instance using Kokkos::DefaultNode::getDefaultNode(), and
    /// uses MPI_COMM_WORLD as the communicator.
    ///
    MpiPlatform () :
      node_ (Kokkos::DefaultNode::getDefaultNode ()),
      comm_ (Teuchos::createMpiComm<int> (Teuchos::opaqueWrapper<MPI_Comm> (MPI_COMM_WORLD)))
    {}

    /// \brief Constructor that accepts a Kokkos Node.
    ///
    /// This version of the constructor uses MPI_COMM_WORLD as the
    /// communicator.  It is declared "explicit" to forbid silent type
    /// conversions from the Node instance to an MpiPlatform.  (A
    /// single-argument constructor that is not declared "explicit"
    /// defines a type conversion method from the input type to the
    /// constructor's class's type.)  The "explicit" declaration does
    /// not affect typical use of this constructor.
    ///
    /// This specialization of MpiPlatform for the default node type
    /// will instantiate a default Node if node.is_null().
    ///
    /// \param node [in/out] The Kokkos Node instance.
    ///
    explicit MpiPlatform (const RCP<Node> &node) :
      node_ (node.is_null() ? Kokkos::DefaultNode::getDefaultNode() : node),
      comm_ (Teuchos::createMpiComm<int> (Teuchos::opaqueWrapper<MPI_Comm> (MPI_COMM_WORLD)))
    {}

    /// \brief Constructor that accepts a Kokkos Node and MPI communicator.
    ///
    /// This version of constructor accepts an arbitrary MPI
    /// communicator.  You first have to wrap the MPI communicator in
    /// a Teuchos::OpaqueWrapper, for example as follows:
    /// \code
    /// MPI_Comm myComm;
    /// // ... set myComm to some MPI communicator ...
    /// MpiPlatform<NodeType> platform (node, Teuchos::opaqueWrapper (myComm));
    /// \endcode
    /// (The lower-case o is correct in \c Teuchos::opaqueWrapper.
    /// That is a templated nonmember constructor that returns a
    /// Teuchos::RCP<Teuchos::OpaqueWrapper<MPI_Comm> in this case.)
    ///
    /// The reason you need to wrap the MPI communicator in a \c
    /// Teuchos::OpaqueWrapper is because that tells RCP to handle
    /// destruction in a special way.  MPI_Comm isn't really an
    /// object, it's a handle to an object.  RCP needs to know this so
    /// that it can do the right thing when the reference count goes
    /// to zero.
    ///
    /// This specialization of MpiPlatform for the default node type
    /// will instantiate a default Node if node.is_null().
    ///
    /// \param node [in/out] The Kokkos Node instance.
    /// \param rawMpiComm [in] The MPI communicator, wrapped in a \c
    ///   Teuchos::OpaqueWrapper.
    ///
    MpiPlatform (const RCP<Kokkos::DefaultNode::DefaultNodeType> &node, 
		 const RCP<const Teuchos::OpaqueWrapper<MPI_Comm> > &rawMpiComm) 
      : node_ (node.is_null() ? Kokkos::DefaultNode::getDefaultNode() : node),
	comm_ (Teuchos::createMpiComm<int> (rawMpiComm))
    {}

    //! Destructor (virtual for memory safety of derived classes).
    virtual ~MpiPlatform() {}

    //@}

    //! @name Class Creation and Accessor Methods
    //@{ 

    //! The \c Teuchos::Comm instance with which this object was created.
    RCP<const Comm<int> > getComm() const {
      return comm_; 
    }

    /// \brief The Kokkos Node instance for this platform to use.
    /// 
    /// This MpiPlatform specialization's constructor may have created
    /// the Node instance, if you invoked the default constructor or
    /// passed in a null Node pointer.
    RCP<Kokkos::DefaultNode::DefaultNodeType> getNode() const {
      return node_;
    }

    //@}

  private:
    //! Unimplemented copy constructor (syntactically forbidden).
    MpiPlatform (const MpiPlatform<Kokkos::DefaultNode::DefaultNodeType> &platform);

    //! Unimplemented assignment operator (syntactically forbidden).
    MpiPlatform& operator= (const MpiPlatform<Kokkos::DefaultNode::DefaultNodeType> &platform);

  protected: 
    //! Teuchos::Comm object instantiated for the platform.
    RCP<Teuchos::MpiComm<int> > comm_;

    //! Kokkos Node object instantiated for the platform.
    RCP<Kokkos::DefaultNode::DefaultNodeType> node_;
  };


} // namespace Tpetra

#endif // TPETRA_MPIPLATFORM_HPP
