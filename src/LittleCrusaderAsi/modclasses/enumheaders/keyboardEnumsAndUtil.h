#ifndef KEYBOARDENUMSANDUTIL
#define KEYBOARDENUMSANDUTIL

#include <unordered_set>
#include <Windows.h>  // you create a lot of noice... TODO: if ever time, try to remove

// json
#include "../../dependencies/JsonForModernC++/json.hpp"

namespace modclasses 
{
  namespace keyboard
  {
    // virtual keys and descriptions from microsoft: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    // enum from: https://www.codeproject.com/Tips/73227/Keys-Enumeration-Win32 and https://www.pinvoke.net/default.aspx/Enums/VirtualKeys.html
    // no class, to make it easier comparable
    enum VirtualKey: unsigned char
    {
      NONE = 0x00, // for invalid stuff
      LEFT_MOUSE_BUTTON = 0x01,
      RIGHT_MOUSE_BUTTON = 0x02,
      CANCEL = 0x03,
      MIDDLE_MOUSE_BUTTON = 0x04,
      EXTRA_MOUSE_BUTTON1 = 0x05,
      EXTRA_MOUSE_BUTTON2 = 0x06,
      BACKSPACE = 0x08,
      TAB = 0x09,
      CLEAR = 0x0C,
      ENTER = 0x0D,
      SHIFT = 0x10,
      CONTROL = 0x11,
      ALT = 0x12, // also menu
      PAUSE = 0x13,
      CAPSLOCK = 0x14,
      ESCAPE = 0x1B,
      SPACE = 0x20,
      PAGE_UP = 0x21,
      PAGE_DOWN = 0x22,
      END = 0x23,
      HOME = 0x24,
      LEFT = 0x25,
      UP = 0x26,
      RIGHT = 0x27,
      DOWN = 0x28,
      SELECT = 0x29,
      PRINT = 0x2A,
      EXECUTE = 0x2B,
      PRINT_SCREEN = 0x2C, // german keys: Druck
      INSERT = 0x2D,
      DELETE_KEY = 0x2E,
      HELP = 0x2F,
      ZERO = 0x30,
      ONE = 0x31,
      TWO = 0x32,
      THREE = 0x33,
      FOUR = 0x34,
      FIVE = 0x35,
      SIX = 0x36,
      SEVEN = 0x37,
      EIGHT = 0x38,
      NINE = 0x39,
      A = 0x41,
      B = 0x42,
      C = 0x43,
      D = 0x44,
      E = 0x45,
      F = 0x46,
      G = 0x47,
      H = 0x48,
      I = 0x49,
      J = 0x4A,
      K = 0x4B,
      L = 0x4C,
      M = 0x4D,
      N = 0x4E,
      O = 0x4F,
      P = 0x50,
      Q = 0x51,
      R = 0x52,
      S = 0x53,
      T = 0x54,
      U = 0x55,
      V = 0x56,
      W = 0x57,
      X = 0x58,
      Y = 0x59,
      Z = 0x5A,
      LEFT_WINDOWS_KEY = 0x5B, // reported as additional key, but as left (not general), so right should report also as such
      RIGHT_WINDOWS_KEY = 0x5C,
      APPLICATIONS_KEY = 0x5D,
      SLEEP = 0x5F,
      NUMPAD0 = 0x60,
      NUMPAD1 = 0x61,
      NUMPAD2 = 0x62,
      NUMPAD3 = 0x63,
      NUMPAD4 = 0x64,
      NUMPAD5 = 0x65,
      NUMPAD6 = 0x66,
      NUMPAD7 = 0x67,
      NUMPAD8 = 0x68,
      NUMPAD9 = 0x69,
      MULTIPLY = 0x6A, // num
      ADD = 0x6B, // num
      SEPERATOR = 0x6C, // num
      SUBTRACT = 0x6D,  // num
      DECIMAL = 0x6E, // german keys: num comma
      DIVIDE = 0x6F, // num
      F1 = 0x70,
      F2 = 0x71,
      F3 = 0x72,
      F4 = 0x73,
      F5 = 0x74,
      F6 = 0x75,
      F7 = 0x76,
      F8 = 0x77,
      F9 = 0x78,
      F10 = 0x79,
      F11 = 0x7A,
      F12 = 0x7B,
      F13 = 0x7C,
      F14 = 0x7D,
      F15 = 0x7E,
      F16 = 0x7F,
      F17 = 0x80,
      F18 = 0x81,
      F19 = 0x82,
      F20 = 0x83,
      F21 = 0x84,
      F22 = 0x85,
      F23 = 0x86,
      F24 = 0x87,
      NUMLOCK = 0x90,
      SCROLL_LOCK = 0x91,
      LEFT_SHIFT = 0xA0,
      RIGHT_SHIFT = 0xA1,
      LEFT_CONTROL = 0xA2,
      RIGHT_CONTROL = 0xA3,
      LEFT_MENU = 0xA4, // left alt (reported as alt addititional key flag)
      RIGHT_MENU = 0xA5, // right alt (german keyboard reports as ctrl + alt(with addititional key flag)
      BROWSER_BACK = 0xA6,
      BROWSER_FORWARD = 0xA7,
      BROWSER_REFRESH = 0xA8,
      BROWSER_STOP = 0xA9,
      BROWSER_SEARCH = 0xAA,
      BROWSER_FAVORITES = 0xAB,
      BROWSER_HOME = 0xAC,
      VOLUME_MUTE = 0xAD,
      VOLUME_DOWN = 0xAE,
      VOLUME_UP = 0xAF,
      NEXT_TRACK = 0xB0,
      PREVIOUS_TRACK = 0xB1,
      STOP_MEDIA = 0xB2,
      PLAY_PAUSE = 0xB3,
      LAUNCH_MAIL = 0xB4,
      SELECT_MEDIA = 0xB5,
      LAUNCH_APP1 = 0xB6,
      LAUNCH_APP2 = 0xB7,
      OEM1 = 0xBA,  // german keys: �
      OEM_PLUS = 0xBB,  // non num versions
      OEM_COMMA = 0xBC,
      OEM_MINUS = 0xBD,
      OEM_PERIOD = 0xBE,
      OEM2 = 0xBF,  // german keys: #
      OEM3 = 0xC0,  // german keys: �
      OEM4 = 0xDB,  // german keys: �
      OEM5 = 0xDC,  // german keys: ZIRKUMFLEX
      OEM6 = 0xDD,  // german keys: �
      OEM7 = 0xDE,  // german keys: �
      OEM8 = 0xDF,
      OEM102 = 0xE2, //  german keys: <
      PROCESS = 0xE5,
      PACKET = 0xE7,
      ATTN = 0xF6,
      CRSEL = 0xF7,
      EXSEL = 0xF8,
      ERASEEOF = 0xF9,
      PLAY = 0xFA,
      ZOOM = 0xFB,
      PA1 = 0xFD,
      OEM_CLEAR = 0xFE
    };


