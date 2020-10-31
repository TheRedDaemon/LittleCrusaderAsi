

#ifndef BLTOVERLAY
#define BLTOVERLAY

// DDFontEngine
#include "../dependencies/DDFontEngine/DDFontEngine.h"

#include "modBase.h"

#include "keyboardInterceptor.h"
#include "addressResolver.h"

// DirectDraw
#include "ddraw.h"

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

  struct FontContainer
  {
    LPDIRECTDRAWSURFACE7 lpFontSurf{ nullptr }; // surface to draw on
    tagBITMAPINFO bitmapInfo;                 // bitmap info
    std::vector<unsigned char> bitmapData;    // actual data
    ABC ABCWidths[256];                       // distance values
    RECT SrcRects[256];                       // Pre-calculated SrcRects for Blt
    int BPlusC[256];                          // another distance
    DWORD TextColor;                          // textcolor
  };

  class FontHandler
  {
  private:
    std::unordered_map<FontTypeEnum, FontContainer> fonts{};

  public:

    bool loadFont(FontTypeEnum fontType, Json &fontData, const IDirectDraw7* drawObject);
    bool drawText(LPDIRECTDRAWSURFACE7 destination, FontTypeEnum fontType, const std::string &text, int32_t posX, int32_t posY,
                  int verticalMaxLength, bool centerBoxHorizontal, bool centerVertical, bool truncate, int32_t &retUsedHorizontalSpace);
    void releaseSurfaces();

  };

  // simple proto class, replace all 'ProtoMod' with the new name
  class BltOverlay : public ModBase
  {
  private:

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
    // and the rect structures (contain also size for them) (hardcoded currently
    RECT menuRect{0, 0, 300, 500 };
    RECT textRect{ 0, 0, 500, 250 };
    RECT inputRect{ 0, 0, 300, 150 };
    // and positions on screen
    std::pair<DWORD, DWORD> menuPos{ 0, 0 };  // upper corner
    std::pair<DWORD, DWORD> textPos{ 300, 0 };        // next to menu
    std::pair<DWORD, DWORD> inputPos{ 0, 0 };   // set by program

    // DDFontEngine stuff
    std::unique_ptr<TDDFontEngine> fontEngine;
    std::unique_ptr<TDDFont> fatText;
    std::unique_ptr<TDDFont> normalText;

    // place for whatever structure will be used for additional input stuff

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
    void createOffSurface(IDirectDrawSurface7** surf, DWORD width, DWORD height, DWORD fillColor);


    // static functions: //

    // small helper method to zero ddObject (and set size value)
    template <typename T>
    void zeroDDObject(T &ddObject)
    {
      // zero memory can be optimized away -> should there every be bugs, use SecureZeroMemory
      ZeroMemory(&ddObject, sizeof(ddObject));
    }
    template <typename T>
    void zeroDDObjectAndSetSize(T &ddObject)
    {
      // zero memory can be optimized away -> should there every be bugs, use SecureZeroMemory
      ZeroMemory(&ddObject, sizeof(ddObject));
      ddObject.dwSize = sizeof(ddObject);
    }

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
