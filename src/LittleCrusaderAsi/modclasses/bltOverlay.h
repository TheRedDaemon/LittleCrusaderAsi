#ifndef BLTOVERLAY
#define BLTOVERLAY

#include "modBase.h"

#include "keyboardInterceptor.h"
#include "addressResolver.h"

namespace modclasses
{

  // simple proto class, replace all 'ProtoMod' with the new name
  class BltOverlay : public ModBase
  {
  private:

    // needed to give the address resolver the right infos
    // can be static, I don't assume changes
    static std::vector<AddressRequest> usedAddresses;

  public:

    ModType getModType() const override
    {
      return ModType::BLT_OVERLAY;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::KEYBOARD_INTERCEPTOR, ModType::ADDRESS_RESOLVER };
    }

    void cleanUp() override;

    /**con- and destructor**/
    BltOverlay(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config); // default cons, does nothing though

    /**additional functions for others**/

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    BltOverlay(const BltOverlay &base) = delete;
    virtual BltOverlay& operator=(const BltOverlay &base) final = delete;

  private:

    // should be defined in .cpp
    void initialize() override;
  };
}

#endif //BLTOVERLAY
