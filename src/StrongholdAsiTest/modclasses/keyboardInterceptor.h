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

    // keep added modifiers (including NONE) for single keys and the status
    // will be run over and checked if changeable/assignable key pressed
    // uses reverse order so that NONE is checked last
    std::map<VK, bool, std::greater<unsigned char>> modifierStatus{ {VK::NONE, true} };  // none will always be true

    // 3 level tree structure, lookup might be a little slow for single keys, but should be fast enough for key presses
    // order -> assinable key -> first modifier -> second modifier
    // null will lead to next null:
    // - no key -> ignored
    // - no fitting modfier -> use single use mod (decend two levels and execute)
    // - no second modfier -> use one modifer mod (decend one level and execute)
    // NOTE: the check order for the modifiers will be the natural order of their values, left to right, shift/control/alt
    std::unordered_map<VK, std::unordered_map<VK, std::unordered_map<VK, KAction*>>> keyboardAction{};

    // keeps track of used assign/changeable keys, to get if a modifier key was lifted
    std::unordered_map<VK, KAction*> currentlyUsedKeys{};

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
    // NOTE: multiple calls to this should work, but there is currently no additional check, so every call to this will create an additional FunctionKey action
    // so it is prefered all key combinations are requested together (different funct, require different calls of cause)
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

    LRESULT CALLBACK keyIntercepter(_In_ int code,_In_ WPARAM wParam, _In_ LPARAM lParam);

    static LRESULT CALLBACK hookForKeyInterceptor(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam);
  };
}

#endif //!KEYBOARDINTERCEPTOR