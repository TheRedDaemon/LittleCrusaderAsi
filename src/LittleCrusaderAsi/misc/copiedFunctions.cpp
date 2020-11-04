//#include "pch.h"

#include "copiedFunc.h"

namespace copyFunc  // containes smaller functions simply copied from other places
{
  // reads the name of a module
  // source: https://stackoverflow.com/a/38352793
  std::basic_string<TCHAR> better_get_module_filename(HMODULE hModule)
  {
    std::basic_string<TCHAR> result(128, 0);
    DWORD err = ERROR_SUCCESS;
    do
    {
      auto actual = GetModuleFileName(hModule, std::addressof(result[0]), result.size());
      err = GetLastError();
      if (actual == 0)
      {
        throw "Couldn't recieve exe name.";
      }
      result.resize(actual);
    }
    while (err == ERROR_INSUFFICIENT_BUFFER);
    return result;
  }

  // create a resource stream
  // source: https://stackoverflow.com/q/45441475
  IStream* CreateStreamOnResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType)
  {
    IStream * ipStream{ NULL };
    DWORD dwResourceSize{ NULL };
    HGLOBAL hglbImage{ NULL };
    LPVOID pvSourceResourceData{ NULL };
    HGLOBAL hgblResourceData{ NULL };
    LPVOID pvResourceData{ NULL };

    HRSRC hrsrc = FindResource(hModule, lpName, lpType);
    if (hrsrc == NULL)
      goto Return;

    dwResourceSize = SizeofResource(hModule, hrsrc);
    hglbImage = LoadResource(hModule, hrsrc);
    if (hglbImage == NULL)
      goto Return;

    pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == NULL)
      goto Return;

    hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == NULL)
      goto Return;

    pvResourceData = GlobalLock(hgblResourceData);

    if (pvResourceData == NULL)
      goto FreeData;

    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);

    GlobalUnlock(hgblResourceData);

    if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
      goto Return;

  FreeData:
    GlobalFree(hgblResourceData);

  Return:
    return ipStream;
  }

  // func to fill CImage
  // source: https://stackoverflow.com/q/45441475
  bool LoadPNGinCImage(HMODULE hModule, CImage &img, const char* resourceName)
  {
    IStream *pStream = CreateStreamOnResource(hModule, resourceName, _T("PNG"));
    if (pStream != nullptr)
    {
      img.Load(pStream);
      img.SetHasAlphaChannel(true);
      pStream->Release();
      return true;
    }
    return false;
  }
}