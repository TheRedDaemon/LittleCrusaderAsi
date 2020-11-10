

#ifndef BLTOVERLAY
#define BLTOVERLAY

#include <deque>

#include "modBase.h"

#include "keyboardInterceptor.h"
#include "addressResolver.h"

// DirectDraw
#include "ddraw.h"

// conains func for PNG load
// TODO?: GDI+ comes with great overhead, even its own thread
// if there ever happens to be a better solution, use it (pure bitmaps would bloat the exe)
#include "../misc/copiedFunc.h"

// all font loading structures were contructed using the DDFontEngine
// https://realmike.org/blog/projects/fast-bitmap-fonts-for-directdraw/
namespace modclasses
{
  // small global helper method to zero ddObject (and set size value)
  template <typename T>
  void ZeroDDObject(T &ddObject)
  {
    // zero memory can be optimized away -> should there every be bugs, use SecureZeroMemory
    ZeroMemory(&ddObject, sizeof(ddObject));
  }


  template <typename T>
  void ZeroDDObjectAndSetSize(T &ddObject)
  {
    // zero memory can be optimized away -> should there every be bugs, use SecureZeroMemory
    ZeroMemory(&ddObject, sizeof(ddObject));
    ddObject.dwSize = sizeof(ddObject);
  }


  // FONT SUFF


  enum class FontTypeEnum
  {
    NONE,             // for config
    SMALL,
    SMALL_BOLD,
    NORMAL,
    NORMAL_BOLD
  };


  // used to parse string to enum
  NLOHMANN_JSON_SERIALIZE_ENUM(FontTypeEnum, {
    {FontTypeEnum::NONE, nullptr},
    {FontTypeEnum::SMALL, "small"},
    {FontTypeEnum::SMALL_BOLD, "smallBold"},
    {FontTypeEnum::NORMAL, "normal"},
    {FontTypeEnum::NORMAL_BOLD, "normalBold"}
  })


  // will contain the fonds to switch to
  struct FontContainer
  {
    LPDIRECTDRAWSURFACE7 lpFontSurf{ nullptr };     // surface to draw on
    std::unique_ptr<char[]> bitmapInfo{};           // bitmap info
    std::unique_ptr<char[]> bitmapData{};           // actual data
    ABC ABCWidths[256];                             // distance values
    RECT SrcRects[256];                             // Pre-calculated SrcRects for Blt
    int BPlusC[256];                                // another distance
    DWORD TextColor;                                // textcolor

    LPBITMAPINFO getBitmapInfo()  // since the structure is constructed using char... (do not know better way)
    {
      return reinterpret_cast<LPBITMAPINFO>(bitmapInfo.get());
    }
  };


  // used inside FontHandler class
  struct FontConfig
  {
    std::string name;
    int height;
    int weight;
    bool italic;
    bool underline;
    bool strikeOut;
  };


  class FontHandler
  {
  private:
    bool useVideoMemory{ false };
    std::unordered_map<FontTypeEnum, FontContainer> fonts{};

  public:

    bool loadFont(FontTypeEnum fontType, Json &fontData, IDirectDraw7* drawObject, DWORD textcolor);
    bool drawText(LPDIRECTDRAWSURFACE7 destination, FontTypeEnum fontType, const std::string &text, int32_t posX, int32_t posY,
                  int horizontalMaxLength, bool centerHorizontal, bool centerVertical, bool truncate,
                  std::function<std::pair<int32_t, int32_t>(RECT)> *reactToRelSize);
    void releaseSurfaces();
    void setUseVideoMemory(bool setTo)
    {
      useVideoMemory = setTo;
    }

  private:

    FontConfig loadDefaultConfig(FontTypeEnum fontType);
    void cleanInErrorCase(FontTypeEnum fontType, std::string &&logMsg);
    void deleteDCObjects(HFONT &hfont, HBITMAP &hBitmap, HDC &hdc);
  };

  
  // MENU STUFF


  enum class MenuAction
  {
    NONE,             // for config
    RIGHT,
    LEFT,
    UP,
    DOWN,
    ACTION,
    BACK
  };

  // used to parse string to enum
  NLOHMANN_JSON_SERIALIZE_ENUM(MenuAction, {
    {MenuAction::NONE, nullptr},
    {MenuAction::RIGHT, "right"},
    {MenuAction::LEFT, "left"},
    {MenuAction::UP, "up"},
    {MenuAction::DOWN, "down"},
    {MenuAction::ACTION, "action"},
    {MenuAction::BACK, "back"}
  })