    using VK = VirtualKey;

    // used to parse string to enum
    NLOHMANN_JSON_SERIALIZE_ENUM(VK, {
      {VK::NONE, nullptr},
      {VK::LEFT_MOUSE_BUTTON, "lmb"},
      {VK::RIGHT_MOUSE_BUTTON, "rmb"},
      {VK::CANCEL, "cancel"},
      {VK::MIDDLE_MOUSE_BUTTON, "mmb"},
      {VK::EXTRA_MOUSE_BUTTON1, "extramb1"},
      {VK::EXTRA_MOUSE_BUTTON2, "extramb2"},
      {VK::BACKSPACE, "backspace"},
      {VK::TAB, "tab"},
      {VK::CLEAR, "clear"},
      {VK::ENTER, "enter"},
      {VK::SHIFT, "shift"},
      {VK::CONTROL, "ctrl"},
      {VK::ALT, "alt"}, // also menu
      {VK::PAUSE, "pause"},
      {VK::CAPSLOCK, "caps"},
      {VK::ESCAPE, "esc"},
      {VK::SPACE, "space"},
      {VK::PAGE_UP, "pgup"},
      {VK::PAGE_DOWN, "pgdown"},
      {VK::END, "end"},
      {VK::HOME, "home"},
      {VK::LEFT, "left"},
      {VK::UP, "up"},
      {VK::RIGHT, "right"},
      {VK::DOWN, "down"},
      {VK::SELECT, "select"},
      {VK::PRINT, "print"},
      {VK::EXECUTE, "execute"},
      {VK::PRINT_SCREEN, "printscreen"},
      {VK::INSERT, "insert"},
      {VK::DELETE_KEY, "delete"},
      {VK::HELP, "help"},
      {VK::ZERO, "0"},
      {VK::ONE, "1"},
      {VK::TWO, "2"},
      {VK::THREE, "3"},
      {VK::FOUR, "4"},
      {VK::FIVE, "5"},
      {VK::SIX, "6"},
      {VK::SEVEN, "7"},
      {VK::EIGHT, "8"},
      {VK::NINE, "9"},
      {VK::A, "a"},
      {VK::B, "b"},
      {VK::C, "c"},
      {VK::D, "d"},
      {VK::E, "e"},
      {VK::F, "f"},
      {VK::G, "g"},
      {VK::H, "h"},
      {VK::I, "i"},
      {VK::J, "j"},
      {VK::K, "k"},
      {VK::L, "l"},
      {VK::M, "m"},
      {VK::N, "n"},
      {VK::O, "o"},
      {VK::P, "p"},
      {VK::Q, "q"},
      {VK::R, "r"},
      {VK::S, "s"},
      {VK::T, "t"},
      {VK::U, "u"},
      {VK::V, "v"},
      {VK::W, "w"},
      {VK::X, "x"},
      {VK::Y, "y"},
      {VK::Z, "z"},
      {VK::LEFT_WINDOWS_KEY, "winleft"},
      {VK::RIGHT_WINDOWS_KEY, "winright"},
      {VK::APPLICATIONS_KEY, "applications"},
      {VK::SLEEP, "sleep"},
      {VK::NUMPAD0, "num0"},
      {VK::NUMPAD1, "num1"},
      {VK::NUMPAD2, "num2"},
      {VK::NUMPAD3, "num3"},
      {VK::NUMPAD4, "num4"},
      {VK::NUMPAD5, "num5"},
      {VK::NUMPAD6, "num6"},
      {VK::NUMPAD7, "num7"},
      {VK::NUMPAD8, "num8"},
      {VK::NUMPAD9, "num9"},
      {VK::MULTIPLY, "nummul"},
      {VK::ADD, "numadd"},
      {VK::SEPERATOR, "numsep"},
      {VK::SUBTRACT, "numsup"},
      {VK::DECIMAL, "numdec"},
      {VK::DIVIDE, "numdiv"},
      {VK::F1, "f1"},
      {VK::F2, "f2"},
      {VK::F3, "f3"},
      {VK::F4, "f4"},
      {VK::F5, "f5"},
      {VK::F6, "f6"},
      {VK::F7, "f7"},
      {VK::F8, "f8"},
      {VK::F9, "f9"},
      {VK::F10, "f10"},
      {VK::F11, "f11"},
      {VK::F12, "f12"},
      {VK::F13, "f13"},
      {VK::F14, "f14"},
      {VK::F15, "f15"},
      {VK::F16, "f16"},
      {VK::F17, "f17"},
      {VK::F18, "f18"},
      {VK::F19, "f19"},
      {VK::F20, "f20"},
      {VK::F21, "f21"},
      {VK::F22, "f22"},
      {VK::F23, "f23"},
      {VK::F24, "f24"},
      {VK::NUMLOCK, "numlock"},
      {VK::SCROLL_LOCK, "scrolllock"},
      {VK::LEFT_SHIFT, "leftshift"},
      {VK::RIGHT_SHIFT, "rightshift"},
      {VK::LEFT_CONTROL, "leftctrl"},
      {VK::RIGHT_CONTROL, "rightctrl"},
      {VK::LEFT_MENU, "leftalt"}, // left alt
      {VK::RIGHT_MENU, "rightalt"}, // right alt
      {VK::BROWSER_BACK, "browserback"},
      {VK::BROWSER_FORWARD, "browserforward"},
      {VK::BROWSER_REFRESH, "browserrefresh"},
      {VK::BROWSER_STOP, "browserstop"},
      {VK::BROWSER_SEARCH, "browsersearch"},
      {VK::BROWSER_FAVORITES, "browserfavorites"},
      {VK::BROWSER_HOME, "browserhome"},
      {VK::VOLUME_MUTE, "volumemute"},
      {VK::VOLUME_DOWN, "volumedown"},
      {VK::VOLUME_UP, "volumeup"},
      {VK::NEXT_TRACK, "nexttrack"},
      {VK::PREVIOUS_TRACK, "prevtrack"},
      {VK::STOP_MEDIA, "stopmedia"},
      {VK::PLAY_PAUSE, "playpause"},
      {VK::LAUNCH_MAIL, "launchmail"},
      {VK::SELECT_MEDIA, "selectmedia"},
      {VK::LAUNCH_APP1, "launchapp1"},
      {VK::LAUNCH_APP2, "launchapp2"},
      {VK::OEM1, "oem1"},
      {VK::OEM_PLUS, "oemplus"},
      {VK::OEM_COMMA, "oemcomma"},
      {VK::OEM_MINUS, "oemminus"},
      {VK::OEM_PERIOD, "oemperiod"},
      {VK::OEM2, "oem2"},
      {VK::OEM3, "oem3"},
      {VK::OEM4, "oem4"},
      {VK::OEM5, "oem5"},
      {VK::OEM6, "oem6"},
      {VK::OEM7, "oem7"},
      {VK::OEM8, "oem8"},
      {VK::OEM102, "oem102"},
      {VK::PROCESS, "process"},
      {VK::PACKET, "packet"},
      {VK::ATTN, "attn"},
      {VK::CRSEL, "crsel"},
      {VK::EXSEL, "exsel"},
      {VK::ERASEEOF, "eraseeof"},
      {VK::PLAY, "play"},
      {VK::ZOOM, "zoom"},
      {VK::PA1, "pa1"},
      {VK::OEM_CLEAR, "oemclear" }
    })

