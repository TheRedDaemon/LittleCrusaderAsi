#ifndef DUMMYMODS
#define DUMMYMODS

#include "../modBase.h"

namespace modclasses
{
  class Test1 : public ModBase
  {
  public:

    ModType getModType() const override
    {
      return ModType::TEST1;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::ADDRESS_BASE };
    }

    // has no dependencies, maybe -> however, might require stuff from the dllmain?
    void giveDependencies(const std::vector<std::weak_ptr<ModBase>>) override
    {
    };

    bool initialize() override
    {
      return initialized;
    };

    /**con- and destructor**/
    Test1()
    {
      initialized = true;
    }

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    Test1(const Test1 &base) = delete;
    virtual Test1& operator=(const Test1 &base) final = delete;
  };

  class Test2 : public ModBase
  {
  public:

    ModType getModType() const override
    {
      return ModType::TEST1;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::TEST1 };
    }

    // has no dependencies, maybe -> however, might require stuff from the dllmain?
    void giveDependencies(const std::vector<std::weak_ptr<ModBase>>) override
    {
    };

    bool initialize() override
    {
      return initialized;
    };

    /**con- and destructor**/
    Test2()
    {
      initialized = true;
    }

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    Test2(const Test2 &base) = delete;
    virtual Test2& operator=(const Test2 &base) final = delete;
  };
}

#endif //DUMMYMODS
