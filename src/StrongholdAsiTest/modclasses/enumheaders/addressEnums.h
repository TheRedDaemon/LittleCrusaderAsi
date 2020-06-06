#ifndef ADDRESSENUMS
#define ADDRESSENUMS

namespace modclasses
{
  // containes enums for every address
  // always add a small description to it (or give it a telling name) and maybe the data type
  // these are only aliases, the actual address (together with commands to used bytes)
  // should be made in the "great resolver file"
  enum class Address
  {
    NONE,                             // DUMMY (and for failure, if needed somewhere)
    VERSION_STRING,                   // (unsigned?) char : Version String of the version displayed in the main menu
    VERSION_NUMBER,                   // signed byte : Version number of the Version String in the menu (minor number)
  };
}

#endif //ADDRESSENUMS