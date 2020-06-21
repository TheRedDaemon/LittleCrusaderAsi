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

    // build ranges // also influences Ki (wall, but no towers if to low or minus) // will endines be respected...
    // (if this is even is the reason why the memory has the lowest value byte first
    // question would be.... the whole function, or the single values? (currently single values) (addresses see link and cheat engine)
    BUILDRANGE_CRUSADE_CASTLE_160,  // signed int32 : build range on a 160x160 map (at least during crusader match)
    BUILDRANGE_CRUSADE_CASTLE_200,  // signed int32 : build range on a 200x200 map (at least during crusader match)
    BUILDRANGE_CRUSADE_CASTLE_300,  // signed int32 : build range on a 300x300 map (at least during crusader match)
    BUILDRANGE_CRUSADE_CASTLE_400,  // signed int32 : build range on a 400x400 map (at least during crusader match)

  };
}

#endif //ADDRESSENUMS