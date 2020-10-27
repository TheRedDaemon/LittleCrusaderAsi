#ifndef BLTOVERLAY
#define BLTOVERLAY

#include "modBase.h"

namespace modclasses
{

  // simple proto class, replace all 'ProtoMod' with the new name
  class BltOverlay : public ModBase
  {

  public:

    ModType getModType() const override
    {
      return ModType::BLT_OVERLAY;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::KEYBOARD_INTERCEPTOR };
    }

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
