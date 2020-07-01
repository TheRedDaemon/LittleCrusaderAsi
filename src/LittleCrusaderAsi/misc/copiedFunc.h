//#pragma once
#ifndef MISC_1
#define MISC_1

#include <string>
#include <Windows.h>

namespace copyFunc
{
  std::basic_string<TCHAR> better_get_module_filename(HMODULE hModule);
}

#endif //MISC_1