
#include <algorithm>

#include "aicLoad.h"

namespace modclasses
{
  AICLoad::AICLoad(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
    // will read config in the future
  };

  // will need address and keyboard stuff
  std::vector<ModType> AICLoad::getDependencies() const
  {
    return { ModType::ADDRESS_RESOLVER, ModType::KEYBOARD_INTERCEPTOR };
  }

  bool AICLoad::initialize()
  {
    auto addressResolver = getMod<AddressResolver>();
    auto keyInterceptor = getMod<KeyboardInterceptor>();

    if (addressResolver && keyInterceptor)
    {
      // request addresses
      if (addressResolver->requestAddresses(usedAddresses, *this))
      {
        aicMemoryPtr = addressResolver->getAddressPointer<int32_t[2704]>(Address::AIC_IN_MEMORY, *this);

        initialized = true;
      }
    }

    if (!initialized)
    {
      LOG(WARNING) << "AICLoad was not initialized.";
    }
    else
    {
      LOG(INFO) << "AICLoad initialized.";
    }

    return initialized;
  }

  void AICLoad::firstThreadAttachAfterModAttachEvent()
  {
    if (initialized)
    {
      // save vanilla values
      std::copy(std::begin(*aicMemoryPtr), std::end(*aicMemoryPtr), vanillaAIC.begin());

      if (vanillaAIC.at(0) != 12)
      {
        LOG(WARNING) << "First thread attached event was seemingly triggerd before AIC initialisation. AICLoad features are likely unreliable."
          << "Indicator: first int is not a 12." ;
      }


      // if it works, load data here
      // in theory, it should not cause direct harm -> if this fires to early, stronghold will place its own values over them

      LOG(INFO) << "AICLoad run through 'first thread attached'-event.";
    }
  }

  std::vector<AddressRequest> AICLoad::usedAddresses{
    {Address::AIC_IN_MEMORY, {{Version::NONE, 10816}}, true, AddressRisk::WARNING}
  };
}