  // forward declaration
  class BltOverlay;

  class MenuBase
  {
    // headline
  private:
    std::string header;
    MenuBase* lastMenu{nullptr};

    // will receive ref to header, also, first bool is 'leaving', true would be leaving the current menu
    // needs to return true, if the menu should be enterable (ignored if leave?)
    std::function<bool(bool leaving, std::string& header)> accessReact{ nullptr };

  public:
    MenuBase(const std::string &headerString, std::function<bool(bool leaving, std::string& header)> accessReactFunc)
      : header(headerString), accessReact(accessReactFunc) { };

    // peforms any action -> if return != null, set current menu in BltOverlay to this return -> needs interfaces in case redraws are needed
    // likely needs redesign -> hwo do I send the other keys?
    MenuBase* executeAction(MenuAction actionType, BltOverlay &over)
    {
      switch (actionType)
      {
        case MenuAction::ACTION:
          return action(over);
        case MenuAction::BACK:
          return back(over);
        case MenuAction::UP:
        case MenuAction::DOWN:
        case MenuAction::RIGHT:
        case MenuAction::LEFT:
          move(actionType, over);
          break;
        default:
          break;
      }

      return nullptr;
    }

    // key is placeholder, until I know what to send
    // send overlay for fully controll
    virtual MenuBase* executeAction(char chr, BltOverlay &over)
    {
      return nullptr;
    }

    // should only be used by BltOverlay
    // forces draw of parent (if any) then this
    // a bit hacky -> either menu is drawn twice, or last menu status of input is restored
    void forceDraw(BltOverlay &over)
    {
      if (lastMenu)
      {
        lastMenu->draw(over);
      }
      draw(over);
    }

    virtual ~MenuBase(){}

    // prevent copy and assign (not sure how necessary)
    MenuBase(const MenuBase&) = delete;
    MenuBase& operator= (const MenuBase&) = delete;

  private:

    virtual MenuBase* access(MenuBase* caller, bool callerLeaving, BltOverlay &over) = 0;
    virtual void move(MenuAction direction, BltOverlay &over) = 0;
    virtual MenuBase* action(BltOverlay &over) = 0;
    virtual MenuBase* back(BltOverlay &over) = 0;
    virtual void draw(BltOverlay &over) = 0;

    // derived are all friend of the BaseClass, to access the private variables
    friend class MainMenu;
    friend class FreeInputMenu;

    template<typename T>
    friend class ChoiceInputMenu;
  };


  class MainMenu : public MenuBase
  {
  private:

    bool bigMenu;
    size_t currentSelected{ 0 };
    std::pair<size_t, size_t> startEndVisible;
    std::unique_ptr<std::vector<std::unique_ptr<MenuBase>>> subMenus;

  public:

    MainMenu(const std::string &headerString, const std::function<bool(bool leaving, std::string& header)> accessReactFunc,
             bool isItBigMenu, std::unique_ptr<std::vector<std::unique_ptr<MenuBase>>> subMenusObj);

    // should only be used by BltOverlay
    // adds menu by move
    // note, the given structure will not be valid after this function
    void addMenuStructure(std::unique_ptr<MenuBase> &menu);

    // prevent copy and assign (not sure how necessary)
    MainMenu(const MainMenu&) = delete;
    MainMenu& operator= (const MainMenu&) = delete;

  private:

    void computeStartEndVisible();
    void menuBoxDrawHelper(std::string& text, int32_t yPos, bool active, BltOverlay &over);

    MenuBase* access(MenuBase* caller, bool callerLeaving, BltOverlay &over) override;
    void move(MenuAction direction, BltOverlay &over) override;
    MenuBase* action(BltOverlay &over) override;
    MenuBase* back(BltOverlay &over) override;
    void draw(BltOverlay &over) override;
  };


  class FreeInputMenu : public MenuBase
  {
  private:

    bool onlyNumber;

    // will tell the current impl if the keyboard is overtaken (and that it might need to free it)
    bool overtake;
    std::string currentInput;

    const std::string defaultValue;
    std::string currentValue;
    std::string resultOfEnter;

  public:
    // prevent copy and assign (not sure how necessary)
    FreeInputMenu(const FreeInputMenu&) = delete;
    FreeInputMenu& operator= (const FreeInputMenu&) = delete;

  private:

