#ifndef KEYBOARDINTERCEPTOR
#define KEYBOARDINTERCEPTOR

#include "modBase.h"
#include "enumheaders/keyboardEnumsAndUtil.h"

namespace modclasses
{
  class KeyboardInterceptor : public ModBase
  {
  private:

    // NOTE:
    // since the intern keys aren't changed and the modifiers are passed through, there are issues regarding completly free definition
    // therefore (until a better approach is developed), the following restrictions are set:
    // 1. key changes can only happen to indididual keys -> they carry the modifiers with them
    // 2. modifier + key is only allowed for functions -> this one still needs caution, since one could use for ex. control + A for a function,
    // which would disallow to quick access the armory menu without jumping

    // key to switch inteceptor on
    VK activationKey{ VK::HOME };

    // keyboard function hook
    HHOOK keyboardHook{nullptr};
    
    // stronghold window
    HWND window{ nullptr };

    // if the keyboard interceptor is active
    bool interceptorActive{ false };

    // the basis should be, that one registers a function, or another key, for a key or a combination
    // if some fits, do something -> important to prepare structure during config load

    // will hold all actions, the other map will only use the pointers
    std::vector<std::unique_ptr<KAction>> actionContainer{};

    // keep added modifiers for single keys and the status
    // also NONE (for "no modifier here"), which will always be true (ex. a key without modifiers but a change will have two NONE as modifiers)
    std::unordered_map<VK, bool> modifierStatus{ {VK::NONE, true} };

    // 3 level tree structure, lookup might be a little slow for single keys, but should be fast enough for key presses
    // order -> assinable key -> first modifier -> second modifier
    // null will lead to next null:
    // - no key -> ignored
    // - no fitting modfier -> use single use mod (decend two levels and execute)
    // - no second modfier -> use one modifer mod (decend one level and execute)
    // NOTE: the check order for the modifiers will be the reverse natural order of their values, right to left, alt/control/shift
    // for the modifiers it will be checked in reverse natural order if one of them is active and uses reverse order so that NONE is checked last
    std::unordered_map<VK, std::map<VK, std::map<VK, KAction*, std::greater<unsigned char>>, std::greater<unsigned char>>> keyboardAction{};

    // keeps track of used assign/changeable keys, to allow registration if a modifier key was lifted
    // note, this will only change the behaviour of combinations with keys that fire multiple times
    // changes with keys that only show down and up will won't be noticed
    std::unordered_map<VK, KAction*> currentlyUsedKeys{};


    // (much) later TODO?: if might be easier for the resolve to create a map of [3] arrays or structs
    // but if more then two modifiers pushed... how to choose which to use first? and which will be first?
  public:
    
    ModType getModType() const override
    {
      return ModType::KEYBOARD_INTERCEPTOR;
    }

    std::vector<ModType> getDependencies() const override
    {
      return {};
    }

    // no dep
    void giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep) override {};
    bool initialize() override;

    /**con- and destructor**/
    KeyboardInterceptor(const Json &config);  // needs to load keyboard reconfig -> also reject requests in order if multi?

    ~KeyboardInterceptor();

    /**additional functions for others**/

    // will try to register the function to the reqested keys, return vector of bool with success result for every combination in order
    // note, that for this (currently) at least modifier one is needed, to add just modifierOn + key, send VK::NONE for modifierTwo
    // the key array need the combinations in order: modifierOne, modifierTwo, key
    // NOTE: multiple calls to this could work, but there is currently no additional check, so every call to this will create an additional FunctionKey action
    // so it is prefered all key combinations are requested together (different functions require different calls to this, of course)
    // also, multiple key combinations result in the ability to "open" multiple calls (multiple key downs before the first lift)
    const std::vector<bool> registerFunction(const std::function<void(const HWND, const bool, const bool)> &funcToExecute,
                                             const std::vector<std::array<VK, 3>> &keyCombinations);

    // needs function to add keyboard strokes to use

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    KeyboardInterceptor(const KeyboardInterceptor &base) = delete;
    virtual KeyboardInterceptor& operator=(const KeyboardInterceptor &base) final = delete;

  private:

    // returns true if key successfully registered
    const bool registerKey(const VK modifierOne, const VK modifierTwo, const VK mainkey, KAction* actionPointer);

    // used to resolve general to extended keys if key delivers only general
    const VK generalToExtendedKey(const WPARAM wParam, const LPARAM lParam);

    // resolves key, also return !true! if key should be !stopped!
    const bool resolveKey(const VK wParam, const LPARAM lParam);

    LRESULT CALLBACK keyIntercepter(_In_ int code,_In_ WPARAM wParam, _In_ LPARAM lParam);

    static LRESULT CALLBACK hookForKeyInterceptor(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam);
  };
}

#endif //!KEYBOARDINTERCEPTOR