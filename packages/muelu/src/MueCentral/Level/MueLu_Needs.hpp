#ifndef MUELU_NEEDS_HPP
#define MUELU_NEEDS_HPP

#include <Teuchos_ParameterList.hpp>
#include <Teuchos_TestForException.hpp>

#include "MueLu_ConfigDefs.hpp"
#include "MueLu_BaseClass.hpp"
#include "MueLu_Exceptions.hpp"

#include "MueLu_ExtendedHashtable.hpp"

namespace MueLu {
  
  /*!
    @class Needs
    @brief Class that allows cross-factory communication of data needs.

    Maintains a list of 'Needs' for a given Level. For example, a restriction factory that
    transposes the tentative prolongator 'Needs' the prolongator factory to save this.

    The data is stored using a variable name and a pointer to the generating factory. The
    pointer to the generating factory is only used as "key". For the Needs class it doesn't
    matter if the given factory pointer is valid or not. So the NULL pointer can also be used.

    A reference counter keeps track of the storage and automatically frees the memory if
    the data is not needed any more. In the standard mode, the data first has to be requested
    by calling the Request function. Then the data can be set by calling Set.
    With Get the user can fetch data when needed. Release decrements the reference counter
    for the given variable.
  */
  class Needs : public BaseClass {

  private:

    UTILS::ExtendedHashtable countTable_; //<! Stores number of outstanding requests for a need.
    UTILS::ExtendedHashtable dataTable_;  //<! Stores data associated with a need.

  public:

    //! @name Constructors/Destructors.
    //@{

    //! Default constructor.
    Needs() { }

    virtual ~Needs() { }

    //@}

    //! @name Set
    //! @brief functions for setting data in data storage
    //@{

    //! Store need label and its associated data. This does not increment the storage counter.
    template <class T>
    void Set(const std::string & ename, const T & entry, const FactoryBase* factory) {
      // Check if data is requested
      if(countTable_.isKey(ename, factory) && countTable_.Get<int>(ename, factory) != 0) {
	if(!countTable_.isKey(ename, factory))
	  countTable_.Set(ename, 0, factory); // make sure that 'ename' is counted
	dataTable_.Set(ename, entry, factory);
      }
    } // Set

    //@}

    //! @name Request/Release data

    //@{

    //! Indicate that an object is needed. This increments the storage counter.
    void Request(const std::string & ename, const FactoryBase* factory) {
      // If it's the first request for 'ename', create a new key in the hashtable
      if(!countTable_.isKey(ename, factory))
        countTable_.Set(ename, 0, factory);

      // Increment counter
      IncrementCounter(ename, factory);
    } //Request

    //! Decrement the storage counter.
    void Release(const std::string & ename, const FactoryBase* factory) {
      // We can only release data if the key 'ename' exists in countTable (and dataTable)
      TEST_FOR_EXCEPTION(!countTable_.isKey(ename, factory), Exceptions::RuntimeError, "MueLu::Needs::Release(): " + ename + " not found. Do a request first.");

      // Decrement reference counter
      DecrementCounter(ename, factory);

      // Desallocation if counter gets zero
      if (countTable_.Get<int>(ename, factory) == 0) {
        countTable_.Remove(ename, factory);
        if(dataTable_.isKey(ename, factory) )
          dataTable_.Remove(ename, factory);
      }
    } //Release

    //@}

    //! @name Get data
    //@{

    //! @brief Get data without decrementing associated storage counter (i.e., read-only access)
    // Usage: Level->Get< RCP<Operator> >("A", A, factoryPtr)
    template <class T>
    void Get(const std::string & ename, T &value, const FactoryBase* factory) const {
      dataTable_.Get<T>(ename, value, factory);
    }

    //! @brief Get data without decrementing associated storage counter (i.e., read-only access)
    // Usage: Level->Get< RCP<Operator> >("A", factoryPtr)
    template <class T>
    T & Get(const std::string & ename, const FactoryBase* factory) {
      TEST_FOR_EXCEPTION(!dataTable_.isKey(ename, factory), Exceptions::RuntimeError, "MueLu::Needs::Get(): " + ename + " not found in dataTable_");
      return dataTable_.Get<T>(ename, factory);
    }

