
#include "keyboardInterceptor.h"

namespace modclasses
{

  // pointer for static function
  static KeyboardInterceptor *handlerPointer{ nullptr };

  // static hook function
  LRESULT CALLBACK KeyboardInterceptor::hookForKeyInterceptor(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam)
  {
    return handlerPointer->keyIntercepter(code, wParam, lParam);
  }

  // Non-static part starts here:

  KeyboardInterceptor::KeyboardInterceptor(const Json &config)
  {
    // load key config and prepare
  }

  KeyboardInterceptor::~KeyboardInterceptor()
  {
    if (keyboardHook)
    {
      // don't know if concurrent stuff could create issues here?
      BOOL success{ UnhookWindowsHookEx(keyboardHook) };

      if (!success)
      {
        LOG(ERROR) << "Failed to unhook keyboard handler. Error code: " << GetLastError();
      }
    }
  }

  bool KeyboardInterceptor::initialize()
  {
    if (!handlerPointer)
    {
      handlerPointer = this;

      keyboardHook = SetWindowsHookEx(WH_KEYBOARD, hookForKeyInterceptor, 0, GetCurrentThreadId());

      // there might be far more to do here

      if (keyboardHook)
      {
        initialized = true;
        LOG(INFO) << "KeyboardInterceptor initialized.";
      }
      else
      {
        LOG(ERROR) << "KeyboardInterceptor was unable to place keyboard hook.";
      }
    }
    else
    {
      LOG(ERROR) << "KeyboardInterceptor needs to place a windows hook and uses a singleton. Creating another KeyboardInterceptor is not allowed.";
    }

    if (!initialized)
    {
      LOG(WARNING) << "KeyboardInterceptor was not initialized.";
    }

    return initialized;
  }

  // member function to handle it
  LRESULT CALLBACK KeyboardInterceptor::keyIntercepter(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam)
  {
    bool stopKey{ false };

    if (code < 0)
    {
      return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    switch (code)
    {
      // if message removed in case of action -> slow if many
      // in case of NO_REMOVE: not
      // I really wonder why? maybe the peek is actually for this and I intercept it twice, because it runs in the same thread?
      // maybe because it is no further processed?
      case HC_NOREMOVE: // HC_ACTION:
      {
        bool keyUp{ lParam & 0x80000000 ? true : false };
        bool keyHold{ lParam & 0x40000000 ? true : false };

        if (!window)
        {
          window = GetActiveWindow();
        }

        // todo: need a much better structure to handle input and input variants

        // activation key uses only press
        if (wParam == activationKey && !(keyUp || keyHold))
        {
          interceptorActive = !interceptorActive;
          stopKey = true;
        }
        else if (interceptorActive)
        {
          if (window)
          {
            // for filtering: (but remember, filtering here might create problems with the way stronghold handles keys, so careful
            // https://stackoverflow.com/questions/40599162/vsto-windows-hook-keydown-event-called-10-times
            // source: Game Hacking: Developing Autonomous Bots for Online Games -> Google Books

            // for debug
            //char keyName[15];
            //GetKeyNameText(lParam, keyName, 15);
            //LOG(INFO) << "Key used: " << keyName;
            //LOG(INFO) << "Key used: " << std::bitset<32>(lParam);
            LOG(INFO) << "Key used: " << wParam;

            // experiment left, right
            if (lParam & 0x01000000 ? true : false)
            {
              LOG(INFO) << "right";
            }
            else
            {
              LOG(INFO) << "left";
            }


            // SendMessage seems to work
            if (wParam == 0x41)
            {
              //sendKey(window, keyUp, keyHold, VK_LEFT, 'a');
              stopKey = true;
            }

            if (wParam == VK_LEFT)
            {
              //sendKey(window, keyUp, keyHold, 0x41, 0);
              stopKey = true;
            }
          }
        }

        break;
      }

      default:
        break;
    }

    return stopKey ? 1 : CallNextHookEx(nullptr, code, wParam, lParam);
  }
}
// (source for input handling (not this) https://stackoverflow.com/a/19802769)

// Notes: if only returns 1, only code 3 (no remove) is delivered -> no keystrokes at all
// test following -> it runs slow, only if a real keystroke is removed? -> true
// trying the same stuff with noremove (and remove it) -> seem to be no slow down

// NOTE: Event Handler has precedence -> knowing when the game runs is crucial, only then one can switch between behaviours....