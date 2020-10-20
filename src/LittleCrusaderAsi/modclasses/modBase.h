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

    // calls actual initalize -> simple abstraction, so that the mod author does not need to return a value
    // idea is to remove a little bit the potential to return something different then the result
    const bool callInitialize()
    {
      initialize();
      return initialized;
    }

    // After the attach of the mod asi, thread attach messages arive.
    // At least some process data (aic was where I noticed this) is already loaded, unlike after DLL_PROCESS_ATTACH
    // This is pretty unsafe, until is found out, what causes this
    // There is no initialization control!
    // NOTE: threads that seem to be started afterwards are for example dsound.dll(?), the loading of threads after 
    virtual void firstThreadAttachAfterModAttachEvent(){}

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

    // IMPORTANT: initialize happen after the main game code is loaded, but before anything is executed!
    // if the intention is to access memory that is just beeing written, one must get the load info... (really need an event handler)
    // actual getting addresses, preparing everything
    // if dependencies or init fails, leave "initialized" to false and return false
    // if successful set "initialized" to true and return true
    // TODO -> it is currently possible to fail but return true. should be changed!!!
    virtual void initialize() = 0;

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

    // misusing the Json transform to get enums from strings
    // if there is a better way, I don't know...
    // NOTE: for this to work, String transformations need to be created with NLOHMANN_JSON_SERIALIZE_ENUM()
    // for every enum intended to use, also, the enums apparently must be accessible form the scope of the caller(?)
    template<typename T>
    const T getEnumFromString(const std::string enumString) const
    {
      Json key = enumString;
      return key.get<T>();
    }
  };
}

#endif //MODBASE
