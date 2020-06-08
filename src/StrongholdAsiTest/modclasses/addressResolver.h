#ifndef ADDRESSRESOLVER
#define ADDRESSRESOLVER

#include <unordered_set>
#include "IntSafe.h" // for DWORD
#include "modBase.h"
#include "addressBase.h"
#include "versionGetter.h"

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
  // at best defined as static (const) vector and then delivered by address
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

    // the risk level of usage
    AddressRisk addressRisk;
  };

  class AddressResolver : public ModBase
  {
  private:

    // TODO: make changeable using config
    AddressRisk conflictLevel{ AddressRisk::CRITICAL }; // level on which a address request is rejected if another of this type or higher is already present

    // a little redundant... maybe remove later, or always call functions
    std::weak_ptr<AddressBase> addrBase{};
    std::weak_ptr<VersionGetter> verGet{};
    Version version{ Version::NONE };
    DWORD addressBase{ 0x0 };

    // TODO: likely inefficient and slow, so maybe change to a better approach latter
    std::map<DWORD, std::vector<AddressRequest*>> conflictCheckerMap; // maybe think of better way?
    std::unordered_map<ModType, std::unordered_set<Address>> approvedAddresses;

  public:

    ModType getModType() const override
    {
      return ModType::ADDRESS_RESOLVER;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::ADDRESS_BASE, ModType::VERSION_GET };
    }

    void giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep) override;
    bool initialize() override;

    /**con- and destructor**/

    // will get a config how to treat address overlaps
    AddressResolver(Json config);

    /**additional functions for others**/

    // returns true if all addresses were registered, else false
    // nature of conflicts is logged
    // AddressRequests need to be persistent for the lifetime of the class, since currently pointers are used to store references
    const bool requestAddresses(const std::vector<AddressRequest> &addrReq, const ModBase &requestingMod);
    // NOTE to impl -> remember that the changes to the check sets only should take effect or be persistent if no conflict or address reject occures
    
    // receives the address type and the mod class that requests
    // returns a pointer of type 'T' to the requested address
    // NOTE: the template value is made to a pointer so request a int pointer like 'getAddressPointer<int>' and not 'getAddressPointer<*int>'
    // it might also be better to save this pointer for further use to avoid calls if multiple are needed, memory is in this case likely far cheaper
    // throws an exception if the mod has no registered access to the address
    template <typename T>
    const T* getAddressPointer(const Address memAddr, const ModBase &requestingMod);

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    AddressResolver(const AddressResolver &base) = delete;
    virtual AddressResolver& operator=(const AddressResolver &base) final = delete;

  private:

    const DWORD getAddress(const Address memAddr);
  };
}

#endif //ADDRESSRESOLVER
