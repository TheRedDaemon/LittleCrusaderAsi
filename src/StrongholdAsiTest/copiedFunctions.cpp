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
}