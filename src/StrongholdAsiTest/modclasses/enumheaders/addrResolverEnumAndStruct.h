#ifndef ADDRESSRESOLVERENUM
#define ADDRESSRESOLVERENUM

#include "../modBase.h" // for enums

namespace modclasses
{
  // used to express how critical the use of the address is
  enum class AddressRisk
  {
    NONE,             // for config -> doesn't change default value
    SAFE,	            // no problem if others use this (only read maybe, might produce garbage if others change them, but not break)
    WARNING,          // likely modifing a value, might not need special handling for now
    FUNCTION_WARNING, // special instance to mark function code that is not called anymore by some actions if a mod is activated, but is still called by other
    CRITICAL          // changes overall flow / redirects functions etc. and has a good chance of breaking things, also to be used for code that is "killed" by changes and is not accessed anymore
  };

  // used to parse string to enum
  NLOHMANN_JSON_SERIALIZE_ENUM(AddressRisk, {
    {AddressRisk::NONE, nullptr},
    {AddressRisk::SAFE, "safe"},
    {AddressRisk::WARNING, "warning"},
    {AddressRisk::FUNCTION_WARNING, "function warning"},
    {AddressRisk::CRITICAL, "critical"}
  })

  // add definition for address request struct
  // at best defined as static vector and then delivered by address
  struct AddressRequest
  {
    // the mod address
    Address address;
    
    // number of addressed bytes including the main address (example: 4 -> 0x1 to 0x4)
    // Version as key, by default Version::NONE is used
    // if adaptions are needed, add the lengths for other versions, it will be checked if the current running version has special treatment
    // use that for exceptions, for default use Version::NONE
    std::unordered_map <Version, size_t> length;
    // likely has the biggest overhead...

    // if true, will change the protection of the memory page that includes the requested memory to execute_readwrite
    // regardless of earlier protection, using the address and the length
    bool allowWrite;

    // the risk level of usage
    // critical is the highest allowed conflict level
    // currently no intention to allow overlaps marked with conflict
    AddressRisk addressRisk;
  };
}

#endif //ADDRESSRESOLVERENUM