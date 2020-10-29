
#include "bltOverlay.h"

#include <initguid.h>
#include "ddraw.h"

// main source ddraw: https://www.codeproject.com/Articles/2370/Introduction-to-DirectDraw-and-Surface-Blitting
// another: http://www.visualbasicworld.de/tutorial-directx-directdraw.html
namespace modclasses
{
  static IDirectDraw7* dd7InterfacePtr;
  static IDirectDrawSurface7* dd7SurfacePtr;
  static IDirectDrawSurface7* dd7BackbufferPtr;

  // this might need far more testing -> dd to d9 might create far other issues
  static bool doNotKeepDD7Interface = true; // for d7tod9 -> removing the references removes handling window
  // in case of non compatible mode Crusader (XP), VideoMemory creates errors
  // if true, there will be no test performed again
  static bool disableHardwareTest = false;

  // test
  static IDirectDrawSurface7* offscreenSurf;
  
  // NOTE: -> linking to static functions seem to work pretty good -> the copy needs to be nice though (in first tests, _stdcall was missing)
  // maybe there are some stuff like _stdcall etc that might help me?


  // IDirectDraw7 ptr is invalid once it reaches this position
  // -> old surfaces sometimes are lost, sometimes they work
  // -> depends on asiLoader (and its config)
  // test with different main dd7 object -> did not get it to work
  // it returned ok, but nothing was drawn on screen
  static HRESULT _stdcall CreateSurfaceFake(IDirectDraw7* const that, LPDDSURFACEDESC2 desc2, LPDIRECTDRAWSURFACE7* surf7, IUnknown* unkn)
  {
    // keep control over pointer
    if (dd7InterfacePtr != nullptr)
    {
      offscreenSurf->Release();

      if (!doNotKeepDD7Interface)
      {
        dd7InterfacePtr->Release();
      }
    }

    HRESULT frontSurfRes{ that->CreateSurface(desc2, surf7, unkn) };

    if (frontSurfRes == S_OK)
    {
      HRESULT res; // need to clean up

      dd7InterfacePtr = that;
      if (!doNotKeepDD7Interface)
      {
        dd7InterfacePtr->AddRef();
      }

      dd7SurfacePtr = *surf7;

      DDSCAPS2 ddscapsBack;
      ZeroMemory(&ddscapsBack, sizeof(ddscapsBack));
      ddscapsBack.dwCaps = DDSCAPS_BACKBUFFER;
      res = dd7SurfacePtr->GetAttachedSurface(&ddscapsBack, &dd7BackbufferPtr);
      if (res == DD_OK)
      {
        LOG(INFO) << "Got backbuffer.";
      }

      //dd7SurfacePtr->Flip(NULL, 0); // asi + 2E701C
      // wrapper+BB6F0
      // dxwrapper.dll+AAAF0 (flip function)

      // write to buffer test, create surface:

      DDSURFACEDESC2 offscreenSurfDes;
      ZeroMemory(&offscreenSurfDes, sizeof(offscreenSurfDes));

      offscreenSurfDes.dwSize = sizeof(offscreenSurfDes);
      offscreenSurfDes.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
      
      // important: without compatibility mode, stronghold does not support Hardware rendering -> DDSCAPS_VIDEOMEMORY only in this case
      // -> could be done by flag in config -> this might be the case for other flags
      offscreenSurfDes.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
      if (!disableHardwareTest)
      {
        offscreenSurfDes.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
      }

      offscreenSurfDes.dwWidth = 100;
      offscreenSurfDes.dwHeight = 100;

      res = dd7InterfacePtr->CreateSurface(&offscreenSurfDes, &offscreenSurf, NULL);
      if (res == DDERR_NODIRECTDRAWHW)
      {
        offscreenSurfDes.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        res = dd7InterfacePtr->CreateSurface(&offscreenSurfDes, &offscreenSurf, NULL);
        
        disableHardwareTest = true;
        LOG(WARNING) << "No hardware support found for DirectDraw. Try compatibility mode for WinXP.";
        if (res != S_OK)
        {
          LOG(ERROR) << "Failed to create offscreen surface.";
        }
      }


      LOG(INFO) << "Got first create.";
    }

    return frontSurfRes;
  }


  static int testAdd{ 0 };


  // position flip: 0047064D (not easily redirectable)
  // maybe here: 00470645 write address of flip function into edx
  // flip is called at a relative constant time -> no menu responsiveness issues
  static HRESULT _stdcall FlipFake(IDirectDrawSurface7* const that, LPDIRECTDRAWSURFACE7 surf7, DWORD flags)
  {
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = 100 + 0; // seems to use exclusive
    rect.bottom = 100 + 0;

    HRESULT bltRes = dd7BackbufferPtr->BltFast(300 + testAdd, 300, offscreenSurf, &rect, DDBLTFAST_NOCOLORKEY);
    ++testAdd;
    if (testAdd > 100)
    {
      testAdd = 0;
    }

    HRESULT res{ that->Flip(surf7, flags) };
    if (res == S_OK)
    {
      // LOG(INFO) << "FLIP."; // <--- I hope this spams -> it spams
      if (that != dd7SurfacePtr)
      {
        LOG(INFO) << "surface changed"; // if the surface is the same, so will be the ddObject...I hope
      }
    }
    return res;
  }


  /*******************************************************/
  // Non-static part starts here:
  /*******************************************************/


  BltOverlay::BltOverlay(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
  }

  void BltOverlay::initialize()
  {
    auto addressResolver = getMod<AddressResolver>();

    if (addressResolver)
    {
      // request addresses
      if (addressResolver->requestAddresses(usedAddresses, *this))
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
    }

    initialized ? LOG(INFO) << "BltOverlay initialized." : LOG(WARNING) << "BltOverlay was not initialized.";
  }


  void BltOverlay::cleanUp()
  {
    // no resource delete -> process end should free it anyway (it also prevents problems from not knowing if still valid)
    // offscreenSurf->Release();
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