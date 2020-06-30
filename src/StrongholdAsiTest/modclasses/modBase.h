#ifndef MODBASE
#define MODBASE

#include <vector>
#include <memory>

#include "IntSafe.h" // for DWORD
#include "enumheaders/modtypes.h"
#include "enumheaders/addressEnums.h"
#include "enumheaders/strongVersions.h"

// logging
#include "../dependencies/logger/easylogging++.h"
#include "../modcore/logUtility.h" // used to mark that log needs to be copied at the end

// json
#include "../dependencies/jsonParser/json.hpp"

namespace modclasses
{
  using Json = nlohmann::json;

  class ModBase; // forward declaration, to make allow the dependency container to see ModBase

  // container to wrap differnt modtypes
  struct DependencyRecContainer
  {
    virtual void receiveDep(const std::shared_ptr<ModBase> depPointer) = 0;
  };

  // container to keep a pointer to the mod weakptr, since this shouldn't go out of scope
  // the delivered modtype is casted static, since the system should use a map to only deliver the fitting values
  template<typename T>
  struct DependencyReceiver : public DependencyRecContainer
  {
    std::weak_ptr<T> *weakPointerPointer;

    DependencyReceiver(std::weak_ptr<T> *pointerToWeakPointer) : weakPointerPointer{ pointerToWeakPointer }{}

    void receiveDep(const std::shared_ptr<ModBase> depPointer) override
    {
      *weakPointerPointer = std::static_pointer_cast<T>(depPointer);
    }
  };

  // base class for all mod impl
  class ModBase
  {
  private:

    // holds depReceiver, will be added on request and then set to null once the dep are done
    std::unique_ptr<std::unordered_map<ModType, std::unique_ptr<DependencyRecContainer>>> depReceiverPointer{};
  
  protected:

    bool initialized{ false };

  public:

    // default cons
    // the constructor should get either nothing (getting everything from somewhere else)
    // or a "basic" object, like a json-node (still to determine) for config
    // constr should then be used to extract the relevant infos from this or mark the init as failed if not possible, etc
    // pointer and address stuff needs to be done in "initialize"!
    // config will be send as ref, but will be created in the modloaderConstr and discarded afterwards, so create a lokal copy if needed
    ModBase(){ }

    // simply returns if initialzed
    virtual bool initialisationDone() const final
    {
      return initialized;
    }

    // get type of this mod
    virtual ModType getModType() const = 0;

    // new dependency approach -> request and add are handled automatic, the modauthor only needs to provide "neededDependencies"
    // get by sending key map
    virtual std::vector<ModType> getDependencies() final
    {
      // get receiver object
      depReceiverPointer = neededDependencies();

      std::vector<ModType> neededTypes;
      neededTypes.reserve(depReceiverPointer->size());
      
      for (auto const& type : *depReceiverPointer)
      {
        neededTypes.push_back(type.first);
      }

      return neededTypes;
    };

    virtual void giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep) final
    {
      for (auto const& depPointer : dep)
      {
        if (auto depConIt = depReceiverPointer->find(depPointer->getModType()); depConIt != depReceiverPointer->end())
        {
          depConIt->second->receiveDep(depPointer);
        }
        else
        {
          LOG(WARNING) << "Received dependency for mod with id '" << static_cast<int>(depPointer->getModType())
            << "' for mod with id '" << static_cast<int>(this->getModType()) << "'. But it wasn't needed.";
        }
      }

      // empties object, no longer needed
      depReceiverPointer.reset();
    };

    // actual getting addresses, preparing everything
    // if dependencies or init fails, leave "initialized" to false and return false
    // if successful set "initialized" to true and return true
    virtual bool initialize() = 0;

    // additional function explizit called before letting the destructors run
    // default does nothing, since I see no real use, put I leave it here
    // NOTE: additionally, cleanUp will be called at the end in reverse dependency order, so mods that depend on others get this call first
    // destructor calls might be arbitrary
    virtual void cleanUp(){}

    virtual ~ModBase(){} // virtual destructor in case we delete an IErrorLog pointer, so the proper derived destructor is called

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    ModBase(const ModBase &base) = delete;
    virtual ModBase& operator=(const ModBase &base) final = delete;

  private:

    // will return 
    virtual std::unique_ptr<std::unordered_map<ModType, std::unique_ptr<DependencyRecContainer>>> neededDependencies() = 0;

  protected:

    // some utility functions

    // needs the ModClass as Template
    // checks if the mod is there and initialized and returns a sharedPointer
    // if there is no mod or the mod is not initialized, an empty pointer is returned
    template<typename T>
    const std::shared_ptr<T> getIfModInit(const std::weak_ptr<T> &modPointer)
    {
      auto sharedModPointer = modPointer.lock();
      if (sharedModPointer && sharedModPointer->initialisationDone())
      {
        return sharedModPointer;
      }
      else
      {
        return std::shared_ptr<T>{};
      }
    }
  };
}

#endif //MODBASE
