#ifndef MODMANAGER
#define MODMANAGER

#include "enumBase.h"

// json
#include "../../dependencies/JsonForModernC++/json.hpp"

// forward declared needed classes
namespace modclasses
{
  class ModBase;
}

namespace modcore
{
  class ModKeeper;
}

// template for EnumType structure
namespace modcore
{
  using ModCreator = std::function<const std::shared_ptr<modclasses::ModBase>(
    const std::weak_ptr<ModKeeper> modKeeper, const nlohmann::json& config)>;

  // structure intended as static creation center -> allow to register mods as pseudoEnums
  // this ID will then be used as MODID -> needs to be provided to other in some way, if there is any intention
  // to make funtion accessable for other mods -> make them public inline static
  inline static constexpr char modManager[]{ "ModManager" };
  struct ModManager : EnumBase<modManager, ModCreator, true>
  {

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
  };
}

// TODO: should really rethink namespaces... there are currently two:
// modcore and modclasses -> they overlap to much
using ModMan = modcore::ModManager;
using ModID = modcore::ModManager::EnumType;
using ModIDKeeper = modcore::ModManager::EnumKeeper;
using ModCreatorFunc = modcore::ModCreator;

#endif // MODMANAGER