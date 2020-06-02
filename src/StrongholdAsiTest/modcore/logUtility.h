#ifndef LOGUTIL
#define LOGUTIL

namespace modcore
{
  // use to mark that the log file should be copied at the end
  // to be used in error case
  void enableErrorLog();
  bool copyErrorLog();
}

#endif // LOGUTIL
