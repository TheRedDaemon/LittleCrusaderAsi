
#include "bltOverlay.h"
#include "../resources/resource.h"

// main source ddraw: https://www.codeproject.com/Articles/2370/Introduction-to-DirectDraw-and-Surface-Blitting
// another: http://www.visualbasicworld.de/tutorial-directx-directdraw.html
namespace modclasses
{
  // macro found online, source: https://gamedev.net/forums/topic/289485-16-bit-color-on-directdraw-surfaces/2825618/
  #define RGB16(r, g, b) ((b >> 3) | ((g >> 2) << 5) | ((r >> 3) << 11))

  // from: https://realmike.org/blog/projects/fast-bitmap-fonts-for-directdraw/
  #define TEXTCOLOR(r, g, b) ((DWORD) (((BYTE) (b) | ((WORD) (g) << 8)) | (((DWORD) (BYTE) (r)) << 16)))

  // pointer for static functions
  static BltOverlay *overPtr{ nullptr };
  

  // NOTE: -> linking to static functions seem to work pretty good -> the copy needs to be nice though (in first tests, _stdcall was missing)
  // maybe there are some stuff like _stdcall etc that might help me?

  // Small Note: In theory it could be possible to allow Crusader to run in windowed mode...maybe.
  // The defining steps are: manipulating the Primary Surface for windowed mode -> create a Clipper for windowed (or for backbuffer)
  // Disable backbuffer on Primary Surface, redirect GetAttachedSurface of Stronghold to return an unrelated Surface as backbuffer
  // (I think this one needs the clipper -> see I-net)
  // use the flip to instead blt the backbuffer on the primary surface -> other problems might stay
  // BACKLOG -> only if ever interest in this

  HMODULE _stdcall BltOverlay::LoadLibraryFake(LPCSTR lpLibFileName)
  {
    for (auto& func : (*overPtr).funcsForDDrawLoadEvent)
    {
      func();
    }

    return LoadLibrary(lpLibFileName);
  }

  HRESULT _stdcall BltOverlay::CreateSurfaceFake(IDirectDraw7* const that, LPDDSURFACEDESC2 desc2, LPDIRECTDRAWSURFACE7* surf7, IUnknown* unkn)
  {
    // keep control over pointer
    overPtr->releaseDDInterfaces();

    HRESULT frontSurfRes{ that->CreateSurface(desc2, surf7, unkn) };

    if (frontSurfRes == S_OK)
    {
      overPtr->prepareDDInterfaces(that, *surf7);
    }

    return frontSurfRes;
  }


  HRESULT _stdcall BltOverlay::FlipFake(IDirectDrawSurface7* const that, LPDIRECTDRAWSURFACE7 surf7, DWORD flags)
  {
    overPtr->bltMainDDOffSurfs();

    return that->Flip(surf7, flags);
  }


  /*******************************************************/
  // Non-static part starts here:
  /*******************************************************/


