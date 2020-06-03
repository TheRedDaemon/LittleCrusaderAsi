// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
//#include "pch.h"

#include <Windows.h>
#include <chrono>
#include <filesystem>

#include "modLoader.h"
#include "logUtility.h"
#include "../misc/copiedFunc.h"

//logging
#include "../dependencies/logger/easylogging++.h"
INITIALIZE_EASYLOGGINGPP

// add at least log-level

// mod loader (as far as I understand, dlls may have issues with some variable scopes, init etc.... so, hopefully everything works)
static std::unique_ptr<modcore::ModLoader> modLoader;

static void initiateLogger()
{
  // very basic config currently
  el::Configurations conf("logger.config");
  el::Loggers::reconfigureLogger("default", conf);
  LOG(INFO) << "Logger loaded.";
}

// if an exception falls through, copy and safe the 
static void copyLogOnError()
{
  el::Loggers::flushAll();  // flush

  // get default name
  std::string filename{ el::Loggers::getLogger("default")->typedConfigurations()->filename(el::Level::Global)};

  // get time
  struct tm newtime;
  time_t now = time(0);
  localtime_s(&newtime, &now);
  std::stringstream timeStream;
  timeStream << std::put_time(&newtime, "%Y-%m-%d-%H.%M.%S");

  // copy file
  try
  {
    std::filesystem::copy_file("plugins/" + filename, "plugins/" + timeStream.str() + "_error_" + filename);
  }
  catch (std::exception&)
  {
    
    // NOTE: if it breaks during int, something breaks
    // this is only a workaorund
    std::string path{ copyFunc::better_get_module_filename(NULL) };
    path.erase(path.find_last_of("\\", path.length()), path.length());
    
    try
    {
      std::filesystem::copy_file(path + "/plugins/" + filename, path + "/plugins/" + timeStream.str() + "_error_" + filename);
    }
    catch (std::exception& o_O)
    {
      LOG(ERROR) << o_O.what() << "Not even additional handling allowed copy...";
    }
  }
}

// dll start
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
)
{

  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:  // case doesn't open a new scope alone, it needs brackets
    {
      DisableThreadLibraryCalls(hModule); // normally there are thread attaches/detaches, this removes them

      // load logger (hoping nothing breaks earlier...)
      initiateLogger();

      // load stuff
      // NOTE: all preparation might be useless, if the memory scope of stronghold is not loaded at this moment -> would need differnt approach
      // would make everything useless, actually
      try
      {
        modLoader = std::make_unique<modcore::ModLoader>();
      }
      catch (std::exception& o_O)
      {
        LOG(ERROR) << "An exception has occured: " << o_O.what() << " (The application might be left unstable.)";
        modcore::enableErrorLog();
      }
      catch (...)
      {
        // do some logging
        LOG(ERROR) << "An unexpected exception has occured. (The application might be left unstable.)";
        modcore::enableErrorLog();
      }
      break;
    }

    case DLL_PROCESS_DETACH:
    {
      try
      {
        // ending stuff on detach
        modLoader.reset();

        // it ending save log extra if error log was called
        if (modcore::copyErrorLog())
        {
          copyLogOnError();
        }
      }
      catch (std::exception& o_O)
      {
        LOG(ERROR) << "An exception has occured: " << o_O.what() << " (The application might be left unstable.)";
        copyLogOnError();
      }
      catch (...)
      {
        // do some logging
        LOG(ERROR) << "An unexpected exception has occured. (The application might be left unstable.)";
        copyLogOnError();
      }

      break;
    }
  }
  return TRUE;
}