#ifndef KEYBOARDINTERCEPTOR
#define KEYBOARDINTERCEPTOR

#include "modBase.h"
#include "enumheaders/keyboardEnums.h"

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
    // maybe two maps, one for change keys, one for functions?
    // other question would be -> enums for key structures?
    // also -> key like alt, crtl as state changer, or as full combination key?
    // also -> key up/down different calls? -> gues not? or for later?

    // map to remember modification keys and status, or set to insert and remove (an array might be more performand, hm?)
    // map to remember key mappings
    // (key -> own struct -> check if modifiers fullfilled -> sorted list, 2 modifiers to non)
    // if some fits, do something -> important to prepare structure during config load

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