    //@}

    //! @name Permanent storage
    //@{

    //! @brief Keep variable 'ename' generated by 'factory'. This variable is not handled by the internal reference counter
    void Keep(const std::string & ename, const FactoryBase* factory = NULL) {
      countTable_.Set(ename, -1, factory);
    }

    //! @brief returns true, if variable 'ename' generated by 'factory' is permanently stored 
    bool isKept(const std::string & ename, const FactoryBase* factory) const {
      if (countTable_.Get<int>(ename, factory) == -1) 
	return true;
      else
	return false;
    }

    //! @brief remove the permanently stored variable 'ename' generated by 'factory' 
    void Delete(const std::string & ename, const FactoryBase* factory) {
      if (!countTable_.isKey(ename, factory)) return; // data not available? TODO: Exception ??

      if (countTable_.Get<int>(ename, factory) != -1) {
	GetOStream(Errors, 0) << "Needs::Delete(): This method is intended to be used when the automatic garbage collector is disabled. Use Release() instead to decrement the reference counter!" << std::endl;
      }

      countTable_.Remove(ename, factory);
      if (dataTable_.isKey(ename, factory))
	dataTable_.Remove(ename, factory);
    }

    //@}

    //! @name Utilities.
    //@{

    //! Test whether a need's value has been saved.
    bool IsAvailable(const std::string & ename, const FactoryBase* factory) {
      return dataTable_.isKey(ename, factory);
    }

    //! Test whether a need has been requested.  Note: this tells nothing about whether the need's value exists.
    bool IsRequested(const std::string & ename, const FactoryBase* factory) {
      return countTable_.isKey(ename, factory);
    }

    //! Test whether a factory is generating factory of a requested variable in Needs
    bool IsRequestedFactory(const FactoryBase* factory) {
      std::vector<std::string> ekeys = RequestedKeys();
      for (std::vector<std::string>::iterator it = ekeys.begin(); it != ekeys.end(); it++) {
	std::vector<const FactoryBase*> ehandles = RequestedHandles(*it);
	for (std::vector<const FactoryBase*>::iterator kt = ehandles.begin(); kt != ehandles.end(); kt++) {
	  if(*kt == factory) // factory is generating factory of requested variable '*it'
	    return true;
	}
      }
      return false;
    }

    //! Test whether a factory is among the generating factories of data that is already available
    bool IsAvailableFactory(const FactoryBase* factory) {
      std::vector<std::string> ekeys = AvailableKeys();
      for (std::vector<std::string>::iterator it = ekeys.begin(); it != ekeys.end(); it++) {
	std::vector<const FactoryBase*> ehandles = AvailableHandles(*it);
	for (std::vector<const FactoryBase*>::iterator kt = ehandles.begin(); kt != ehandles.end(); kt++) {
	  if(*kt == factory) // factory is generating factory of requested variable '*it'
	    return true;
	}
      }
      return false;
    }

    //! @brief Return the number of outstanding requests for a need.
    //!  Throws a <tt>Exceptions::RuntimeError</tt> exception if the need either hasn't been requested or hasn't been saved.
    int NumRequests(const std::string & ename, const FactoryBase* factory) const {
      //FIXME should we return 0 instead of throwing an exception?
      TEST_FOR_EXCEPTION(!countTable_.isKey(ename, factory), Exceptions::RuntimeError, "MueLu::Needs::NumRequests(): " + ename + " not found in countTable_");
      return countTable_.Get<int>(ename, factory);
    }

    //! @name Helper functions
    //@{

    //! Returns a vector of strings containing all key names of requested variables
    std::vector<std::string> RequestedKeys() const {
      return countTable_.keys();
    }

    std::vector<const FactoryBase*> RequestedHandles(const std::string & ename) const {
      return countTable_.handles(ename);
    }

    //! Returns a vector of strings containing all key names of available variables
    std::vector<std::string> AvailableKeys() {
      return dataTable_.keys();
    }

    std::vector<const FactoryBase*> AvailableHandles(const std::string & ename) {
      return dataTable_.handles(ename);
    }

