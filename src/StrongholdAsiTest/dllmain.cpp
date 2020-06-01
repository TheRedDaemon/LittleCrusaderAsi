// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
//#include "pch.h"

#include <Windows.h>

#include "modLoader.h"

// mod loader (as far as I understand, dlls may have issues with some variable scopes, init etc.... so, hopefully everything works)
static std::unique_ptr<modcore::ModLoader> modLoader;

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
)
{

  try
  {

    switch (ul_reason_for_call)
    {
      case DLL_PROCESS_ATTACH:  // case doesn't open a new scope alone, it needs brackets
      {
        DisableThreadLibraryCalls(hModule); // normally there are thread attaches/detaches, this removes them

        // load stuff
        // NOTE: all preparation might be useless, if the memory scope of stronghold is not loaded at this moment -> would need differnt approach
        // would make everything useless, actually
        modLoader = std::make_unique<modcore::ModLoader>();

        break;
      }

      case DLL_PROCESS_DETACH:
      {

        // ending stuff on detach
        modLoader.reset();

        break;
      }
    }
    return TRUE;


  }
  catch (std::exception& ex)
  {
    // do logging
  }
  catch (...)
  {
    // do some logging
  }
}