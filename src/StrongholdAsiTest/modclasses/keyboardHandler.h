#ifndef KEYBOARDHANDLER
#define KEYBOARDHANDLER

#include "modBase.h"

namespace modclasses
{
  class KeyboardHandler : public ModBase
  {
  private:

    // keyboard function hook
    HHOOK keyboardHook{nullptr};
    
    // stronghold window
    HWND window{ nullptr };

    // the basis should be, that one registers a function, or another key, for a key or a combination
    // maybe two maps, one for change keys, one for functions?
    // other question would be -> enums for key structures?
    // also -> key like alt, crtl as state changer, or as full combination key?
    // also -> key up/down different calls? -> gues not? or for later?

  public:
    
    ModType getModType() const override
    {
      return ModType::KEYBOARD_HANDLER;
    }

    std::vector<ModType> getDependencies() const override
    {
      return {};
    }

    // no dep
    void giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep) override {};
    bool initialize() override;

    /**con- and destructor**/
    KeyboardHandler(const Json &config);  // needs to load keyboard reconfig -> also reject requests in order if multi?

    ~KeyboardHandler();

    /**additional functions for others**/

    // needs function to add keyboard strokes to use

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    KeyboardHandler(const KeyboardHandler &base) = delete;
    virtual KeyboardHandler& operator=(const KeyboardHandler &base) final = delete;

  private:

    LRESULT CALLBACK keyIntercepter(_In_ int code,_In_ WPARAM wParam, _In_ LPARAM lParam);

    static LRESULT CALLBACK hookForKeyInterceptor(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam);
  };
}

#endif //!KEYBOARDHANDLER