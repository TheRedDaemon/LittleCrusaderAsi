#ifndef STRONGVERSIONS
#define STRONGVERSIONS

namespace modclasses
{
  // containes enums for versions, used to get the right addresses etc.
  enum class Version
  {
    NONE,       // failed
    V1P41P1SE,  // 1.41.1-E
    V1P41       // 1.41
  };

  // used to parse string to enum
  NLOHMANN_JSON_SERIALIZE_ENUM(Version, {
    {Version::NONE, nullptr},
    {Version::V1P41P1SE, "V1.41.1-E"},
    {Version::V1P41, "V1.41"}
  })
}

#endif //STRONGVERSIONS