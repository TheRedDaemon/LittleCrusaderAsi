#ifndef MODMANAGER
#define MODMANAGER

#include "enumBase.h"

// json
#include "../../dependencies/JsonForModernC++/json.hpp"

// logging
// NOTE: during class init, using logs will lead to exceptions so prevent use
#include "../dependencies/easylogging++/easylogging++.h"
#include "../modcore/logUtility.h" // used to mark that log needs to be copied at the end

// forward declared needed classes
namespace modclasses
{
  class ModBase;
}

namespace modcore
{
  using ModCreator = std::function<const std::shared_ptr<modclasses::ModBase>(const nlohmann::json& config)>;

  // structure intended as static creation center -> allow to register mods as pseudoEnums
  // this ID will then be used as MODID -> needs to be provided to other in some way, if there is any intention
  // to make funtion accessable for other mods -> make them public inline static
  // now truly mod manager -> instead of ol ModKeeper, this takes care of general functions
  inline static constexpr char modManager[]{ "ModManager" };
  struct ModManager : EnumBase<modManager, ModCreator, true>
  {
  private:

    // keeps the mod together with the mods that requested it
    struct ModContainer final
    {
      std::shared_ptr<modclasses::ModBase> mod{};
      std::vector<EnumType> modsThatNeedThis{};
    };

    // keeps the loaded mods -> order is not important here
    inline static std::unordered_map<EnumType, ModContainer> loadedMods{};

    // keeps direct poiter to mods in intended init order
    inline static std::vector<modclasses::ModBase*> orderedMods{};

    inline static HMODULE handle{ nullptr };

  public:

    static EnumKeeper RegisterMod(std::string &&name, ModCreator &&creatorFunc)
    {
      // register creator, return enum
      // concept of nullptr return is stopped by the dependency fulfill
      // -> there is a garantuee that the mod objects are at least created
      //   -> they may say "not initialized", but will be still there
      //   -> leaving the abort -> if it is forced, it should not leave dev anyway
      return CreateEnum(std::forward<std::string>(name), std::forward<ModCreator>(creatorFunc));
    }
    // calling GetEnumByName allows now access to the creation function


    static HMODULE GetOwnModuleHandle()
    {
      return handle;
    }


    // needs the ModClass as Template
    // checks if the mod is there, initialized and requested
    // returns a shared pointer (prefer saving it as weak_ptr in the mod classes)
    // if there is no mod or is not a dependency of the requesting an empty pointer is returned
    // the later writes also a message to a log file
    // NOTE: receiveable mods need T::ID to be the mod id
    template<typename T>
    static const std::shared_ptr<T> GetModIfReq(EnumType requestingModType)
    {
      auto it{ loadedMods.find(T::ID) };
      if (it == loadedMods.end()) // check if this mod is not there
      {
        return std::shared_ptr<T>{};
      }
        
      ModContainer& modCon{ it->second };

      // check if mod was requested
      if (std::find(modCon.modsThatNeedThis.begin(), modCon.modsThatNeedThis.end(), requestingModType) != modCon.modsThatNeedThis.end())
      {
        return std::static_pointer_cast<T>(modCon.mod);
      }
      else
      {
        LOG(WARNING) << "The mod '" << requestingModType->getName() << "' tried to receive the non-requested mod '"
          << it->first->getName() << "'.";
        return std::shared_ptr<T>{};
      }
    }


    // makes it friend from ModLoader, to make initalize easier and hide stuff from mods
    friend class ModLoader; // allows access modContainer vector to fill
  };
}

// TODO: should really rethink namespaces... there are currently two:
// modcore and modclasses -> they overlap to much
using ModMan = modcore::ModManager;
using ModID = modcore::ModManager::EnumType;
using ModIDKeeper = modcore::ModManager::EnumKeeper;
using ModCreatorFunc = modcore::ModCreator;

#endif // MODMANAGER