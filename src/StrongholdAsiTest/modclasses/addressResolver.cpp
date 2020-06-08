
#include "addressResolver.h"

namespace modclasses
{

  AddressResolver::AddressResolver(Json config)
  {
    auto confIt = config.find("rejectLevel");
    if (confIt != config.end())
    {
      AddressRisk configConflictLevel{ confIt.value().get<AddressRisk>() };

      if (configConflictLevel != AddressRisk::NONE)
      {
        conflictLevel = configConflictLevel;
      }
    }
  }

  void AddressResolver::giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep)
  {
    if (dep.size() == 2)
    {
      for (const auto &mod : dep)
      {
        switch (mod->getModType())
        {
          case ModType::ADDRESS_BASE:
            addrBase = std::static_pointer_cast<AddressBase>(mod);
            break;
          case ModType::VERSION_GET:
            addrBase = std::static_pointer_cast<VersionGetter>(mod);
            break;
          default:
            break;
        }
      }
    }

    if (addrBase.expired() || verGet.expired())
    {
      LOG(WARNING) << "AddressResolver failed to receive dependencies.";
    }
  }

  bool AddressResolver::initialize()
  {
    if (auto addressBaseRec = addrBase.lock())
    {
      if (auto versionGetter = verGet.lock())
      {
        if (addressBaseRec->initialisationDone() && versionGetter->initialisationDone())
        {
          addressBase = addressBaseRec->getBaseAddress();
          version = versionGetter->getCurrentStrongholdVersion();

          if (requestAddresses(versionGetter->returnUsedAddresses(), *versionGetter))
          {
            initialized = true;
            LOG(INFO) << "AddressResolver initialized.";
          }
        }
      }
    }

    if (!initialized)
    {
      LOG(WARNING) << "AddressResolver was not initialized.";
    }

    return initialized;
  }

  template <typename T>
  const T* AddressResolver::getAddressPointer(const Address memAddr, const ModBase &requestingMod)
  {
    if (auto& modAddrIt = approvedAddresses.find(requestingMod.getModType() != approvedAddresses.end())
    {
      
      if ((modAddrIt->second).find(memAddr) != (modAddrIt->second).end())
      {
        return reinterpret_cast<T*>(addressBase + getAddress(memAddr));
      }
    }

    throw std::exception(("The address of type with id '" + std::to_string(static_cast<int>(memAddr))
                          + "' was not approved for mod with id '" + std::to_string(static_cast<int>(requestingMod.getModType())) + "'.").c_str());
  }

  const bool AddressResolver::requestAddresses(const std::vector<AddressRequest> &addrReq, const ModBase &requestingMod)
  {
    // TODO

    return false;
  }
}