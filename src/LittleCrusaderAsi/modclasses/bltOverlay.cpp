
#include "bltOverlay.h"

#include "ddraw.h"

namespace modclasses
  // at 0046F710 both adderesses for directDraw are requested it seems... -> 0046F727 mov edi, [pointer zu GetProcAddress ] ?
  // address createEx: ab 0046F710
{ // address create call: 0046FCB8? (mulitple?)
  // address for create jump pointer: 0057D3D4
  // using ugly function pointers for tests
  using DDCreate = HRESULT(_stdcall *)(GUID*, LPDIRECTDRAW*, IUnknown*);
  using DDCreateEx = HRESULT(_stdcall *)(GUID*, LPVOID*, REFIID, IUnknown*);
  using ProcFake = FARPROC(_stdcall *)(HMODULE, LPCSTR);
  using DD7CreateSurface = HRESULT(_stdcall IDirectDraw7::*)(LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE7*, IUnknown*);

  static DDCreate ddCreate;
  static DDCreateEx ddCreateEx;
  static IDirectDraw7* dd7InterfacePtr;

  static HRESULT _stdcall DirectDrawCreateFake(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuter)
  {
    return ddCreate(lpGUID, lplpDD, pUnkOuter);
  }

  // stronghold seems to use CreateEx; test: asi + 1FC680
  // seems to be called: dxwrapper.dll + BC170 (eigentliches object)
  // possible CreateSurface Fkt: dxwrapper.dll + B3650
  // Extreme call to CreateSurface: 0046FEB8 
  // place for insert: 0046FEB5
  static HRESULT _stdcall DirectDrawCreateExFake(GUID * lpGuid, LPVOID *lplpDD, REFIID iid, IUnknown *pUnkOuter)
  {
    HRESULT res{ ddCreateEx(lpGuid, lplpDD, iid, pUnkOuter) };

    if (res == S_OK)
    {
      // get interface ptr... hopefully (and hopefully this is still used)
      dd7InterfacePtr = (IDirectDraw7*) *lplpDD;

      // trying to find position of CreateSurface by calling it
     // DDSURFACEDESC2 ddsd;
      //ZeroMemory(&ddsd, sizeof(ddsd));  // empty
      //ddsd.dwSize = sizeof(ddsd);

      //IDirectDrawSurface7* surfPtr;
      //dd7InterfacePtr->CreateSurface(&ddsd, &surfPtr, 0);

      // now need to test where the Create Surface Functions are called
      // need flip (if it is used) and the BackbufferSurface -> not defined yet, can not do this here
      // maybe I can get the BackbufferSurface in the first flip?
      // currently, I need to get the flip position to intercept it, the other way would be to create another impl of the Objects and return that

      // small test -> try to get pointer to flip
      //int b;
      //HRESULT o = dd7InterfacePtr->GetVerticalBlankStatus(&b);

      
      //if (o == S_OK)
      //{
      //  LOG(INFO) << "Did not return non-sense.";
      //}
    }

    return res;
  }
  
  // NOTE: -> extern functions seem to work pretty good... however...copy them nicely
  // maybe there are some stuff like _stdcall etc that might help me?
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


  // IDirectDraw7 ptr is invalid once it reaches this position -> that might be a result of the wrapper... test without
  // -> important, I do not know if one can use surfaces from different IDirectDraw7 objects
  // -> problem could be, that I always loose the objects -> recreating seems to be a problem...
  // -> maybe I can get a handle of an object and keep it alive somehow -> first find flip
  static HRESULT _stdcall CreateSurfaceFake(IDirectDraw7* const that, LPDDSURFACEDESC2 desc2, LPDIRECTDRAWSURFACE7* surf7, IUnknown* unkn)
  {
    HRESULT res{ that->CreateSurface(desc2, surf7, unkn) };

    if (res == S_OK)
    {
      dd7InterfacePtr = that;
      LOG(INFO) << "Got first create.";
    }

    return res;
  }

  // if no luck with create surface -> creating a surface and calling flip could be an option to find the code

  /*******************************************************/
  // Non-static part starts here:
  /*******************************************************/

  BltOverlay::BltOverlay(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
  }

  void BltOverlay::initialize()
  {

    // getProcAddress
    // clean up?
    DWORD oldAddressProtection{ 0 };
    if (!VirtualProtect(reinterpret_cast<DWORD*>(0x0046F727), 6, PAGE_EXECUTE_READWRITE, &oldAddressProtection))
    {
      LOG(WARNING) << "Couldn't allow write access of memory. Error code: " << GetLastError();
    }

    // maybe need some byte writer -> works for extreme
    // 
    char* mov = reinterpret_cast<char*>(0x0046F727);
    int* to = reinterpret_cast<int*>(0x0046F727 + 1);
    char* nop = reinterpret_cast<char*>(0x0046F727 + 5);

    *mov = 0xBF;
    *to = reinterpret_cast<int>(GetProcAddressFake);
    *nop = 0x90;

    // first CreateSurface
    if (!VirtualProtect(reinterpret_cast<DWORD*>(0x0046FEB5), 5, PAGE_EXECUTE_READWRITE, &oldAddressProtection))
    {
      LOG(WARNING) << "Couldn't allow write access of memory. Error code: " << GetLastError();
    }

    // maybe need some byte writer -> works for extreme
    // call is always relative -> absolute pointer - pos from where i write to - length of edited segment (or absolute pointer - next address?)

    char* call = reinterpret_cast<char*>(0x0046FEB5);
    to = reinterpret_cast<int*>(0x0046FEB5 + 1);

    *call = 0xE8;
    *to = reinterpret_cast<int>(CreateSurfaceFake) - 0x0046FEB5 - 5;
  }
}
