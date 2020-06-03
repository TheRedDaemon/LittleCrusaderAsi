
// all modTypes include (will (hoefully?) be many one day)
#include "../modclasses/addressBase.h" 
#include "../Modclasses/versionGetter.h"
//#include ...

// dummy:
#include "../modclasses/dummyStuff/dummyMods.h"

#include <string>

#include "modLoader.h"

namespace modcore
{
  namespace MC = modclasses;

  const std::shared_ptr<Mod> ModLoader::createMod(MT modType, const Json &config)
  {
    // using giant switch statement
    switch (modType)
    {
      case MT::ADDRESS_BASE:
        return std::make_shared<MC::AddressBase>();

      case MT::VERSION_GET:
      case MT::ADDRESS_RESOLVER:
      case MT::KEYBOARD_HANDLER:
      case MT::EVENT_HANDLER:


      // dummy:
      case MT::TEST1:
        return std::make_shared<MC::Test1>();
      case MT::TEST2:
        return std::make_shared<MC::Test2>();

      default:
        throw std::exception(("Received a not handled ModType with id: " + std::to_string(static_cast<int>(modType))).c_str());
    }
  }
}