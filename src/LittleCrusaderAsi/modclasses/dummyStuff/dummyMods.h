#ifndef DUMMYMODS
#define DUMMYMODS

#include "../modBase.h"
#include "../AddressResolver.h"

namespace modclasses
{
  class Test1 : public ModBase
  {
  private:

    std::weak_ptr<AddressResolver> resolver{};

    std::vector<AddressRequest> usedAddresses{
      {Address::VERSION_STRING, {{Version::NONE, 9}, {Version::V1P41, 5}}, false, AddressRisk::SAFE},
      {Address::VERSION_NUMBER, {{Version::NONE, 1}}, false, AddressRisk::CRITICAL}
    };

  public:

    Test1(std::weak_ptr<modcore::ModKeeper> modKeeper) : ModBase(modKeeper){};

    ModType getModType() const override
    {
      return ModType::TEST1;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::ADDRESS_RESOLVER };
    }

    void initialize() override
    {
      getMod<AddressResolver>();
      initialized = resolver.lock()->requestAddresses(usedAddresses, *this);
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
  private:

    std::weak_ptr<Test1> test{};

  public:

    ModType getModType() const override
    {
      return ModType::TEST2;
    }

    std::vector<ModType> getDependencies() const override
    {
      return { ModType::TEST1 };
    }

    void initialize() override {};

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
