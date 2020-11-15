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

    Version version{ Version::NONE };
    DWORD addressBase{ 0x0 };

    // TODO: likely inefficient and slow, so maybe change to a better approach latter
    std::map<DWORD, std::unordered_map<ModType, std::unordered_set<AddressRequest*>>> addressSortContainer{};

  public:

    ModType getModType() const override
    {
      return ModType::ADDRESS_RESOLVER;
    }

    std::vector<ModType> getDependencies() const override;

    /**con- and destructor**/

    // will get a config how to treat address overlaps
    AddressResolver(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config);

    /**additional functions for others**/

    // returns true if all addresses were registered, else false
    // nature of conflicts is logged
    // AddressRequests need to be persistent for the lifetime of the class, since currently pointers are used to store references
    // NOTE: vector pointers are only stable until it runs out of space and needs to allocate more, so take this into account
    // requesting multiple small address amounts should be possible, however (to be honest) I am not as sure
    // also, requesting the same address from the same mod twice might produce unwanted behaviour
    // (ex. requesting a successful request again together with a failing will delete both -> the first will fail)
    const bool requestAddresses(std::vector<AddressRequest> &addrReq, const ModBase &requestingMod);
    // NOTE to impl -> remember that the changes to the check sets only should take effect or be persistent if no conflict or address reject occures

    // maybe TODO?: should it be possible to give addresses free, if a mod fails after obtaining them (maybe setting AddressRisk of them to safe)?

    // receives the address type and the mod class that requests
    // returns a pointer of type 'T' to the requested address
    // NOTE: the template value is made to a pointer so request a int pointer like 'getAddressPointer<int>' and not 'getAddressPointer<*int>'
    // it might also be better to save this pointer for further use to avoid calls if multiple are needed, memory is in this case likely far cheaper
    // throws an exception if the mod has no registered access to the address
    template <typename T>
    T* getAddressPointer(const Address memAddr, const ModBase &requestingMod)
    {
      if (initialized)
      {

        DWORD address{ getAddress(memAddr) };
        if (const auto& modAddrIt = addressSortContainer.find(address); modAddrIt != addressSortContainer.end())
        {
          if (const auto& modIt = (modAddrIt->second).find(requestingMod.getModType()); modIt != (modAddrIt->second).end())
          {
            bool allowed{ false };
            for (const auto& reg : (modIt->second))
            {
              allowed = allowed || reg->address == memAddr;
            }

            if (allowed)
            {
              return reinterpret_cast<T*>(addressBase + address);
            }
          }
        }

        throw std::exception(("The address of type with id '" + std::to_string(static_cast<int>(memAddr))
                              + "' was not approved for mod with id '" + std::to_string(static_cast<int>(requestingMod.getModType())) + "'.").c_str());
      }

      throw std::exception("AddressResolver wasn't successfully initialized, but \"getAddressPointer\" was still called.");
    }

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    AddressResolver(const AddressResolver &base) = delete;
    virtual AddressResolver& operator=(const AddressResolver &base) final = delete;

  private:

    void initialize() override;

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