    MenuBase* access(MenuBase* caller, bool callerLeaving, BltOverlay &over) override;
    void move(MenuAction direction, BltOverlay &over) override;
    MenuBase* action(BltOverlay &over) override;
    MenuBase* back(BltOverlay &over) override;
    void draw(BltOverlay &over) override;
  };


  // template?
  template<typename T>
  class ChoiceInputMenu : public MenuBase
  {
  private:

    // vector will contain choices -> strings are whats displayed
    std::vector<std::pair<std::string, T>> choicePairs;

    const std::string defaultValue;
    std::string currentValue;
    std::string resultOfEnter;
  
  public:
    // prevent copy and assign (not sure how necessary)
    ChoiceInputMenu(const ChoiceInputMenu&) = delete;
    ChoiceInputMenu& operator= (const ChoiceInputMenu&) = delete;

  private:

    MenuBase* access(MenuBase* caller, bool callerLeaving, BltOverlay &over) override;
    void move(MenuAction direction, BltOverlay &over) override;
    MenuBase* action(BltOverlay &over) override;
    MenuBase* back(BltOverlay &over) override;
    void draw(BltOverlay &over) override;
  };


  // MAIN PART
  
  // simple proto class, replace all 'ProtoMod' with the new name
  class BltOverlay : public ModBase
  {
  private:

    // containes the rects of every menu part
    // everything hardcoded, also only need one of them
    static constexpr struct MenuCompRects
    {
      // can not be const, since non const pointer required (made inline?)
      inline static RECT mainMenu{ 0, 0, 300, 500 };
      inline static RECT consoleBorder{ 300, 0, 800, 250 };
      inline static RECT msgIcon{ 800, 0, 812, 12 };
      inline static RECT bigButton{ 0, 500, 280, 560 };
      inline static RECT bigButtonPressed{ 0, 560, 280, 620 };
      inline static RECT smallButton{ 300, 250, 580, 282 };
      inline static RECT smallButtonPressed{ 300, 282, 580, 314 };
      inline static RECT smallInputBox{ 300, 314, 600, 464 };
      inline static RECT bigInputBox{ 600, 250, 900, 500 };
      inline static RECT inputField{ 300, 496, 550, 528 };
      inline static RECT inputFieldSelected{ 300, 528, 550, 560 };
      inline static RECT okBox{ 300, 464, 332, 496 };
      inline static RECT okBoxSelected{ 332, 464, 364, 496 };
      inline static RECT cancelBox{ 364, 464, 396, 496 };
      inline static RECT cancelBoxSelected{ 396, 464, 428, 496 };
      inline static RECT goldArrowUp{ 280, 500, 300, 560 };
      inline static RECT goldArrowDown{ 280, 560, 300, 620 };
      inline static RECT silverArrowUp{ 580, 250, 600, 282 };
      inline static RECT silverArrowDown{ 580, 282, 600, 314 };
    } menuRects{};

    IDirectDraw7* dd7InterfacePtr{ nullptr };            // currentMainInterface
    IDirectDrawSurface7* dd7BackbufferPtr{ nullptr };    // currentBackbufferInterface

    // keeps ddInterfaceRefs
    // -> for d7tod9 (removing the references removes handling window for ex.)
    bool doNotKeepDD7Interface{ false };
    
    // DDs does not have hardware support out of the box
    // -> if calls fail, this will be set to true, preventing repeats
    bool disableHardwareTest{ false };

    // -> redesign in your head:
    // alpha blending seems to be a complicated thing 
    // -> there is no function support, one would have to do it manually in software
    // -> so plan changed -> text stuff smaller and everything opaque menu

    // main surfaces applied per fast blt
    IDirectDrawSurface7* menuOffSurf; // menu
    IDirectDrawSurface7* textOffSurf; // display text
    IDirectDrawSurface7* inputOffSurf; // input field in the middle of screen
    // and the rect structures (contain also size for them) (hardcoded currently)
    RECT menuRect{0, 0, 300, 500 };
    RECT textRect{ 0, 0, 500, 250 };
    RECT inputRect{ 0, 0, 300, 250 };
    // and positions on screen
    std::pair<DWORD, DWORD> menuPos{ 0, 0 };  // upper corner
    std::pair<DWORD, DWORD> textPos{ 300, 0 };        // next to menu
    std::pair<DWORD, DWORD> inputPos{ 0, 0 };   // set by program
    // status
    bool menuActive{ false };
    bool textActive{ false };
    bool inputActive{ false };

    // small menu indicator active
    bool menuIndicator{ false };
    RECT menuSmallRect{ 10, 10, 290, 70 };

