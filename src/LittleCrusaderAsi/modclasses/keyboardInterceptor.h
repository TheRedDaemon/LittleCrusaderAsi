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
    HHOOK keyboardHook{ nullptr };

    // char message hook
    HHOOK charHook{ nullptr };
    // keeper for ref -> only one at the time is allowed
    // user of this need to know, that setting this will prevent other inputs until removed
    // if the third func is set, all keyboard inputs are delivered to this object, otherwise, all input will be stopped
    // unlike a KPassage, this third receiver needs to return a bool -> if true, the input is devoured
    std::tuple<ModBase*, std::function<void(char)>, std::function<bool(const HWND, const bool, const bool, const VK)>>
      charHandlerFunc{ nullptr, nullptr, nullptr };
    
    // stronghold window
    HWND window{ nullptr };

    // if the keyboard interceptor is active
    bool interceptorActive{ false };

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

    // declare public -> request mod registration and receive id (or nullptr)
    inline static ModIDKeeper ID{
      ModMan::RegisterMod("keyboardInterceptor", [](const Json& config)
      {
        return std::static_pointer_cast<ModBase>(std::make_shared<KeyboardInterceptor>(config));
      })
    };
    
    ModID getModID() const override
    {
      return ID;
    }

    std::vector<ModID> getDependencies() const override
    {
      return {};
    }

    /**con- and destructor**/
    KeyboardInterceptor(const Json &config);  // needs to load keyboard reconfig -> also reject requests in order if multi?

    ~KeyboardInterceptor();

    /**additional functions for others**/


    // will try to register the function to the reqested keys
    // return type is either bool (all worked) or vector of bool (for every combination) (template true -> simple bool return)
    // KeyComb is either a reference to a Json containing the keys, or directly a vector of std::array<VK, 3> (modifierTwo, modifierOne, changeableKey)
    // note, that for this is at least modifier one is needed, to add just modifierOn + key, send VK::NONE for modifierTwo
    // the key array need the combinations in order: modifierOne, modifierTwo, key
    // NOTE: multiple calls to this could work, but there is currently no additional check, so every call to this will create an additional FunctionKey action
    // so it is prefered all key combinations are requested together (different functions require different calls to this, of course)
    // also, multiple key combinations result in the ability to "open" multiple calls (multiple key downs before the first lift)
    template<bool simpleReturn, typename KeyComb>
    const auto registerFunction(const std::function<void(const HWND, const bool, const bool)> &funcToExecute,
                                KeyComb &keyCombinations)
    {
      std::array<bool, 3> allowedKeyComb{ false, true, true };
      return registerKeyComb<simpleReturn, false>(funcToExecute, keyCombinations, allowedKeyComb);
    }


    // will try to register the function to the reqested keys
    // return type is either bool (all worked) or vector of bool (for every combination) (template true -> simple bool return)
    // KeyComb is either a reference to a Json containing the keys, or directly a vector of std::array<VK, 3> (modifierTwo, modifierOne, changeableKey)
    // registerPassage derails only single changeable keys, since the modifier keys would not reach the function
    // it is then up to the single mod what to do with the keys
    // NOTE: multiple calls to this could work, but there is currently no additional check, so every call to this will create an additional PassageKey action
    // so it is prefered all key combinations are requested together (different functions require different calls to this, of course)
    // also, multiple key combinations result in the ability to "open" multiple calls (multiple key downs before the first lift)
    template<bool simpleReturn, typename KeyComb>
    const auto registerPassage(const std::function<void(const HWND, const bool, const bool, const VK)> &funcToExecute,
                               KeyComb &keyCombinations)
    {
      std::array<bool, 3> allowedKeyComb{ true, false, false }; // only single keys
      return registerKeyComb<simpleReturn, true>(funcToExecute, keyCombinations, allowedKeyComb);
    }

    // register the function to receive chars given by the keyboard
    // returns if registering was possible
    // only one mod can receive them at the time and also needs to free them
    // if a valid third func is given, this object will receive all input and can decide if it should reach
    // the char handler and the program -> the char handler will devour everything that creates chars
    // if the third in nullptr, all keys are passed to the char handler and the programm
    const bool lockChars(ModBase* mod, std::function<void(char)> &func,
                         std::function<bool(const HWND, const bool, const bool, const VK)> &passage);

    // frees the char lock
    const bool freeChars(ModBase* mod);


    // needs function to add keyboard strokes to use

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    KeyboardInterceptor(const KeyboardInterceptor &base) = delete;
    virtual KeyboardInterceptor& operator=(const KeyboardInterceptor &base) final = delete;

  private:

    void initialize() override;


    // main template for both register functions
    template<bool simpleReturn, bool passage, typename Func, typename KeyComb>
    const auto registerKeyComb(const Func &funcToExecute, KeyComb &keyCombinations, std::array<bool, 3> &allowedKeyComb)
    {
      std::vector<std::array<VK, 3>>* combPtr{ nullptr };

      // only used if array needed
      std::vector<std::array<VK, 3>> kombArray;
      if constexpr (std::is_same<KeyComb, Json>::value)
      {
        kombArray = resolveKeyConfig(keyCombinations);
        combPtr = &kombArray;
      }
      else
      {
        combPtr = &keyCombinations;
      }

      // construct return type
      typename std::conditional<simpleReturn, bool, std::vector<bool>>::type workingKeyCombinations;
      if constexpr (simpleReturn)
      {
        workingKeyCombinations = true;
      }
      else
      {
        workingKeyCombinations = std::vector<bool>(combPtr->size(), false);
      }

      if (funcToExecute)
      {
        // construct object
        typename std::conditional<passage, std::unique_ptr<KPassage>, std::unique_ptr<KAction>>::type newFunc;
        if constexpr (passage)
        {
          newFunc = std::make_unique<KPassage>(funcToExecute);;
        }
        else
        {
          newFunc = std::make_unique<KFunction>(funcToExecute);
        }

        bool onePlaced{ false };

        for (size_t i = 0; i < combPtr->size(); i++)
        {
          const std::array<VK, 3>& keys = combPtr->at(i);

          bool success = registerKey(keys.at(0), keys.at(1), keys.at(2), newFunc.get(), allowedKeyComb);
          onePlaced = onePlaced || success;

          if constexpr (simpleReturn)
          {
            workingKeyCombinations = workingKeyCombinations && success;
          }
          else
          {
            workingKeyCombinations[i] = success;
          }
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


    // receives the json object of one key config and turns it into an vector with the key combinations
    // doesn't check if valid positions, however, if structure not valid, returns an array of VK::NONE
    const std::vector<std::array<VK, 3>> resolveKeyConfig(const Json &keyCombinations);

    // returns true if key successfully registered
    // allowedKeyComb tells the resolver which version are allowed
    // -> 0: only key allowed, 1: modifier + key, 2: modifier + modifier + key
    const bool registerKey(const VK modifierOne, const VK modifierTwo, const VK mainkey,
                           KAction* actionPointer, std::array<bool, 3> &allowedKeyComb);

    // used to resolve general to extended keys if key delivers only general
    const VK generalToExtendedKey(const WPARAM wParam, const LPARAM lParam);

    // resolves key, also return !true! if key should be !stopped!
    const bool resolveKey(const VK wParam, const LPARAM lParam);

    // hooks and taker functions //

    // normal keyboard intercepter
    LRESULT CALLBACK keyIntercepter(_In_ int code,_In_ WPARAM wParam, _In_ LPARAM lParam);

    static LRESULT CALLBACK hookForKeyInterceptor(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam);

    // message interceptor -> only used for char passage
    LRESULT CALLBACK charInterceptor(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam);

    static LRESULT CALLBACK hookForCharInterceptor(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam);
  };
}

#endif //!KEYBOARDINTERCEPTOR