    namespace
    {
      using VK = keyboard::VirtualKey;

      // keys are allowed to modify input, but not be modified
      // note wParam send the general key, however, using "lParam & 0x01000000 ? true : false" one can find out, if it is the right or the left key
      // the general key will be used to detect the key, and then will be resolved to the side
      // to get a general "shift" result, just add both left and right shift combination
      // also note, the altGr key is control + right alt, not only right alt
      // also, the lookup will use the order r/l and alt/control/shift, which is an implementation result
      // (check map is traversed in reverse natural order, so that NONE is at the end)
      const std::unordered_set<VirtualKey> modificationKeys{
        VK::LEFT_SHIFT, VK::RIGHT_SHIFT, VK::LEFT_MENU, VK::RIGHT_MENU, VK::LEFT_CONTROL, VK::RIGHT_CONTROL
      };

      //  these keys can be changed, but also be assigned to functions etc. // TODO: fill
      const std::unordered_set<VirtualKey> changeableKeys{
        VK::PAUSE, VK::SPACE, VK::LEFT, VK::UP, VK::RIGHT, VK::DOWN,
        VK::ZERO, VK::ONE, VK::TWO, VK::THREE, VK::FOUR, VK::FIVE, VK::SIX, VK::SEVEN, VK::EIGHT, VK::NINE,
        VK::A, VK::B, VK::C, VK::D, VK::E, VK::F, VK::G, VK::H, VK::I, VK::J, VK::K, VK::L, VK::M, VK::N,
        VK::O, VK::P, VK::Q, VK::R, VK::S, VK::T, VK::U, VK::V, VK::W, VK::X, VK::Y, VK::Z,

        // numpad keys
        VK::NUMPAD0, VK::NUMPAD1, VK::NUMPAD2, VK::NUMPAD3, VK::NUMPAD4, VK::NUMPAD5, VK::NUMPAD6, VK::NUMPAD7, VK::NUMPAD8, VK::NUMPAD9,

        // all numpad operators
        VK::MULTIPLY, VK::ADD, VK::SEPERATOR, VK::SUBTRACT, VK::DECIMAL, VK::DIVIDE,

        // I assume these will work
        VK::F1, VK::F2, VK::F3, VK::F4, VK::F5, VK::F6, VK::F7, VK::F8, VK::F9, VK::F10, VK::F11, VK::F12,

        // are equal to the german keys (not num versions)
        VK::OEM_PLUS, VK::OEM_COMMA,  VK::OEM_MINUS, VK::OEM_PERIOD

        // there are likely some not as easy accessible
      };

