
#include "logUtility.h"

namespace modcore
{
  static bool copyLogAtEnd{ false };

  void enableErrorLog()
  {
    if (!copyLogAtEnd)
    {
      copyLogAtEnd = true;
    }
  }

  bool copyErrorLog()
  {
    return copyLogAtEnd;
  }
}