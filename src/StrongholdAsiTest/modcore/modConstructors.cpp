
// all modTypes include (will (hopefully?) be many one day)
#include "../modclasses/addressBase.h" 
#include "../Modclasses/versionGetter.h"
#include "../Modclasses/addressResolver.h"
#include "../modclasses/keyboardInterceptor.h"
#include "../modclasses/buildRangeChanger.h"
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
        return std::make_shared<MC::VersionGetter>();
      case MT::ADDRESS_RESOLVER:
        return std::make_shared<MC::AddressResolver>(config);
      case MT::KEYBOARD_INTERCEPTOR:
        return std::make_shared<MC::KeyboardInterceptor>(config);
      case MT::BUILD_RANGE_CHANGER:
        return std::make_shared<MC::BuildRangeChanger>(config);

      // dummy:
      case MT::TEST1:
        return std::make_shared<MC::Test1>();
      case MT::TEST2:
        return std::make_shared<MC::Test2>();

      case MT::EVENT_HANDLER:
      default:
        throw std::exception(("Received a not handled ModType with id: " + std::to_string(static_cast<int>(modType))).c_str());
    }
  }
}