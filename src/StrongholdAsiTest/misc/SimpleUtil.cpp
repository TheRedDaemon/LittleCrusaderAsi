//#include "pch.h"

#include "SimpleUtil.h"

namespace simpleUtil
{

  bool isStrongholdExtreme()
  {
    std::string path{ copyFunc::better_get_module_filename(NULL) };

    path.erase(0, path.find_last_of("\\", path.length()) + 1);
    
    if (path.find("Extreme") != std::string::npos)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}