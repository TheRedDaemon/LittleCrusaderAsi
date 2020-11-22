#ifndef MODKEEPER
#define MODKEEPER

#include "modManager.h"

// logging
#include "../dependencies/easylogging++/easylogging++.h"
#include "../modcore/logUtility.h" // used to mark that log needs to be copied at the end

namespace modcore
{
  using Mod = modclasses::ModBase;

  class ModKeeper final // preventing inheritance, since destructor wont be virtual...
  {
  private:

    // keeps the mod together with its enum and the mods that requested it
    // also keeps friend function for find
    struct ModContainer final
    {
      ModID modEnum;
      std::shared_ptr<modclasses::ModBase> mod{};
      std::vector<ModID> modsThatNeedThis{};

      // for make_shared, modsThatNeedThis is initialized empty
      ModContainer(ModID modMt, std::shared_ptr<modclasses::ModBase> modPtr) : modEnum(modMt), mod(modPtr) {}
    };

    std::vector<std::shared_ptr<ModContainer>> loadedMods{};

    HMODULE handle;

  public:

    ModKeeper() = delete;
    ModKeeper(HMODULE ownModuleHandle) : handle{ownModuleHandle} {}

    ~ModKeeper(){}

    // needs the ModClass as Template
    // checks if the mod is there, initialized and requested
    // returns a shared pointer (prefer saving it as weak_ptr in the mod classes)
    // if there is no mod or is not a dependency of the requesting an empty pointer is returned
    // the later writes also a message to a log file
    template<typename T>
    const std::shared_ptr<T> getModIfInitAndReq(ModID requestingModType)
    {
      std::shared_ptr<T> modPointer;

      if (!loadedMods.empty()) // check if no mods there
      {
        size_t counter{ 0 };
        do
        {
          modPointer = std::dynamic_pointer_cast<T>(loadedMods[counter]->mod);
          ++counter;
        }
        while (!modPointer && counter < loadedMods.size());

        if (modPointer)
        {
          ModContainer& modCon = *(loadedMods[counter - 1]);

          if (std::find(modCon.modsThatNeedThis.begin(), modCon.modsThatNeedThis.end(), requestingModType) != modCon.modsThatNeedThis.end())
          {
            return modPointer;
          }
          else
          {
            LOG(WARNING) << "The mod '" << requestingModType->getName() << "' tried to receive a non-requested mod with id '" 
              << modCon.modEnum->getName() << "'.";
          }
        }
      }

      return std::shared_ptr<T>{};
    }

    HMODULE getOwnModuleHandle()
    {
      return handle;
    }

    /**misc**/

    // prevent copy and assign (not sure how necessary, or if I done it right...)
    ModKeeper(const ModKeeper &base) = delete;
    virtual ModKeeper& operator=(const ModKeeper &base) final = delete;
  
  private:

    friend class ModLoader; // allows access modContainer vector to fill
  };
}

#endif //MODKEEPER