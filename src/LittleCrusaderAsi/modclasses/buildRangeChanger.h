#ifndef BUILDRANGECHANGER
#define BUILDRANGECHANGER

#include "modBase.h"
#include "addressResolver.h"
#include "keyboardInterceptor.h"
#include "bltOverlay.h"


namespace modclasses
{
  enum class BuildRange
  {
    RANGE_160 = 0,
    RANGE_200 = 1,
    RANGE_300 = 2,
    RANGE_400 = 3,
    NONE = 99
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

    // storing range status
    struct BuildRangeData
    {
      int32_t* address{ nullptr };
      int32_t defaultValue{ 0 };
      int32_t currentCustomValue{ 0 };
      bool fileValueProvided{ false };
      int32_t fileValue{ 0 };
    };

    // will contain the data -> access happens using the enum values
    // 4 map sizes -> 4 values
    std::array<BuildRangeData, 4> buildRanges{};

    // needed to give the address resolver the right infos
    // can be static, I don't assume changes
    static std::vector<AddressRequest> usedAddresses;

  public:

    // declare public -> request mod registration and receive id (or nullptr)
    inline static ModIDKeeper ID{
      ModMan::RegisterMod("buildRangeChanger", [](const Json& config)
      {
        return std::static_pointer_cast<ModBase>(std::make_shared<BuildRangeChanger>(config));
      })
    };
    
    ModID getModID() const override
    {
      return ID;
    }

    std::vector<ModID> getDependencies() const override;

    /**con- and destructor**/
    BuildRangeChanger(const Json &config);

    /**additional functions for others**/

    // sets the custom(!) value
    // also applies it, if range changer switched ON
    void setBuildRange(BuildRange range, int32_t value);

    // will set the status to the requested, if not already set
    void setRangeChangeStatus(bool active);

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    BuildRangeChanger(const BuildRangeChanger &base) = delete;
    virtual BuildRangeChanger& operator=(const BuildRangeChanger &base) final = delete;

  private:

    /**keyboard functions**/

    // NOTE: change of this will appear in the log, but will not reflect in the menu if it is open
    void switchRangeChange(const HWND window, const bool keyUp, const bool repeat);

    /****/

    void initialize() override;

    // will reset the custom values, but does not apply
    // if toFileValue is true, will use the config values if provided
    void resetValues(bool toFileValues);

    // sets values custom if custom "true" or default if "false"
    // will update isChange based on that
    void applyValues(bool custom);

    void createMenu();
  };
}

#endif //BUILDRANGECHANGER
