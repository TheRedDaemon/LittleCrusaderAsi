
#include "versionGetter.h"

namespace modclasses
{
  void VersionGetter::giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep)
  {
    if (dep.size() == 1)
    {
      if (dep.at(0)->getModType() == ModType::ADDRESS_BASE)
      {
        addrBase = std::static_pointer_cast<AddressBase>(dep.at(0));
      }
    }

    if (addrBase.expired())
    {
      LOG(WARNING) << "VersionGetter failed to receive dependency.";
    }
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
    if (auto addressBase = addrBase.lock())
    {
      if (addressBase->initialisationDone())
      {
        // version elif
        // extreme41
        if (isThisVersion(
          (unsigned char*)(addressBase->getBaseAddress() + relExtremeVersion41String),
          (signed char*)(addressBase->getBaseAddress() + relExtremeVersion41Number),
          "V1.%d.1-E", 41))
        {
          version = Version::V1P41P1SE;
        }
        // 41
        else if (isThisVersion(
          (unsigned char*)(addressBase->getBaseAddress() + relVersion41String),
          (signed char*)(addressBase->getBaseAddress() + relVersion41Number),
          "V1.%d", 41))
        {
          version = Version::V1P41;
        }
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
    {Address::VERSION_STRING, {{Version::NONE, 9}, {Version::V1P41, 5}}, AddressRisk::SAFE},
    {Address::VERSION_NUMBER, {{Version::NONE, 1}}, AddressRisk::SAFE}
  };
}