  BltOverlay::BltOverlay(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
    auto confIt = config.find("doNotKeepDD7Interface");
    if (confIt != config.end() && confIt.value().is_boolean())
    {
      doNotKeepDD7Interface = confIt.value().get<bool>();
    }
    else
    {
      LOG(WARNING) << "BltOverlay: No valid 'doNotKeepDD7Interface' found. Needs to be boolean. Defaults to 'false'. "
        << "Only used for some D7to9 modes (if asiloader is dxwrapper) to prevent issues. Ex. window modes needs 'true', or it vanishes.";
    }

    confIt = config.find("menuIndicator");
    if (confIt != config.end() && confIt.value().is_boolean())
    {
      menuIndicator = confIt.value().get<bool>();
    }
    else
    {
      LOG(WARNING) << "BltOverlay: No valid 'menuIndicator' found. Needs to be boolean. Defaults to 'false'. "
        << "If 'true', shows a menu box as indicator that the menu is active, even if closed.";
    }

    confIt = config.find("menuActiveAtStart");
    if (confIt != config.end() && confIt.value().is_boolean())
    {
      menuActive = confIt.value().get<bool>();
    }
    else
    {
      LOG(WARNING) << "BltOverlay: No valid 'menuActiveAtStart' found. Needs to be boolean. Defaults to 'false'. "
        << "If 'true', the menu will be open at the start.";
    }

    confIt = config.find("consoleActiveAtStart");
    if (confIt != config.end() && confIt.value().is_boolean())
    {
      textActive = confIt.value().get<bool>();
    }
    else
    {
      LOG(WARNING) << "BltOverlay: No valid 'consoleActiveAtStart' found. Needs to be boolean. Defaults to 'false'. "
        << "If 'true', the console will be visible at start.";
    }

    confIt = config.find("fontConfigs");
    if (confIt != config.end() && confIt.value().is_object())
    {
      fontConfigs = confIt.value();
    }
    else
    {
      LOG(WARNING) << "BltOverlay: No valid 'fontConfigs' found. Needs to be an object with font configs. Using default fonts.";
    }

    confIt = config.find("keyConfig");
    if (confIt != config.end())
    {
      auto keyIt = confIt.value().find("menu");
      if (keyIt != confIt.value().end())
      {
        menuShortcut = keyIt.value();
      }
      else
      {
        LOG(WARNING) << "BltOverlay: 'keyConfig': No 'menu' found. Menu can not be opened or closed with the keyboard.";
      }

      keyIt = confIt.value().find("console");
      if (keyIt != confIt.value().end())
      {
        consoleShortcut = keyIt.value();
      }
      else
      {
        LOG(WARNING) << "BltOverlay: 'keyConfig': No 'console' found. Console can not be opened or closed with the keyboard.";
      }
    }
    else
    {
      LOG(WARNING) << "BltOverlay: No 'keyConfig' found. No keyboard control can be provided.";
    }
  }

  void BltOverlay::initialize()
  {
    bool issue{ false };

    if (overPtr)
    {
      issue = true;
      LOG(ERROR) << "BltOverlay needs to be singleton. Creating another BltOverlay is not allowed.";
    }
    overPtr = this;

    // load menuComponents
    if (!(issue || copyFunc::LoadPNGinCImage(getOwnModuleHandle(), menuComp, MAKEINTRESOURCE(MENU_COMPONENTS))))
    {
      issue = true;
      LOG(WARNING) << "BltOverlay was unable to load the menu components.";
    }
    
    auto addressResolver = getMod<AddressResolver>();
    if (!issue && addressResolver && addressResolver->requestAddresses(usedAddresses, *this))
    {
      // TODO(?): maybe some byte writer would be an idea... (but assembly to opcode would be too much/difficult I guess)
      // would need to construct byte array out of given values with types...

       // ddraw LoadLibrary -> load ddraw library, needs to be replaced with relative call + 1 nop
      unsigned char* libCall{ addressResolver->getAddressPointer<unsigned char>(Address::DD_LoadLibrary, *this) };
      int32_t* libAddr{ reinterpret_cast<int32_t*>(libCall + 1) };
      unsigned char* libNop{ reinterpret_cast<unsigned char*>(libCall + 5) };

      // call rel32 -> rel32 = absolute pointer - pos from where i write to - length of edited segment(call) (or absolute pointer - next address?)
      *libCall = 0xE8;
      *libAddr = reinterpret_cast<int32_t>(LoadLibraryFake) - reinterpret_cast<int32_t>(libCall) - 5;
      *libNop = 0x90;

      // first CreateSurface -> responsible for the main window, called when the app started, switched back to or the resolution changed
      unsigned char* surfCall{ addressResolver->getAddressPointer<unsigned char>(Address::DD_MainSurfaceCreate, *this) };
      int32_t* surfAddr{ reinterpret_cast<int32_t*>(surfCall + 1) };

      // call rel32 -> rel32 = absolute pointer - pos from where i write to - length of edited segment (or absolute pointer - next address?)
      *surfCall = 0xE8;
      *surfAddr = reinterpret_cast<int32_t>(CreateSurfaceFake) - reinterpret_cast<int32_t>(surfCall) - 5;

      // first found Flip
      unsigned char* flipMov = addressResolver->getAddressPointer<unsigned char>(Address::DD_MainFlip, *this);
      int32_t* flipAddr{ reinterpret_cast<int32_t*>(flipMov + 1) };

      // mov edx, func address
      *flipMov = 0xBA;
      *flipAddr = reinterpret_cast<int>(FlipFake);

      // initalize position values (only needs update on menu)
      menuActive = !menuActive;
      switchMenu(0, false, false);

      initialized = true;
    }

    // key register should not prevent initialization
    auto keyInterceptor = getMod<KeyboardInterceptor>();
    if (keyInterceptor && initialized)
    {
      if (!menuShortcut.empty())
      {
        std::function<void(const HWND, const bool, const bool)> func =
          [this](const HWND hw, const bool keyUp, const bool repeat) { this->switchMenu(hw, keyUp, repeat); };

        if (!keyInterceptor->registerFunction<true>(func, menuShortcut))
        {
          BltOverlay::sendToConsole("BltOverlay: Menu: At least one key combination was not registered.", el::Level::Warning);
        }
      }

      if (!consoleShortcut.empty())
      {
        std::function<void(const HWND, const bool, const bool)> func =
          [this](const HWND hw, const bool keyUp, const bool repeat) { this->switchConsole(hw, keyUp, repeat); };

        if (!keyInterceptor->registerFunction<true>(func, consoleShortcut))
        {
          BltOverlay::sendToConsole("BltOverlay: Console: At least one key combination was not registered.", el::Level::Warning);
        }
      }

      keyInterPtr = keyInterceptor;
      
      // test taking everyting away -> works? i guess
      //enableCharReceive(true);
    }

    initialized ? sendToConsole("BltOverlay initialized.", el::Level::Info) : LOG(WARNING) << "BltOverlay was not initialized.";
  }


