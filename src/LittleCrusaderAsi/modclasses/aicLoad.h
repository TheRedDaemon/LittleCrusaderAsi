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
    using AIC = AIPersonalityFieldsEnum;

  private:
    
    // will point to the loaded aics in memory, the access will be the following: AI * personalityField
    int32_t (*aicMemoryPtr)[2704]{ 0x0 };

    // stores a copy of the original values for quick swaps
    std::array<int32_t, 2704> vanillaAIC;

    // config data safe:
    
    // start state
    bool isChanged{ false };

    // store keyboard shortcuts
    Json keysActivate;
    Json keysReloadMain;
    Json keysReloadAll;

    // name of the folder
    std::string aicFolder;

    // stores the to be loaded aic-file names
    // the first is the main, the values of all following will only be used if main has no value for them, and so on
    std::vector<std::string> loadList{};

    // current idea: store values in unordered map<int_id, unordered map<string, value>>
    // so, everything is stored and can be reconstructed
    std::unordered_map<int32_t, std::unordered_map<std::string, int32_t>> loadedAICValues;

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

  private:

    void loadAICFile(const std::string name);

    // returns if the value is valid, at the same time it fills an int32_t with the right value
    // this is necessary for some fields that use enum values
    // the int32_t will be unchanged, if the value is not valid
    const bool isValidPersonalityValue(const AIC field, const Json &value, int32_t &validValue);
    const int32_t getAICFieldIndex(const AICharacterName aiName, const AIC field) const;
  };
}

#endif //AICLOAD