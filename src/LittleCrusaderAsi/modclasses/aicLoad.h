#ifndef AICLOAD
#define AICLOAD

#include <memory>

#include "IntSafe.h" // for DWORD
#include "modBase.h"

// all enums
#include "enumheaders/aiAndPersonalityEnums.h"

// needed mods:
#include "addressResolver.h"
#include "keyboardInterceptor.h"

namespace modclasses
{

  // NOTE: current approach expects 16 AIs, so Versions less then 1.3 (or 1.31 ?) will need additional handling
  class AICLoad : public ModBase
  {
  private:
    
    // will point to the loaded aics in memory, the access will be the following: AI * personalityField
    int32_t (*aicMemoryPtr)[2704]{ 0x0 };

    // stores a copy of the original values for quick swaps
    std::array<int32_t, 2704> vanillaAIC;

    // here will be the position for the loaded aic data, will see how it will be

    // needed to give the address resolver the right infos
    // can be static, I don't assume changes
    static std::vector<AddressRequest> usedAddresses;

  public:

    // get type of this mod
    ModType getModType() const override
    {
      return ModType::AIC_LOAD;
    }

    std::vector<ModType> getDependencies() const override;

    bool initialize() override;

    // if this works, will be used to get the loaded aic data
    // this is not reliable und should be replaced by some event system
    void firstThreadAttachAfterModAttachEvent() override;

    /**con- and destructor**/

    AICLoad(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config);

    /**additional functions for others**/

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    AICLoad(const AICLoad &base) = delete;
    virtual AICLoad& operator=(const AICLoad &base) final = delete;
  };
}

#endif //AICLOAD