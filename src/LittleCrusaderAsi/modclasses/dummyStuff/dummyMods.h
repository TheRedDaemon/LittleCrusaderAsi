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

    ModType getModType() const override
    {
      return ModType::TEST1;
    }

    std::unique_ptr<std::unordered_map<ModType, std::unique_ptr<DependencyRecContainer>>> neededDependencies() override
    {
      auto mapPointer = std::make_unique<std::unordered_map<ModType, std::unique_ptr<DependencyRecContainer>>>();
      mapPointer->try_emplace(ModType::ADDRESS_RESOLVER, std::make_unique<DependencyReceiver<AddressResolver>>(&resolver));
      return mapPointer;
    }

    bool initialize() override
    {
      initialized = resolver.lock()->requestAddresses(usedAddresses, *this);
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
  private:

    std::weak_ptr<Test1> test{};

  public:

    ModType getModType() const override
    {
      return ModType::TEST1;
    }

    std::unique_ptr<std::unordered_map<ModType, std::unique_ptr<DependencyRecContainer>>> neededDependencies() override
    {
      auto mapPointer = std::make_unique<std::unordered_map<ModType, std::unique_ptr<DependencyRecContainer>>>();
      mapPointer->try_emplace(ModType::TEST1, std::make_unique<DependencyReceiver<Test1>>(&test));
      return mapPointer;
    }

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
