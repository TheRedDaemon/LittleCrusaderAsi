
#include "addressResolver.h"

namespace modclasses
{
  // throw error is address not known
  const DWORD AddressResolver::getAddress(const Address memAddr)
  {
    // this one will contain a giant switch with all memory addresses and nested switches for the versions (and comments to the length in byte)
    // might get gigantic (hopefully?), so another structure might be needed one day, but a switch should be fast

    // no default, if nothing found just run further
    switch (memAddr)
    {
      // version infos
      case Address::VERSION_STRING:
        switch (version)
        {
          case Version::V1P41P1SE:
            return 0x1A1FE8; // 9 byte
          case Version::V1P41:
            return 0x1A1FDC; // 5 byte
        }
        break;

      case Address::VERSION_NUMBER:
        switch (version)
        {
          case Version::V1P41P1SE:
            return 0x24EFC; // 1 byte
          case Version::V1P41:
            return 0x24EEC; // 1 byte
        }
        break;


    }

    // if no value was returned, then there seems to be none, throw exception in this case
    throw std::exception(("No address of type with id '" + std::to_string(static_cast<int>(memAddr))
                          + "' found for version with id '" + std::to_string(static_cast<int>(version)) + "'.").c_str());
  }
}
