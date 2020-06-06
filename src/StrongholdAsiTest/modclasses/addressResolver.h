#ifndef ADDRESSRESOLVER
#define ADDRESSRESOLVER

#include "IntSafe.h" // for DWORD
#include "modBase.h"
#include "addressBase.h"
#include "versionGetter.h"

namespace modclasses
{
  // used to express how critical the use of the address is
  enum class AddressRisk
  {
    SAVE,	        // no problem if others use this (only read maybe, might produce garbage if others change them, but not break)
    WARNING,      // likely modifing a value, might not need special handling for now
    CRITICAL      // changes overall flow / redirects functions etc., could lead to incompatility issues between mods
  };

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

    // a little redundant... maybe remove later, or always call functions
    std::weak_ptr<AddressBase> addrBase{};
    std::weak_ptr<AddressBase> verGet{};
    Version version{ Version::NONE };
    DWORD addressBase{ 0x0 };

  public:

    ModType getModType() const override
    {
      return ModType::VERSION_GET;
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

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    AddressResolver(const AddressResolver &base) = delete;
    virtual AddressResolver& operator=(const AddressResolver &base) final = delete;
  };
}

#endif //ADDRESSRESOLVER