    // font stuff
    FontHandler fntHandler{};
    Json fontConfigs;

    // console -> (current max size hardcoded to 10)
    std::deque<std::string> conQueue;

    // menu components
    CImage menuComp;
    IDirectDrawSurface7* compSurf;

    // functions to execute on DDraw Load event
    std::vector<std::function<void()>> funcsForDDrawLoadEvent;

    // keyboard stuff
    Json menuShortcut;
    Json consoleShortcut;
    std::weak_ptr<KeyboardInterceptor> keyInterPtr;
    std::unordered_map<VK, MenuAction> menuActions;
    bool keysValid{ true };

    // menu ptr;
    std::unique_ptr<MainMenu> mainMenu{}; // mostly for reference -> will own every menu part at the end
    MenuBase* currentMenu{ nullptr };  // current menu will receive all inputs if active


    // needed to give the address resolver the right infos
    // can be static, I don't assume changes
    static std::vector<AddressRequest> usedAddresses;


  public:

    ModType getModType() const override
    {
      return ModType::BLT_OVERLAY;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::KEYBOARD_INTERCEPTOR, ModType::ADDRESS_RESOLVER };
    }

    void cleanUp() override;

    /**con- and destructor**/
    BltOverlay(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config); // default cons, does nothing though

    /**additional functions for others**/

    // send a msg to the console
    // the return bool will indicate if the msg was delivered to the console
    bool sendToConsole(const std::string &msg);
    
    // send a msg to the console and create a log
    // uses the already forced singelton for this
    // the return bool will indicate if the msg was delivered to the console
    // log should always happen (but of cause would dirty the trace)
    // currently only level INFO, WARNING and ERROR are supported
    static bool sendToConsole(const std::string &msg, el::Level logLevel);

    // TODO: this might need a better place (event handler?)
    // event fired when ddraw is loaded in stronghold (at least for GetProcAddress for Create calls)
    void registerDDrawLoadEvent(std::function<void()> &&func);

    // warning -> takes of cause ownership
    void addMenu(std::unique_ptr<MenuBase> menu);

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    BltOverlay(const BltOverlay &base) = delete;
    virtual BltOverlay& operator=(const BltOverlay &base) final = delete;


  private:

    // should be defined in .cpp
    void initialize() override;

    // releases current interfaces if new encountered
    void releaseDDInterfaces();

    // creates all needed interfaces after a new main object was registered
    void prepareDDInterfaces(IDirectDraw7* const that, IDirectDrawSurface7* const dd7SurfacePtr);

    // blts the overlay surface in the backbuffer
    void bltMainDDOffSurfs();

    // needs pointer to value
    bool createOffSurface(IDirectDrawSurface7** surf, DWORD width, DWORD height, DWORD fillColor);

    // redraws console based on msg queue
    void updateConsole();

    // keyboard:

    void switchConsole(const HWND window, const bool keyUp, const bool repeat);
    
    void switchMenu(const HWND window, const bool keyUp, const bool repeat);
    
    // returns bool to also control the key passage in input case
    bool controlMenu(const HWND window, const bool keyUp, const bool repeat, const VK key);

    // can be called by the input fields -> enables and disabled char receive, if possible
    // note, if already set, will also return false -> keep memory of status
    bool enableCharReceive(bool enable);

    // takes the send input chars
    void receiveChar(char chr);

    // static functions: //

    // fires when stronghold requests ddraw to be loaded
    static HMODULE _stdcall LoadLibraryFake(LPCSTR lpLibFileName);

    // IDirectDraw7 ptr is invalid once it reaches this position
    // -> old surfaces sometimes are lost, sometimes they work
    // -> depends on asiLoader (and its config)
    // test with different main dd7 object -> did not get it to work
    // it returned ok, but nothing was drawn on screen
    static HRESULT _stdcall CreateSurfaceFake(IDirectDraw7* const that, LPDDSURFACEDESC2 desc2, LPDIRECTDRAWSURFACE7* surf7, IUnknown* unkn);

    // flip is called at a relative constant time -> no menu responsiveness issues
    static HRESULT _stdcall FlipFake(IDirectDrawSurface7* const that, LPDIRECTDRAWSURFACE7 surf7, DWORD flags);

    // all menu friend
    friend class MainMenu;
    friend class FreeInputMenu;
    
    template<typename T>
    friend class ChoiceInputMenu;
  };
}

#endif //BLTOVERLAY