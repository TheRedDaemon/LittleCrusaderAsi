#ifndef MODBASE
#define MODBASE

#include <vector>
#include <memory>

#include "modtypes.h"

// logging
#include "../dependencies/logger/easylogging++.h"
#include "../modcore/logUtility.h" // used to mark that log needs to be copied at the end

// json
#include "../dependencies/jsonParser/json.hpp"

namespace modclasses
{
  using Json = nlohmann::json;

  // base class for all mod impl
  class ModBase
  {
  
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

    // returnes required dependencies
    virtual std::vector<ModType> getDependencies() const = 0;

    // give required dependencies, but as generalized vector
    // shared ptr for better casting, however, better use weak_ptr in the classes
    virtual void giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep) = 0;

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
  };
}

#endif //MODBASE
