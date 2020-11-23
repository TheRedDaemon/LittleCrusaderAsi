#ifndef VERSIONGETTER
#define VERSIONGETTER

#include "modBase.h"
#include "addressBase.h"
#include "enumheaders/addrResolverEnumAndStruct.h" // to send request object

namespace modclasses
{
  class VersionGetter : public ModBase
  {
  private:
    // version addresses are placed ans constants here
    // this class will provide a method for the address resolver to add those to the conflict test handle

    // currently relative addresses (do I even need them if I am in the same address scope?)
    // additional, version number and whole version two addresses, until better method found
    // also no version text overwrite currently

    const DWORD relExtremeVersion41String{ 0x1A1FE8 }; // 9 byte string (using 10)
    const DWORD relExtremeVersion41Number{ 0x24EFC }; // 1 byte

    const DWORD relVersion41String{ 0x1A1FDC }; // 9 byte string (using 10)
    const DWORD relVersion41Number{ 0x24EEC }; // 1 byte

    // needed to give the address resolver the right infos
    static std::vector<AddressRequest> usedAddresses;

    Version version{ Version::NONE };

  public:

    // declare public -> request mod registration and receive id (or nullptr)
    inline static ModIDKeeper ID{ 
      ModMan::RegisterMod( "versionGetter", [](const Json&)
      {
        return std::static_pointer_cast<ModBase>(std::make_shared<VersionGetter>());
      }) 
    };
    
    ModID getModID() const override
    {
      return ID;
    }

    std::vector<ModID> getDependencies() const override
    {
      return { AddressBase::ID };
    }

    /**con- and destructor**/
    VersionGetter(){}

    /**additional functions for others**/ 

    // return version (will be received 
    const Version getCurrentStrongholdVersion() const
    {
      if (initialized)
      {
        return version;
      }
      throw std::exception("VersionGetter wasn't successfully initialized, but \"getCurrentStrongholdVersion\" was still called.");
    }

    // might even need some sort of smartPointer etc., however, maybe these objects are unique
    std::vector<AddressRequest>& returnUsedAddresses()
    {
      return usedAddresses;
    }

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    VersionGetter(const VersionGetter &base) = delete;
    virtual VersionGetter& operator=(const VersionGetter &base) final = delete;

  private:

    void initialize() override;
  };
}

#endif //VERSIONGETTER
