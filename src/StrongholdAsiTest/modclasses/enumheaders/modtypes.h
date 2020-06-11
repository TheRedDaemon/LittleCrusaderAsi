#ifndef MODTYPES
#define MODTYPES  // header gard without seperator to avoid problems with enums with the same name

// json
#include "../dependencies/jsonParser/json.hpp"

namespace modclasses 
{
  // needs to contain every single mod type,
  // will also be used to define dependencies
  enum class ModType
  {
    NONE, // for invalif stuff
    ADDRESS_BASE,         // no dependency, gets at least the base class
    VERSION_GET,       // uses own set of addresses to determine version, requires ADDRESS_BASE
    ADDRESS_RESOLVER,     // gets version from the VERSION_GETTER, other modules should request use of addresses, requires ADDRESS_BASE and VERSION_GETTER
    KEYBOARD_HANDLER,     // planned module for handling keyboard stuff
    EVENT_HANDLER,         // planned module for handling events, by intercepting functions and execute others before giving control back to system
    
    
    
    
    
    
    TEST1,             // dummy, will be needy
    TEST2              // dummy, will be used for cyclic ref with TEST1
  };

  // used to parse string to enum
  NLOHMANN_JSON_SERIALIZE_ENUM(ModType, {
    {ModType::NONE, nullptr},
    {ModType::VERSION_GET, "versionGetter"},
    {ModType::ADDRESS_RESOLVER, "addressResolver"},
    {ModType::TEST1, "test1"},
    {ModType::TEST2, "test2"}
  })
}

#endif //MODTYPES