      //  these keys are allowed to activate the key interceptor (and maybe later other stuff?)
      const std::unordered_set<VirtualKey> activationKeys{
        VK::HOME, VK::DELETE_KEY, VK::PAGE_UP, VK::PAGE_DOWN
      };

      // NOTE: which keys can be used for what is still subject of change
    }

    inline const bool isModificationKey(const VK key)
    {
      return modificationKeys.find(key) != modificationKeys.end();
    }

    inline const bool isChangeableKey(const VK key)
    {
      return changeableKeys.find(key) != changeableKeys.end();
    }

    inline const bool isActivationKey(const VK key)
    {
      return activationKeys.find(key) != activationKeys.end();
    }

    
    /*************************************************************************/


    class KeyAction
    {
    public:

      virtual void doAction(const HWND window, const bool keyUp, const bool repeat, const VK key) const = 0;

      KeyAction() {}
      virtual ~KeyAction(){ }
      // prevent copy and assign (not sure how necessary)
      KeyAction(const KeyAction &keyAction) = delete;
      virtual KeyAction& operator=(const KeyAction &keyAction) final = delete;
    };


    // for now ignoring lparam, also doesn't send letters at the moment
    // adding restructure to use char modifiers would require additional structure changes
    // at the moment, I rather use the switch, or later, a event notification that an text input is required
    // could also be that, at this point, keyboard changes are done in the crusader keyboard handler and
    // this here will only serv for func calls (and this might make this class useless)
    class KeyChange : public KeyAction
    {
    private:

      const VK keyToUse;

    public:

      void doAction(const HWND window, const bool keyUp, const bool repeat, const VK key) const override;

      KeyChange(VK keySendInstead):keyToUse{ keySendInstead }{};

      // prevent copy and assign (not sure how necessary)
      KeyChange(const KeyChange &keyAction) = delete;
      virtual KeyChange& operator=(const KeyChange &keyAction) final = delete;
    };


    class KeyFunction : public KeyAction
    {
    private:

      std::function<void(const HWND, const bool, const bool)> funcToUse;

    public:

      void doAction(const HWND window, const bool keyUp, const bool repeat, const VK key) const override;

      KeyFunction(const std::function<void(const HWND, const bool, const bool)> &funcToExecute):funcToUse{ funcToExecute }{}

      // prevent copy and assign (not sure how necessary)
      KeyFunction(const KeyFunction &keyAction) = delete;
      virtual KeyFunction& operator=(const KeyFunction &keyAction) final = delete;
    };


    // pass the key -> keeping others for now
    class KeyPassage : public KeyAction
    {
    private:

      std::function<void(const HWND, const bool, const bool, const VK)> passageFunc;

    public:

      void doAction(const HWND window, const bool keyUp, const bool repeat, const VK key) const override;

      KeyPassage(const std::function<void(const HWND, const bool, const bool, const VK)> &passageFunc):passageFunc{ passageFunc }{}

      // prevent copy and assign (not sure how necessary)
      KeyPassage(const KeyPassage &keyAction) = delete;
      virtual KeyPassage& operator=(const KeyPassage &keyAction) final = delete;
    };
  }

  using VK = keyboard::VirtualKey;
  using KAction = keyboard::KeyAction;
  using KChange = keyboard::KeyChange;
  using KFunction = keyboard::KeyFunction;
  using KPassage = keyboard::KeyPassage;
}

#endif //KEYBOARDENUMSANDUTIL
