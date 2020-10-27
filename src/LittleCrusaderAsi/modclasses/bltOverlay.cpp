
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

  static DDCreate ddCreate;
  static DDCreateEx ddCreateEx;
  static IDirectDraw7* dd7InterfacePtr;

  static HRESULT _stdcall DirectDrawCreateFake(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuter)
  {
    return ddCreate(lpGUID, lplpDD, pUnkOuter);
  }

  // stronghold seems to use CreateEx;
  static HRESULT _stdcall DirectDrawCreateExFake(GUID * lpGuid, LPVOID *lplpDD, REFIID iid, IUnknown *pUnkOuter)
  {
    HRESULT res{ ddCreateEx(lpGuid, lplpDD, iid, pUnkOuter) };

    if (res == S_OK)
    {
      // get interface ptr... hopefully (and hopefully this is still used)
      dd7InterfacePtr = (IDirectDraw7*) *lplpDD;

      // now need to test where the Create Surface Functions are called
      // need flip (if it is used) and the BackbufferSurface -> not defined yet, can not do this here
      // maybe I can get the BackbufferSurface in the first flip?

      // small test -> return non-sense sadly, need further tests -> try to get pointer to flip
      int b;
      HRESULT o = dd7InterfacePtr->GetVerticalBlankStatus(&b);
      
      if (o == S_OK)
      {
        LOG(INFO) << "Did not return non-sense.";
      }
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

  /*******************************************************/
  // Non-static part starts here:
  /*******************************************************/

  BltOverlay::BltOverlay(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
  }

  void BltOverlay::initialize()
  {

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
  }
}
