#ifndef KEYBOARDINTERCEPTOR
#define KEYBOARDINTERCEPTOR

#include "modBase.h"
#include "enumheaders/keyboardEnumsAndUtil.h"

namespace modclasses
{
  class KeyboardInterceptor : public ModBase
  {
  private:

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
    std::vector<KAction> actionContainer{};

    // keep added modifiers (including NONE) for single keys and the status
    // will be run over and checked if changeable/assignable key pressed
    std::map<VK, bool> modifierStatus{ {VK::NONE, true} };  // none will always be true

    // 3 level tree structure, lookup might be a little slow for single keys, but should be fast enough for key presses
    // order -> assinable key -> first modifier -> second modifier
    // null will lead to next null:
    // - no key -> ignored
    // - no fitting modfier -> use single use mod (decend two levels and execute)
    // - no second modfier -> use one modifer mod (decend one level and execute)
    // NOTE: the check order for the modifiers will be the natural order of their values, left to right, shift/control/alt
    std::unordered_map<VK, std::unordered_map<VK, std::unordered_map<VK, KAction*>>> keyboardAction;

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

    // needs function to add keyboard strokes to use

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    KeyboardInterceptor(const KeyboardInterceptor &base) = delete;
    virtual KeyboardInterceptor& operator=(const KeyboardInterceptor &base) final = delete;

  private:

    LRESULT CALLBACK keyIntercepter(_In_ int code,_In_ WPARAM wParam, _In_ LPARAM lParam);

    static LRESULT CALLBACK hookForKeyInterceptor(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam);
  };
}

#endif //!KEYBOARDINTERCEPTOR