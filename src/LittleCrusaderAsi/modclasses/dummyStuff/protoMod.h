#ifndef PROTOMOD
#define PROTOMOD

// needs to be adapted to new relative position
#include "../modBase.h"

namespace modclasses
{

  // simple proto class, replace all 'ProtoMod' with the new name, also, see 'include'
  class ProtoMod : public ModBase
  {

  public:

    // needs to be registered by ModManager
    ModID getModID() const override
    {
      return nullptr;
    }

    std::vector<ModID> getDependencies() const override
    {
      return {};
    }

    /**con- and destructor**/
    ProtoMod() { } // default cons, does nothing though

    /**additional functions for others**/

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    ProtoMod(const ProtoMod &base) = delete;
    virtual ProtoMod& operator=(const ProtoMod &base) final = delete;

  private:

    // should be defined in .cpp
    void initialize() override
    {
    }
  };
}

#endif //PROTOMOD
