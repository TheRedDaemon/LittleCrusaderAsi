
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

  /*******************************************************/
  // Non-static part starts here:
  /*******************************************************/

  KeyboardInterceptor::KeyboardInterceptor(const Json &config)
  {
    // TODO? maybe add try catch? (maybe also to other config resolves? might otherwise break the system to easy?)
    // TODO: more Tests!

    // load key config and prepare, doing even the add here
    auto confIt = config.find("activationKey");
    if (confIt != config.end())
    {
      VK possibleActivationKey{ confIt.value().get<VK>() };

      if (isActivationKey(possibleActivationKey))
      {
        activationKey = possibleActivationKey;
      }
    }

    confIt = config.find("keyRedefines");
    if (confIt != config.end())
    {
      std::array<bool, 3> allowedKeyComb{ true, false, false };

      for (auto& keyKombination : confIt.value().items())
      {
        // don't know if easier way to parse from string back to enum
        Json key;
        key["key"] = keyKombination.key();
        VK toModify{ key["key"].get<VK>() };

        if (isChangeableKey(toModify))
        {
          auto& currentKeyCombis = keyKombination.value();
          std::vector<std::array<VK, 3>> vkCombinations{ resolveKeyConfig(currentKeyCombis) };

          std::unique_ptr<KAction> newChange = std::make_unique<KChange>(toModify);
          bool onePlaced{ false };

          for (const auto& combi : vkCombinations)
          {
            bool success{ registerKey(combi[0], combi[1], combi[2], newChange.get(), allowedKeyComb) };
            if (!success)
            {
              // maybe more output which one? problem: I translate all, so I don't have the single key combi here
              LOG(WARNING) << "KeyboardInterceptor config: Failed adding one combination for key: " << keyKombination.key();
            }
            onePlaced = onePlaced || success;
          }

          if (onePlaced)
          {
            actionContainer.push_back(std::move(newChange));
          }
        }
        else
        {
          LOG(WARNING) << "KeyboardInterceptor config: No changeable key to replace: " << keyKombination.key();
        }
      }
    }
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


  const bool KeyboardInterceptor::registerKey(const VK modifierOne, const VK modifierTwo, const VK mainkey,
                                              KAction* actionPointer, std::array<bool, 3> &allowedKeyComb)
  {
    // check if allowed
    bool addKeys{true};
    int valid_keys{ 0 };

    addKeys = addKeys && isChangeableKey(mainkey);
    if (addKeys)
    {
      ++valid_keys;

      if (modifierOne != VK::NONE)
      {
        addKeys = addKeys && isModificationKey(modifierOne);
        if (addKeys)
        {
          ++valid_keys;

          if (modifierTwo != VK::NONE)
          {
            addKeys = addKeys && isModificationKey(modifierTwo);
            if (addKeys)
            {
              ++valid_keys;
            }
          }
        }
      }
    }

    // checks if mainkey and modifierOne are not NONE and valid keys, then if modifierTwo has a valid value
    if (addKeys && allowedKeyComb.at(valid_keys - 1))  // if main key invalid, add keys should be false, and so -1 should never be parsed
    {
      auto& lastLevel = keyboardAction[mainkey][modifierOne];
      if (lastLevel.find(modifierTwo) == lastLevel.end())
      {
        lastLevel[modifierTwo] = actionPointer;
        modifierStatus.try_emplace(modifierOne, false);
        modifierStatus.try_emplace(modifierTwo, false);
        return true;
      }
    }
    return false;
  }


  const std::vector<bool> KeyboardInterceptor::registerFunction(const std::function<void(const HWND, const bool, const bool)> &funcToExecute,
                                                                const std::vector<std::array<VK, 3>> &keyCombinations)
  {
    std::vector<bool> workingKeyCombinations(keyCombinations.size(), false);
    std::array<bool, 3> allowedKeyComb{false, true, true};

    if (funcToExecute)
    {
      std::unique_ptr<KAction> newFunc = std::make_unique<KFunction>(funcToExecute);
      bool onePlaced{ false };

      for (size_t i = 0; i < keyCombinations.size(); i++)
      {
        const std::array<VK, 3>& keys = keyCombinations.at(i);

        bool success = registerKey(keys.at(0), keys.at(1), keys.at(2), newFunc.get(), allowedKeyComb);
        onePlaced = onePlaced || success;
        workingKeyCombinations[i] = success;
      }

      if (onePlaced)
      {
        actionContainer.push_back(std::move(newFunc));
      }
    }
    else
    {
      LOG(WARNING) << "KeyboardIntercepter: No valid function for key function add provided.";
    }

    // will mostly stay silent, use return to throw or log
    // TODO: maybe some debug logs would be practical...?
    return workingKeyCombinations;
  }


  const std::vector<bool> KeyboardInterceptor::registerFunction(const std::function<void(const HWND, const bool, const bool)> &funcToExecute,
                                                                const Json &keyCombinations)
  {
    std::vector<std::array<VK, 3>> keyCombArray{ resolveKeyConfig(keyCombinations) };
    return registerFunction(funcToExecute, keyCombArray);
  }


  const bool KeyboardInterceptor::resolveKey(const VK key, const LPARAM lParam)
  {
    bool keyUp{ lParam & 0x80000000 ? true : false };
    bool keyHold{ lParam & 0x40000000 ? true : false };

    // for debug
    //char keyName[30];
    //GetKeyNameText(lParam, keyName, 30);
    //LOG(INFO) << "Key used: " << std::bitset<32>(lParam);
    //LOG(INFO) << "Key used: " << keyName << " " << key;

    // current approach doesn't allow to mix the keys -> every single one has to be either change or modifier keys

    // check switchkeys
    if (isActivationKey(key))
    {
      if (key == activationKey && !(keyUp || keyHold))
      {
        interceptorActive = !interceptorActive;
        return true;
      }
    }
    else if (isModificationKey(key))  // modifiers are always tracked
    {
      if (modifierStatus.find(key) != modifierStatus.end() && (keyUp || !keyHold))
      {
        modifierStatus[key] = !keyUp;
      }
    }
    else if (interceptorActive && isChangeableKey(key))
    {
      // TODO: TEST
      KAction* actionToPerform{ nullptr };

      // check if key even has modification
      // NOTE: pretty ugly, but at least returns fast, I guess...
      if (const auto& keyIt = keyboardAction.find(key); keyIt != keyboardAction.end())
      {

        // check first modifier
        auto modOneIt{ keyIt->second.begin() };
        const auto& modOneEnd{ keyIt->second.end() };
        while (!actionToPerform && modOneIt != modOneEnd)
        {
          if (modifierStatus[modOneIt->first])  // assuming already telled that it exits
          {

            // check second modifier
            auto modTwoIt{ modOneIt->second.begin() };
            const auto& modTwoEnd{ modOneIt->second.end() };
            while (!actionToPerform && modTwoIt != modTwoEnd)
            {
              // no shift + shift (NOTE: although, left + right will pass)
              // (also None has an extra check to allow NONE + NONE + Key stuff, every key change will actually fall in this category)
              if (modifierStatus[modTwoIt->first] && (modTwoIt->first == VK::NONE || modOneIt->first != modTwoIt->first))
              {
                actionToPerform = modTwoIt->second;
              }

              modTwoIt = std::next(modTwoIt);
            }
          }

          modOneIt = std::next(modOneIt);
        }
      }

      if (actionToPerform)
      {
        // check if other key action used, then change (also includes new "start" for this action
        if (const auto& currIt = currentlyUsedKeys.find(key); currIt != currentlyUsedKeys.end() && currIt->second != actionToPerform)
        {
          currIt->second->doAction(window, true, false);  // release other key

          if (!keyUp) // will only move action to other kombination if not key up
          {
            actionToPerform->doAction(window, false, false); // execute first other action
            currentlyUsedKeys[key] = actionToPerform;
          }
        }
        else
        {
          // execute if non other active and add to/remove from watch map
          actionToPerform->doAction(window, keyUp, keyHold);
          if (keyUp)
          {
            currentlyUsedKeys.erase(key);
          }
          else
          {
            currentlyUsedKeys[key] = actionToPerform;
          }
        }

        return true;
      }
    }

    return false;
  }


  // member function to handle it
  // Notes: if only returns 1, only code 3 (no remove) is delivered -> no keystrokes at all
  // test following -> it runs slow, only if a real keystroke is removed? -> true
  // trying the same stuff with noremove (and remove it) -> seem to be no slow down
  // NOTE: the current asumption is also, that the interceptor works sequential, if this it thread based, everything will fall into pieces
  // however, since debug this freezes stronghold... I guess it is? -> it seems to be resolved in the main thread
  LRESULT CALLBACK KeyboardInterceptor::keyIntercepter(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam)
  {
    bool stopKey{ false };

    if (code < 0)
    {
      return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    switch (code)
    {
      // if message removed in case of HC_Action -> slow if many
      // in case of NO_REMOVE: not
      // I really wonder why? maybe the peek is actually for this and I intercept it twice, because it runs in the same thread?
      // maybe because it is no further processed?
      case HC_NOREMOVE: // HC_ACTION:
      {
        if (!window)
        {
          window = GetActiveWindow();
        }

        // for filtering: (but remember, filtering here might create problems with the way stronghold handles keys, so careful
        // https://stackoverflow.com/questions/40599162/vsto-windows-hook-keydown-event-called-10-times
        // source: Game Hacking: Developing Autonomous Bots for Online Games -> Google Books

        if (window) // two tests to make sure it only executes if GetActiveWindow() found something
        {
          stopKey = resolveKey(generalToExtendedKey(wParam, lParam), lParam);
        }

        break;
      }

      default:
        break;
    }

    return stopKey ? 1 : CallNextHookEx(nullptr, code, wParam, lParam);
  }


  const VK KeyboardInterceptor::generalToExtendedKey(const WPARAM wParam, const LPARAM lParam)
  {
    bool keyExtended{ lParam & 0x01000000 ? true : false }; // extended keys are also the right hand shift and alt keys etc.

    switch (wParam)
    {
      case VK::ALT:
        return keyExtended ? VK::RIGHT_MENU : VK::LEFT_MENU;
      case VK::SHIFT:
        return keyExtended ? VK::RIGHT_SHIFT : VK::LEFT_SHIFT;
      case VK::CONTROL:
        return keyExtended ? VK::RIGHT_CONTROL : VK::LEFT_CONTROL;
      default:
        return static_cast<VK>(wParam); // overflow possible? -> likely no support for bigger utf keys, hmm?
    }
  }


  const std::vector<std::array<VK, 3>> KeyboardInterceptor::resolveKeyConfig(const Json &keyCombinations)
  {
    std::vector<std::array<VK, 3>> kombinations{};

    try
    {
      for (auto& keyKombination : keyCombinations.items())
      {
        auto& currentKombi = keyKombination.value();
        std::array<VK, 3> keys{ VK::NONE, VK::NONE, VK::NONE };
        bool ableToUse{ true };
        size_t keyCount{ currentKombi.size() };

        if (keyCount > 0 && keyCount < 4)
        {
          // get change/assign key
          keys[2] = currentKombi[keyCount - 1].get<VK>();
          ableToUse = ableToUse && keys[2] != VK::NONE;

          // get modfier
          if (keyCount > 1)
          {
            keys[0] = currentKombi[0].get<VK>();
            ableToUse = ableToUse && keys[0] != VK::NONE;

            // get modfier two
            if (keyCount > 2)
            {
              keys[1] = currentKombi[1].get<VK>();
              ableToUse = ableToUse && keys[1] != VK::NONE;
            }
          }
        }
        else
        {
          ableToUse = false;
        }

        if (ableToUse)
        {
          kombinations.push_back(keys);
        }
        else
        {
          kombinations.push_back({ VK::NONE, VK::NONE, VK::NONE });
          LOG(WARNING) << "Unable to parse key kombination: " << currentKombi;
        }
      }
    }
    catch (const std::exception& o_O)
    {
      LOG(ERROR) << "Error during key config resolve: " << o_O.what();
    }

    return kombinations;
  }
}
// (more sources for input handling (not this) https://stackoverflow.com/a/19802769)
// NOTE: Event Handler would be practical -> knowing when the game runs is crucial, only then one can switch between behaviours without input