
#include "addressResolver.h"

namespace modclasses
{
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

  template <typename T>
  const T* AddressResolver::getAddressPointer(const Address memAddr, const ModBase &requestingMod)
  {
    // TODO: check if mod has registred access to the address

    return reinterpret_cast<T*>(addressBase + getAddress(memAddr));
  }
}