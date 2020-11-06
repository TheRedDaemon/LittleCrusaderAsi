

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
      inline static RECT bigButtonPressed{ 0, 560, 280, 560 };
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
    bool textActive{ true };
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
    // the return bool will indicate if the msg was delivered to the console
    // log should always happen (but of cause would dirty the trace)
    // currently only level INFO, WARNING and ERROR are supported
    bool sendToConsole(const std::string &msg, el::Level logLevel);

    // TODO: this might need a better place (event handler?)
    // event fired when ddraw is loaded in stronghold (at least for GetProcAddress for Create calls)
    void registerDDrawLoadEvent(std::function<void()> &&func);

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

    // redraws input field
    void updateInput();

    // redraws menu
    void updateMenu();


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
  };
}

#endif //BLTOVERLAY