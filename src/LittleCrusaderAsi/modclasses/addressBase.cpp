#include <Windows.h>

#include "addressBase.h"

namespace modclasses
{
  bool AddressBase::initialize()
  {
    // code of the tutorial here
    // source: https://relearex.wordpress.com/2017/12/26/hooking-series-part-i-import-address-table-hooking/

    addressBase = (DWORD)GetModuleHandleW(NULL);

    // with the source tut, maybe far more would be possible?

    if (addressBase != 0x0) // assuming base address never 0x0
    {
      initialized = true;
      LOG(INFO) << "AddressBase initialized.";
    }
    else
    {
      LOG(WARNING) << "AddressBase was not initialized.";
    }

    return initialized;
  }
}
