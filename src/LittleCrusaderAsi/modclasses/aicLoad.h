#ifndef AICLOAD
#define AICLOAD

#include <memory>

#include "IntSafe.h" // for DWORD
#include "modBase.h"

// all enums
//#include "enumheaders/aiAndPersonalityEnums.h"
#include "enumheaders/pseudoEnumsAI.h"

// needed mods:
#include "addressResolver.h"
#include "keyboardInterceptor.h"
#include "bltOverlay.h"

namespace modclasses
{

  // NOTE: current approach expects 16 AIs, so Versions less then 1.3 (or 1.31 ?) will need additional handling
  class AICLoad : public ModBase
  {

  private:
    
    // will point to the loaded aics in memory
    int32_t (*aicMemoryPtr)[2704]{ 0x0 };

    // stores a copy of the original values for quick swaps
    std::array<int32_t, 2704> vanillaAIC{};

    // config data safe:
    
    // start state
    bool isChanged{ false };

    // store keyboard shortcuts
    Json keysActivate;
    Json keysReloadMain;
    Json keysReloadAll;

    // folder of the mod relativ to stronghold
    std::string relModPath;
    // name of the folder
    std::string aicFolder;

    // stores the to be loaded aic-file names
    // the first is the main, the values of all following will only be used if main has no value for them, and so on
    std::vector<std::string> loadList{};

    // store values in unordered map<string, std::unique_ptr<unordered map<int_id, value>>>
    std::unordered_map<std::string, std::unique_ptr<std::unordered_map<int32_t, int32_t>>> loadedAICValues;
    // TODO(?): maybe it would be a idea to store the summed values of everything but the main aic?

    // "include in aic save"-array
    // 0 is unused -> will use the AIName value to check
    std::array<bool, 17> saveArray{};
    std::string lastSaveName{ "" }; // remembered to confirm overwrite

    // needed to give the address resolver the right infos
    // can be static, I don't assume changes
    static std::vector<AddressRequest> usedAddresses;

  public:

    // declare public -> request mod registration and receive id (or nullptr)
    inline static ModIDKeeper ID{
      ModMan::RegisterMod("aicLoad", [](const Json& config)
      {
        return std::static_pointer_cast<ModBase>(std::make_shared<AICLoad>(config));
      })
    };

    // get type of this mod
    ModID getModID() const override
    {
      return ID;
    }

    std::vector<ModID> getDependencies() const override;

    /**con- and destructor**/

    AICLoad(const Json &config);

    /**additional functions for others**/

    // needs ai-char, field and a fitting value; simply provide the later as value/string/enum, JSON will take care of the transform
    // if no issue, sets the field to the requested value (NOTE -> for some fields, using the "wrong" enum (ex. Unit, not DiggingUnit) might yield working results)
    // current AICLoad settings, like if AICs are applied or deactivated, are ignored and not changed
    // returns bool indicating whether the function was successful
    // NOTE: "false" likely means an invalid value, ai-char or field were NONE, or the mod was not initialized
    const bool setPersonalityValue(AINameEnum aiName, AICEnum field, const Json value);

    // basically a raw value edit with (a little?) less overhead then setPersonalityValue
    // tests only if the ai-char or field are NONE, or if the mod was not initialized
    // the use of static casted enums might make this a bit safer, but only if the right field is chosen
    // returns bool indicating whether the function was successful
    const bool setPersonalityValueUnchecked(AINameEnum aiName, AICEnum field, const int32_t value);

    // name should be the direct file name, fileRelativeToMod should be true if the function is executed by initialization code
    // false for calls using the keyboard interceptor (and maybe one day an event handler?), they are executed by stronghold directly
    // making the file system is relative to the exe
    // returns empty pointer in unrecoverable error case
    std::unique_ptr<std::unordered_map<int32_t, int32_t>> loadAICFile(const std::string &name, const bool fileRelativeToMod) const;

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    AICLoad(const AICLoad &base) = delete;
    virtual AICLoad& operator=(const AICLoad &base) final = delete;

  private:

    /**keyboard functions**/

    void activateAICs(const HWND window, const bool keyUp, const bool repeat);

    void reloadMainAIC(const HWND window, const bool keyUp, const bool repeat);

    void reloadAllAIC(const HWND window, const bool keyUp, const bool repeat);

    /****/

    void initialize() override;

    // this will be given to an event executor (currently BltOverlay, maybe one day EventHandler)
    // should be triggered after the AICvalues are loaded
    void initialAICLoad();

    // returns empty pointer in unrecoverable error case
    // if size_t == 0, value ignored
    std::unique_ptr<std::unordered_map<int32_t, int32_t>> loadAICFile(const std::string &name,
      const bool fileRelativeToMod, const size_t mapInitSize) const;
    void applyAICs();

    // returns if the value is valid, at the same time it fills an int32_t with the right value
    // this is necessary for some fields that use enum values
    // the int32_t will be unchanged, if the value is not valid
    const bool isValidPersonalityValue(AICEnum field, const Json &value, int32_t &validValue) const;
    const int32_t getAICFieldIndex(AINameEnum aiName, AICEnum field) const;

    // helper function
    // -> sorts AIC value types based on range and if they have special input
    //    types, represented by pseudo enums
    // -> no special meaning, only internally relevant
    // returns intPtr to static structure, should be ok
    const int32_t* getReactionNumber(AICEnum field) const;

    // menu creation

    void createMenu();

    template<typename T>
    void createEnumMenuHelper(AINameEnum aiName, AICEnum field, MenuBase& charMenu);

    // save current AIC status in file
    std::string saveAIC(const std::string& fileName, const bool fileRelativeToMod);

    // to safe space in the menu definition
    template<typename PseudoEnumClass>
    std::string getNameOrNULL(int32_t value)
    {
      const std::string* const ref{ PseudoEnumClass::GetName(value) };
      return ref ? *ref : PseudoEnumClass::None->getName();
    }
  };
}

#endif //AICLOAD