    std::string GetType(const std::string & ename, const FactoryBase* fac) const {
      return dataTable_.GetType(ename, fac);
    }

    //@}

    //! @name I/O Functions
    //@{

    //! Printing method
    void print(Teuchos::FancyOStream &out, const VerbLevel verbLevel = Default) const {
      Teuchos::TabularOutputter outputter(out);
      outputter.pushFieldSpec("name", Teuchos::TabularOutputter::STRING, Teuchos::TabularOutputter::LEFT, Teuchos::TabularOutputter::GENERAL, 32);
      outputter.pushFieldSpec("gen. factory addr.", Teuchos::TabularOutputter::STRING, Teuchos::TabularOutputter::LEFT, Teuchos::TabularOutputter::GENERAL, 18);
      outputter.pushFieldSpec("req", Teuchos::TabularOutputter::INT, Teuchos::TabularOutputter::LEFT, Teuchos::TabularOutputter::GENERAL, 3);
      outputter.pushFieldSpec("type", Teuchos::TabularOutputter::STRING, Teuchos::TabularOutputter::LEFT, Teuchos::TabularOutputter::GENERAL, 10);
      outputter.pushFieldSpec("data", Teuchos::TabularOutputter::STRING, Teuchos::TabularOutputter::LEFT, Teuchos::TabularOutputter::GENERAL, 20);
      outputter.outputHeader();

      std::vector<std::string> ekeys = countTable_.keys();
      for (std::vector<std::string>::iterator it = ekeys.begin(); it != ekeys.end(); it++) {
	std::vector<const FactoryBase*> ehandles = countTable_.handles(*it);
	for (std::vector<const FactoryBase*>::iterator kt = ehandles.begin(); kt != ehandles.end(); kt++) {
	  outputter.outputField(*it);   // variable name
	  outputter.outputField(*kt);   // factory ptr
	  int reqcount = 0;             // request counter
	  countTable_.Get<int>(*it, reqcount, *kt);
	  outputter.outputField(reqcount);
	  // variable type
	  std::string strType = dataTable_.GetType(*it, *kt);
	  if(strType.find("Xpetra::Operator")!=std::string::npos) {
	    outputter.outputField("Operator" );
	    outputter.outputField(" ");
	  } else if(strType.find("Xpetra::MultiVector")!=std::string::npos) {
	    outputter.outputField("Vector");
	    outputter.outputField("");
	  } else if(strType == "int") {
	    outputter.outputField(strType);
	    int data = 0; dataTable_.Get<int>(*it, data, *kt);
	    outputter.outputField(data);
	  } else if(strType == "double") {
	    outputter.outputField(strType);
	    double data = 0.0; dataTable_.Get<double>(*it, data, *kt);
	    outputter.outputField(data);
	  } else if(strType == "string") {
	    outputter.outputField(strType);
	    std::string data = ""; dataTable_.Get<std::string>(*it, data, *kt);
	    outputter.outputField(data);
	  } else {
	    outputter.outputField(strType);
	    outputter.outputField("unknown");
	  }

	  outputter.nextRow();
	}
      }
    }

    //@}

  private:

    //! @name Helper functions
    //@{

    //! @brief Increments counter for variable <tt>ename</tt> (generated by the given factory <tt>factory</tt>)
    void IncrementCounter(const std::string & ename, const FactoryBase* factory) {
      int currentCount = countTable_.Get<int>(ename, factory);
      if(currentCount != -1) { // if counter not disabled
	countTable_.Set(ename, ++currentCount, factory);
      }
    }

    //! @brief Decrements counter for variable <tt>ename</tt> (generated by the given factory <tt>factory</tt>)
    void DecrementCounter(const std::string & ename, const FactoryBase* factory) {
      int currentCount = countTable_.Get<int>(ename, factory);
      if(currentCount != -1 && currentCount != 0) { // if counter not disabled
	countTable_.Set(ename, --currentCount, factory);
      }
      // TODO: if currentCount == 0 -> Exception??
    }

    //@}

  private:

    //! Copy constructor
    Needs(const Needs & source) { }

  }; //class Needs

} //namespace MueLu

#define MUELU_NEEDS_SHORT

#endif //ifndef MUELU_NEEDS_HPP
