#ifndef ADDRESSRESOLVER
#define ADDRESSRESOLVER

#include <unordered_set>
#include "IntSafe.h" // for DWORD
#include "modBase.h"
#include "addressBase.h"
#include "versionGetter.h"

#include "enumheaders/addrResolverEnumAndStruct.h"

namespace modclasses
{
  class AddressResolver : public ModBase
  {
  private:

    AddressRisk conflictLevel{ AddressRisk::CRITICAL }; // level on which a address request is rejected if another of this type or higher is already present

    // a little redundant... maybe remove later, or always call functions
    std::weak_ptr<AddressBase> addrBase{};
    std::weak_ptr<VersionGetter> verGet{};
    Version version{ Version::NONE };
    DWORD addressBase{ 0x0 };

    // TODO: likely inefficient and slow, so maybe change to a better approach latter
    std::map<DWORD, std::unordered_map<ModType, std::unordered_set<AddressRequest*>>> addressSortContainer{};

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
    AddressResolver(const Json &config);

    /**additional functions for others**/

    // returns true if all addresses were registered, else false
    // nature of conflicts is logged
    // AddressRequests need to be persistent for the lifetime of the class, since currently pointers are used to store references
    const bool requestAddresses(std::vector<AddressRequest> &addrReq, const ModBase &requestingMod);
    // NOTE to impl -> remember that the changes to the check sets only should take effect or be persistent if no conflict or address reject occures

    // maybe TODO?: should it be possible to give addresses free, if a mod fails after obtaining them (maybe setting AddressRisk of them to safe)?

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

    // for intern resolve
    const bool checkIfAddressRiskViolated(AddressRisk newRisk, AddressRisk oldRisk);
    const std::pair<DWORD, DWORD> getStartAndEndAddress(const AddressRequest& req);

    // fills addressesWhereToAddRequest, addToNewAddressStart and addToNewAddressEnd
    // also return a bool that says "true" if the risk level was violated
    const bool checkRiskAndOverlaps(
      const ModType requestModType, const AddressRequest &newToAddReq, const std::pair<DWORD, DWORD> newAddrStartEnd,
      const std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &reqInPlace, std::unordered_set<DWORD> &addressesWhereToAddRequest,
      std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &addToNewAddressStart, std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &addToNewAddressEnd);
  };
}

#endif //ADDRESSRESOLVER