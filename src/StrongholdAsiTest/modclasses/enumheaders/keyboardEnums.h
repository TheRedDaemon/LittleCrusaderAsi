#ifndef KEYBOARDENUMS
#define KEYBOARDENUMS

#include <unordered_set>

// json
#include "../../dependencies/jsonParser/json.hpp"

namespace modclasses 
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
    ALT = 0x12,
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
    PRINT_SCREEN = 0x2C,
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
    LEFT_WINDOWS_KEY = 0x5B,
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
    MULTIPLY = 0x6A,
    ADD = 0x6B,
    SEPERATOR = 0x6C,
    SUBTRACT = 0x6D,
    DECIMAL = 0x6E,
    DIVIDE = 0x6F,
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
    LEFT_MENU = 0xA4,
    RIGHT_MENU = 0xA5,
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
    OEM1 = 0xBA,
    OEM_PLUS = 0xB8,
    OEM_COMMA = 0xBC,
    OEM_MINUS = 0xBD,
    OEM_PERIOD = 0xBE,
    OEM2 = 0xBF,
    OEM3 = 0xC0,
    OEM4 = 0xDB,
    OEM5 = 0xDC,
    OEM6 = 0xDD,
    OEM7 = 0xDE,
    OEM8 = 0xDF,
    OEM102 = 0xE2,
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

  // used to parse string to enum // TODO:fill
  NLOHMANN_JSON_SERIALIZE_ENUM(VK, {
    {VK::NONE, nullptr}
  })

  namespace
  {
    // keys are allowed to modify input, but not be modified
    const std::unordered_set<VirtualKey> modificationKeys{
      VK::LEFT_SHIFT, VK::RIGHT_SHIFT, VK::ALT, VK::RIGHT_CONTROL, VK::LEFT_CONTROL
    };

    //  these keys can be changed // TODO: fill
    const std::unordered_set<VirtualKey> changeableKeys{

    };

    //  these keys are allowed to activate the key interceptor (and maybe later other stuff?)
    const std::unordered_set<VirtualKey> activationKeys{
      VK::HOME, VK::DELETE_KEY, VK::PAGE_UP, VK::PAGE_DOWN
    };
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
}

#endif //KEYBOARDENUMS