  void BltOverlay::cleanUp()
  {
    menuComp.Destroy(); // release everything, for safety

    // no resource delete -> process end should free it anyway (it also prevents problems from not knowing if still valid)
    // offscreenSurf->Release();
  }


  void BltOverlay::registerDDrawLoadEvent(std::function<void()> &&func)
  {
    funcsForDDrawLoadEvent.push_back(func);
  }


  // keyboard stuff


  void BltOverlay::switchMenu(const HWND, const bool keyUp, const bool repeat)
  {
    if (keyUp || repeat)
    {
      return;
    }

    // adapt console position
    if (!menuIndicator)
    {
      textPos.first = menuActive ? menuRect.left : menuRect.right;
    }

    menuActive = !menuActive;
  }


  void BltOverlay::switchConsole(const HWND, const bool keyUp, const bool repeat)
  {
    if (keyUp || repeat)
    {
      return;
    }

    textActive = !textActive;
  }


  bool BltOverlay::controlMenu(const HWND window, const bool keyUp, const bool repeat, const VK key)
  {
    if (keyUp || repeat || !currentMenu)
    {
      return false;
    }

    auto keyIt{ menuActions.find(key) };
    if (keyIt == menuActions.end())
    {
      return false;
    }

    MenuBase* switchMenu{ currentMenu->executeAction(menuActions[key], *this) };
    if (switchMenu != nullptr)
    {
      currentMenu = switchMenu;
    }

    return true;
  }


  // intern functions //


  bool BltOverlay::createOffSurface(IDirectDrawSurface7** surf, DWORD width, DWORD height, DWORD fillColor)
  {
    DDSURFACEDESC2 offscreenSurfDes;
    ZeroDDObjectAndSetSize<DDSURFACEDESC2>(offscreenSurfDes);

    offscreenSurfDes.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

    // important: without compatibility mode, stronghold does not support Hardware rendering -> DDSCAPS_VIDEOMEMORY only in this case
    // -> could be done by flag in config -> this might be the case for other flags
    offscreenSurfDes.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    if (!disableHardwareTest)
    {
      offscreenSurfDes.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    }

    offscreenSurfDes.dwWidth = width;
    offscreenSurfDes.dwHeight = height;

    HRESULT res = dd7InterfacePtr->CreateSurface(&offscreenSurfDes, surf, NULL);
    if (res == DDERR_NODIRECTDRAWHW)
    {
      offscreenSurfDes.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
      res = dd7InterfacePtr->CreateSurface(&offscreenSurfDes, surf, NULL);

      disableHardwareTest = true;
      LOG(WARNING) << "No hardware support found for DirectDraw. Try compatibility mode for WinXP.";
    }

    if (res != S_OK)
    {
      LOG(ERROR) << "Failed to create offscreen surface.";
      return false;
    }

    // set keying color to black
    DDCOLORKEY ddck;
    ddck.dwColorSpaceLowValue = ddck.dwColorSpaceHighValue = 0;
    (*surf)->SetColorKey(DDCKEY_SRCBLT, &ddck);

    // dummy initial color fill
    if (fillColor != NULL)  // NULL would be black anyway
    {
      DDBLTFX fx;
      ZeroDDObjectAndSetSize<DDBLTFX>(fx);

      fx.dwFillColor = fillColor;

      res = (*surf)->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &fx);
    }

