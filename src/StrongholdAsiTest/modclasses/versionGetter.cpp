
#include "versionGetter.h"

namespace modclasses
{
  std::unique_ptr<std::unordered_map<ModType, std::unique_ptr<DependencyRecContainer>>> VersionGetter::neededDependencies()
  {
    auto mapPointer = std::make_unique<std::unordered_map<ModType, std::unique_ptr<DependencyRecContainer>>>();
    mapPointer->try_emplace(ModType::ADDRESS_BASE, std::make_unique<DependencyReceiver<AddressBase>>(&addrBase));
    return mapPointer;
  }

  // version helper
  static bool isThisVersion(
    unsigned char *versionString, signed char *versionNumber,
    std::string expVersionString, signed char expVersionNumber)
  {
    bool stringEqual{ true };
    
    size_t i = 0;
    do
    {
      stringEqual = stringEqual && expVersionString[i] == *(versionString + i);
      ++i;
    }
    while (stringEqual && i < expVersionString.size());

    return stringEqual && expVersionNumber == *versionNumber;
  }

  bool VersionGetter::initialize()
  {
    auto addressBaseMod = getIfModInit<AddressBase>(addrBase);
    
    if (addressBaseMod)
    {
      // version elif
      // extreme41
      if (isThisVersion(
        (unsigned char*)(addressBaseMod->getBaseAddress() + relExtremeVersion41String),
        (signed char*)(addressBaseMod->getBaseAddress() + relExtremeVersion41Number),
        "V1.%d.1-E", 41))
      {
        version = Version::V1P41P1SE;
        LOG(INFO) << "Detected Stronghold Extreme 1.41.1-E.";
      }
      // 41
      else if (isThisVersion(
        (unsigned char*)(addressBaseMod->getBaseAddress() + relVersion41String),
        (signed char*)(addressBaseMod->getBaseAddress() + relVersion41Number),
        "V1.%d", 41))
      {
        version = Version::V1P41;
        LOG(INFO) << "Detected Stronghold 1.41.";
      }
    }

    if (version == Version::NONE)
    {
      LOG(WARNING) << "VersionGetter was not initialized.";
    }
    else
    {
      initialized = true;
      LOG(INFO) << "VersionGetter initialized.";
    }

    return initialized;
  }
  
  // NONE will be extreme 41
  // also safe for now, since this runs before all other memory accesses and only reads,
  // but who knows for what the version string could be used...
  std::vector<AddressRequest> VersionGetter::usedAddresses{
    {Address::VERSION_STRING, {{Version::NONE, 9}, {Version::V1P41, 5}}, false, AddressRisk::SAFE},
    {Address::VERSION_NUMBER, {{Version::NONE, 1}}, false, AddressRisk::SAFE}
  };
}