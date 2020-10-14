#ifndef MODBASE
#define MODBASE

#include <vector>
#include <memory>

#include "IntSafe.h" // for DWORD
#include "enumheaders/modtypes.h"
#include "enumheaders/addressEnums.h"
#include "enumheaders/strongVersions.h"

#include "../modcore/modKeeper.h"

// json
#include "../dependencies/jsonParser/json.hpp"

namespace modclasses
{
  using Json = nlohmann::json;

  // base class for all mod impl
  class ModBase
  {
  private:

    std::weak_ptr<modcore::ModKeeper> keeper{};
  
  protected:

    bool initialized{ false };

  public:

    // default cons
    // the constructor should get either nothing (getting everything from somewhere else)
    // or a "basic" object, like a json-node (still to determine) for config
    // constr should then be used to extract the relevant infos from this or mark the init as failed if not possible, etc
    // pointer and address stuff needs to be done in "initialize"!
    // config will be send as ref, but will be created in the modloaderConstr and discarded afterwards, so create a lokal copy if needed
    ModBase() {}

    // additional constructor created for mods that have dependencies
    // use this in your constructor, otherwise receiving mods will not be possible
    // more info see default constructor
    ModBase(const std::weak_ptr<modcore::ModKeeper> modKeeper) : keeper{ modKeeper }{}

    // simply returns if initialzed
    virtual bool initialisationDone() const final
    {
      return initialized;
    }

    // get type of this mod
    virtual ModType getModType() const = 0;

    // requested mods -> dependencies are created before will be available to be used by this mod
    virtual std::vector<ModType> getDependencies() const = 0;

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

  protected:

    // some utility functions

    // return shared pointer of mod if there, requested, initialized
    // otherwise the return will be an empty pointer
    // if the mod should be used further, use a weak pointer to store it in the class
    template<typename T>
    const std::shared_ptr<T> getMod()
    {
      // double test, but i do not have access to initialisationDone otherwise, or not?
      auto keeperPointer = keeper.lock();
      if (keeperPointer)
      {
        auto modPointer = keeperPointer->getModIfInitAndReq<T>(getModType());
        if (modPointer && modPointer->initialisationDone())
        {
          return modPointer;
        }
      }
      else
      {
        LOG(WARNING) << "The mod with id '" << static_cast<int>(getModType()) << "' tried to receive a mod but has no keeper.";
      }

      return std::shared_ptr<T>{};
    }
  };
}

#endif //MODBASE
