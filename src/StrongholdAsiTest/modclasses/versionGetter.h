#ifndef VERSIONGETTER
#define VERSIONGETTER

#include "IntSafe.h" // for DWORD
#include "modBase.h"
#include "strongVersions.h"
#include "addressBase.h"

namespace modclasses
{
  class VersionGetter : public ModBase
  {
  private:
    // version addresses are placed ans constants here
    // this class will provide a method for the address resolver to add those to the conflict test handle

    // example (not really address)
    const DWORD noRealAddress{ 0xCE505B5A }; // dummy

    Version version{ Version::NONE };

    std::weak_ptr<AddressBase> addrBase{};  // is it nullptr?

  public:
    
    ModType getModType() const override
    {
      return ModType::VERSION_GET;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::ADDRESS_BASE };
    }

    void giveDependencies(const std::vector<std::weak_ptr<ModBase>>) override;
    bool initialize() override;

    /**con- and destructor**/
    // none so far

    /**additional functions for others**/ 

    // return version (will be received 
    const Version getCurrentStrongholdVersion() const
    {
      return version;
    }

    // function that returns whatever the address resolver need for an object
    //std::vector<"object"> getUsedAddresses(); // might even need some sort of smartPointer etc., however, maybe these objects are unique

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    VersionGetter(const VersionGetter &base) = delete;
    virtual VersionGetter& operator=(const VersionGetter &base) final = delete;
  };
}

#endif //VERSIONGETTER
