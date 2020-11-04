//#pragma once
#ifndef MISC_1
#define MISC_1

#include <string>
#include <Windows.h>

#include "atlimage.h"

namespace copyFunc
{
  std::basic_string<TCHAR> better_get_module_filename(HMODULE hModule);

  IStream* CreateStreamOnResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);

  bool LoadPNGinCImage(HMODULE hModule, CImage &img, const char* resourceName);
}

#endif //MISC_1