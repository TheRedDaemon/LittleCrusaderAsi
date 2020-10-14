#ifndef BUILDRANGECHANGER
#define BUILDRANGECHANGER

#include "modBase.h"
#include "addressResolver.h"
#include "keyboardInterceptor.h"

namespace modclasses
{
  enum class BuildRange
  {
    NONE,
    RANGE_160,
    RANGE_200,
    RANGE_300,
    RANGE_400
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(BuildRange, {
    {BuildRange::NONE, nullptr},
    {BuildRange::RANGE_160, "160"},
    {BuildRange::RANGE_200, "200"},
    {BuildRange::RANGE_300, "300"},
    {BuildRange::RANGE_400, "400"}
  })

  class BuildRangeChanger : public ModBase
  {
  private:

    bool isChanged{ false };

    // stored so shortcut can be added after everything else works
    Json keyboardShortcut;

    std::unordered_map<BuildRange, int32_t*> castleRanges;
    std::unordered_map<BuildRange, std::pair<int32_t, int32_t>> defaultAndNewValues;

    // needed to give the address resolver the right infos
    // can be static, I don't assume changes
    static std::vector<AddressRequest> usedAddresses;

  public:
    
    ModType getModType() const override
    {
      return ModType::BUILD_RANGE_CHANGER;
    }

    std::vector<ModType> getDependencies() const override;

    bool initialize() override;

    /**con- and destructor**/
    BuildRangeChanger(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config);

    /**additional functions for others**/

    void switchRangeChange(const HWND window, const bool keyUp, const bool repeat);

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    BuildRangeChanger(const BuildRangeChanger &base) = delete;
    virtual BuildRangeChanger& operator=(const BuildRangeChanger &base) final = delete;
  };
}

#endif //BUILDRANGECHANGER
