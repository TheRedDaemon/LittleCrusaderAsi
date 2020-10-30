
#include "bltOverlay.h"

// main source ddraw: https://www.codeproject.com/Articles/2370/Introduction-to-DirectDraw-and-Surface-Blitting
// another: http://www.visualbasicworld.de/tutorial-directx-directdraw.html
namespace modclasses
{
  // amcro found online, source: https://gamedev.net/forums/topic/289485-16-bit-color-on-directdraw-surfaces/2825618/
  #define RGB16(r,g,b) ((b>>3)|((g>>2)<<5)|((r>>3)<<11))

  // pointer for static functions
  static BltOverlay *overPtr{ nullptr };
  

  // NOTE: -> linking to static functions seem to work pretty good -> the copy needs to be nice though (in first tests, _stdcall was missing)
  // maybe there are some stuff like _stdcall etc that might help me?


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
    overPtr->bltMainDDOffSurf();

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
    
    auto addressResolver = getMod<AddressResolver>();
    if (!issue && addressResolver && addressResolver->requestAddresses(usedAddresses, *this))
    {
      // TODO(?): maybe some byte writer would be an idea... (but assembly to opcode would be too much/difficult I guess)
      // would need to construct byte array out of given values with types...

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

      initialized = true;
    }

    initialized ? LOG(INFO) << "BltOverlay initialized." : LOG(WARNING) << "BltOverlay was not initialized.";
  }


  void BltOverlay::cleanUp()
  {
    // no resource delete -> process end should free it anyway (it also prevents problems from not knowing if still valid)
    // offscreenSurf->Release();
  }


  // intern functions //


  void BltOverlay::createOffSurface(IDirectDrawSurface7** surf, DWORD width, DWORD height, DWORD fillColor)
  {
    DDSURFACEDESC2 offscreenSurfDes;
    zeroDDObjectAndSetSize<DDSURFACEDESC2>(offscreenSurfDes);

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

    // test
    //offscreenSurfDes.dwFlags |= DDSD_PIXELFORMAT;
    //zeroDDObjectAndSetSize(offscreenSurfDes.ddpfPixelFormat);
    //offscreenSurfDes.ddpfPixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_ALPHAPREMULT | DDPF_PALETTEINDEXED8 | DDPF_RGB;
    //offscreenSurfDes.ddpfPixelFormat.dwRGBBitCount = 32;

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
      return;
    }

    // dummy initial color fill
    DDBLTFX fx;
    zeroDDObjectAndSetSize<DDBLTFX>(fx);

    fx.dwFillColor = fillColor;

    res = (*surf)->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &fx);
  }


  void BltOverlay::releaseDDInterfaces()
  {
    if (dd7InterfacePtr != nullptr)
    {
      menuOffSurf->Release();
      textOffSurf->Release();
      inputOffSurf->Release();

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
    zeroDDObject<DDSCAPS2>(ddscapsBack);
    ddscapsBack.dwCaps = DDSCAPS_BACKBUFFER;
    res = dd7SurfacePtr->GetAttachedSurface(&ddscapsBack, &dd7BackbufferPtr);
    if (res == DD_OK)
    {
      LOG(INFO) << "Got backbuffer.";
    }

    createOffSurface(&menuOffSurf, menuRect.right, menuRect.bottom, RGB16(0,0,255));
    createOffSurface(&textOffSurf, textRect.right, textRect.bottom, RGB16(255, 0, 0));
    createOffSurface(&inputOffSurf, inputRect.right, inputRect.bottom, RGB16(0, 255, 0));
    
    // set input pos to middle
    DDSURFACEDESC2 surfaceInfos;
    zeroDDObjectAndSetSize<DDSURFACEDESC2>(surfaceInfos);
    dd7SurfacePtr->GetSurfaceDesc(&surfaceInfos);

    /*
    DDPIXELFORMAT px;
    zeroDDObjectAndSetSize(px);
    menuOffSurf->GetPixelFormat(&px);
    dd7SurfacePtr->GetPixelFormat(&px);
    */

    inputPos.first = surfaceInfos.dwWidth / 2 - inputRect.right / 2;
    inputPos.second = surfaceInfos.dwHeight / 2 - inputRect.bottom / 2;

    LOG(INFO) << "Finished surface prepare.";
  }


  void BltOverlay::bltMainDDOffSurf()
  {
    /*
    // test
    static int testAdd{ 0 };

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = 100 + 0; // seems to use exclusive
    rect.bottom = 100 + 0;

    HRESULT bltRes = dd7BackbufferPtr->BltFast(300 + testAdd, 300, mainOffSurf, &rect, DDBLTFAST_NOCOLORKEY);
    ++testAdd;
    if (testAdd > 100)
    {
      testAdd = 0;
    }
    */

    // NOTE: normal blt can auto transform the size to fit
    dd7BackbufferPtr->BltFast(menuPos.first, menuPos.second, menuOffSurf, &menuRect, DDBLTFAST_NOCOLORKEY);
    dd7BackbufferPtr->BltFast(textPos.first, textPos.second, textOffSurf, &textRect, DDBLTFAST_NOCOLORKEY);
    dd7BackbufferPtr->BltFast(inputPos.first, inputPos.second, inputOffSurf, &inputRect, DDBLTFAST_NOCOLORKEY);
  }


  std::vector<AddressRequest> BltOverlay::usedAddresses{
    {Address::DD_MainSurfaceCreate, {{Version::NONE, 5}}, true, AddressRisk::CRITICAL},
    {Address::DD_MainFlip, {{Version::NONE, 5}}, true, AddressRisk::CRITICAL}
  };

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