    return true;
  }


  void BltOverlay::releaseDDInterfaces()
  {
    if (dd7InterfacePtr != nullptr)
    {
      menuOffSurf->Release();
      textOffSurf->Release();
      inputOffSurf->Release();
      compSurf->Release();
      fntHandler.releaseSurfaces();

      if (!doNotKeepDD7Interface)
      {
        dd7InterfacePtr->Release();
      }
    }
  }


  void BltOverlay::prepareDDInterfaces(IDirectDraw7* const that, IDirectDrawSurface7* const dd7SurfacePtr)
  {
    HRESULT res; // need to clean up

    dd7InterfacePtr = that;
    if (!doNotKeepDD7Interface)
    {
      dd7InterfacePtr->AddRef();
    }

    DDSCAPS2 ddscapsBack;
    ZeroDDObject<DDSCAPS2>(ddscapsBack);
    ddscapsBack.dwCaps = DDSCAPS_BACKBUFFER;
    res = dd7SurfacePtr->GetAttachedSurface(&ddscapsBack, &dd7BackbufferPtr);
    if (res != DD_OK)
    {
      LOG(WARNING) << "BltOverlay: Did not get backbuffer. Overlay will not work.";
      dd7BackbufferPtr = nullptr;
    }

    bool mainSurfOk{ true };
    mainSurfOk = mainSurfOk && createOffSurface(&menuOffSurf, menuRect.right, menuRect.bottom, 0);
    mainSurfOk = mainSurfOk && createOffSurface(&textOffSurf, textRect.right, textRect.bottom, 0);
    // leaking here for test -> clipper (at least without config? not enough
    // IDirectDrawClipper* clip;
    // dd7InterfacePtr->CreateClipper(0, &clip, 0);
    // textOffSurf->SetClipper(clip);
    mainSurfOk = mainSurfOk && createOffSurface(&inputOffSurf, inputRect.right, inputRect.bottom, 0);
    if (!mainSurfOk)
    {
      LOG(ERROR) << "BltOverlay: At least one overlay surface could not get created. Removing backbuffer pointer to prevent crashes.";
      dd7BackbufferPtr = nullptr;
    }

    bool menuCompOk{ true };
    menuCompOk = mainSurfOk && createOffSurface(&compSurf, menuComp.GetWidth(), menuComp.GetHeight(), NULL);
    HDC surfDC{ nullptr };
    if (menuCompOk = mainSurfOk && compSurf->GetDC(&surfDC) == S_OK; menuCompOk)
    {
      menuCompOk = mainSurfOk && menuComp.BitBlt(surfDC, 0, 0, SRCCOPY);
      compSurf->ReleaseDC(surfDC);
    }
    
    if (!menuCompOk)
    {
      LOG(ERROR) << "BltOverlay: Creation of component surface caused problems.";
      dd7BackbufferPtr = nullptr;
    }
    
    // set input pos to middle
    DDSURFACEDESC2 surfaceInfos;
    ZeroDDObjectAndSetSize<DDSURFACEDESC2>(surfaceInfos);
    dd7SurfacePtr->GetSurfaceDesc(&surfaceInfos);

    inputPos.first = surfaceInfos.dwWidth / 2 - inputRect.right / 2;
    inputPos.second = surfaceInfos.dwHeight / 2 - inputRect.bottom / 2;

    // prepare font -> should write something more fitting with the code given later
    fntHandler.setUseVideoMemory(!disableHardwareTest);
    bool fontOk{ true };
    fontOk = fontOk && fntHandler.loadFont(FontTypeEnum::NORMAL, fontConfigs, dd7InterfacePtr, TEXTCOLOR(230, 230, 230));
    fontOk = fontOk && fntHandler.loadFont(FontTypeEnum::NORMAL_BOLD, fontConfigs, dd7InterfacePtr, TEXTCOLOR(230, 230, 150));
    fontOk = fontOk && fntHandler.loadFont(FontTypeEnum::SMALL, fontConfigs, dd7InterfacePtr, TEXTCOLOR(230, 230, 230));
    fontOk = fontOk && fntHandler.loadFont(FontTypeEnum::SMALL_BOLD, fontConfigs, dd7InterfacePtr, TEXTCOLOR(230, 230, 150));
    if (!fontOk)
    {
      LOG(WARNING) << "BltOverlay: At least one font failed to load.";
    }

    // should update console
    sendToConsole("BltOverlay: Finished surface prepare.", el::Level::Info);
  }


  void BltOverlay::bltMainDDOffSurfs()
  {
    if (textActive)
    {
      dd7BackbufferPtr->BltFast(textPos.first, textPos.second, textOffSurf, &textRect, DDBLTFAST_NOCOLORKEY);
    }
    
    if (menuActive)
    {
      dd7BackbufferPtr->BltFast(menuPos.first, menuPos.second, menuOffSurf, &menuRect, DDBLTFAST_NOCOLORKEY);
    }
    else if (menuIndicator)
    {
      dd7BackbufferPtr->BltFast(menuPos.first + 10, menuPos.second + 10, menuOffSurf, &menuSmallRect, DDBLTFAST_NOCOLORKEY);
    }

    if (inputActive)
    {
      dd7BackbufferPtr->BltFast(inputPos.first, inputPos.second, inputOffSurf, &inputRect, DDBLTFAST_SRCCOLORKEY);
    }
  }


  bool BltOverlay::sendToConsole(const std::string &msg)
  {
    return sendToConsole(msg, el::Level::Unknown);
  }


  bool BltOverlay::sendToConsole(const std::string &msg, el::Level logLevel)
  {
    switch (logLevel)
    {
      case el::Level::Error:
        LOG(ERROR) << msg;
        break;
      case el::Level::Warning:
        LOG(WARNING) << msg;
        break;
      case el::Level::Info:
        LOG(INFO) << msg;
        break;
      default:
        LOG(WARNING) << "BltOverlay: Requested not supported log level for 'sendToConsole'.";
        break;
    }

    // using already forced singelton -> starts to get pretty horrible...
    if (!overPtr)
    {
      return false;
    }

    BltOverlay& bltRef{ *overPtr };
    if (!bltRef.initialized)
    {
      return false;
    }

    if (bltRef.conQueue.size() >= 30)  // max currently hardcoded
    {
      bltRef.conQueue.pop_back();
    }
    bltRef.conQueue.push_front(msg);

    bltRef.updateConsole();

    return true;
  }


  void BltOverlay::updateConsole()
  {
    if (!dd7BackbufferPtr){
      // return if no backbuffer
      return;
    }

    // fill background (prepare DDBLTFX somewhere else?)
    DDBLTFX fx;
    ZeroDDObjectAndSetSize<DDBLTFX>(fx);
    fx.dwFillColor = RGB16(18, 18, 18);
    textOffSurf->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &fx);

    // write text
    // TODO: currently, blt text is not properly cut, instead, the position seems to be made valid (y from -something to 0),
    // but only a the remaining part is used... -> see if there is a solution? (maybe clipper?, maybe blt instead?)
    // error if position out of bounce: 0x88760096, also bltfast does not support clipper
    // TDOD: check error msg code
    const int32_t maxWidth{ textRect.right - textRect.left - 45 };
    int32_t remainingHeight{ textRect.bottom - textRect.top - 10};
    std::pair<int32_t, int32_t> lastValidPos{ textRect.left + 35, textRect.bottom - 10 };

    std::function<std::pair<int32_t, int32_t>(RECT)> posAdapt{
      [&remainingHeight, &lastValidPos](RECT rec) {
        remainingHeight -= rec.bottom;
        lastValidPos.second -= rec.bottom;
        return lastValidPos;
      }
    };

    for (auto& msg : conQueue)
    {
      if (remainingHeight < 0)
      {
        break;
      }
      fntHandler.drawText(textOffSurf, FontTypeEnum::SMALL, msg, 0, 0, maxWidth, false, false, false, &posAdapt);

      // only draw if it fits
      if (lastValidPos.second + 12 > 0) // + height of icon
      {
        if (lastValidPos.second < 0)  // cut it (since I do not know if I can have the hardware do it...)
        {
          RECT otherRect{ menuRects.msgIcon };
          otherRect.top -= lastValidPos.second;
          textOffSurf->BltFast(14, 0, compSurf, &otherRect, DDBLTFAST_SRCCOLORKEY);
        }
        else
        {
          textOffSurf->BltFast(14, lastValidPos.second, compSurf, &menuRects.msgIcon, DDBLTFAST_SRCCOLORKEY);
        }
      }
    }

    // add border
    textOffSurf->BltFast(0, 0, compSurf, &menuRects.consoleBorder, DDBLTFAST_SRCCOLORKEY);
  }

  bool BltOverlay::enableCharReceive(bool enable)
  {
    auto interceptor = keyInterPtr.lock();
    if (!interceptor)
    {
      return false;
    }

    std::function<void(char)> func{
      [this](char chr)
      { 
        this->receiveChar(chr); 
      }
    };
    std::function<bool(const HWND, const bool, const bool, const VK)> passage{
      [this](const HWND window, const bool keyUp, const bool repeat, const VK key)
      {
        return this->controlMenu(window, keyUp, repeat, key);
      }
    };
    return enable ? interceptor->lockChars(this, func, passage) : interceptor->freeChars(this);
  }

  void BltOverlay::receiveChar(char chr)
  {
    std::string s{ chr };
    if (currentMenu)
    {
      currentMenu->executeAction(chr, *this);
    }
  }

  std::vector<AddressRequest> BltOverlay::usedAddresses{
    {Address::DD_MainSurfaceCreate, {{Version::NONE, 5}}, true, AddressRisk::CRITICAL},
    {Address::DD_MainFlip, {{Version::NONE, 5}}, true, AddressRisk::CRITICAL},
    {Address::DD_LoadLibrary, {{Version::NONE, 6}}, true, AddressRisk::CRITICAL}
  };

  // other createSurface calls (extreme)(not all, but I failed to get past the requests):
  // 0046FF51
  // 00470007 (mov)

  // codes called when hovering over create Szenario, that access the text:
  // maybe it will be possible to one day use the text drawing functions of Stronghold
  // however, for now, i will use a simple for my own
  // 0x46A3B0
  // 0x473150
  // 0x469D04
  // 0x469DDE
  // 0x469E10

  /*******************************************************/
  // Unused function safe (+ notes):
  /*******************************************************/
  /* comment start

  // way to create easier (old school) function pointers
  using DDCreate = HRESULT(_stdcall *)(GUID*, LPDIRECTDRAW*, IUnknown*);
  using DDCreateEx = HRESULT(_stdcall *)(GUID*, LPVOID*, REFIID, IUnknown*);

  static DDCreate ddCreate;
  static DDCreateEx ddCreateEx;

  static HRESULT _stdcall DirectDrawCreateFake(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuter)
  {
    return ddCreate(lpGUID, lplpDD, pUnkOuter);
  }

  // crusader seems to use CreateEx
  static HRESULT _stdcall DirectDrawCreateExFake(GUID * lpGuid, LPVOID *lplpDD, REFIID iid, IUnknown *pUnkOuter)
  {
    HRESULT res{ ddCreateEx(lpGuid, lplpDD, iid, pUnkOuter) };

    return res;
  }

  // getProcAddress replace -> not needed for process
  // address for Extreme 1.41.1-E: 0x0046F727 (rel: 0x6F727)
  // would need to be replaced with 0xBF + this func address + 0x90 (mov in edi(?) + addr + nop)
  // this function would redirect the first create DirectDraw object calls to the custom create functions
  // however, the there created seem to not be used for the actual game
  static FARPROC _stdcall GetProcAddressFake(HMODULE hmod, LPCSTR str)
  {
    std::string fkt{ str };
    if (fkt == "DirectDrawCreate")
    {
      ddCreate = (DDCreate)GetProcAddress(hmod, str);
      return (FARPROC)DirectDrawCreateFake;
    }
    else if (fkt == "DirectDrawCreateEx")
    {
      ddCreateEx = (DDCreateEx)GetProcAddress(hmod, str);
      return (FARPROC)DirectDrawCreateExFake;
    }

    return GetProcAddress(hmod, str);
  }

  comment end */
}