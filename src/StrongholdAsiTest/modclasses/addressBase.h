#ifndef ADDRESSBASE
#define ADDRESSBASE

#include "IntSafe.h" // for DWORD
#include "modBase.h"

namespace modclasses
{
  class AddressBase : public ModBase
  {
  private:
    DWORD addressBase{ 0x0 }; // filled from initialize

  public:

    ModType getModType() const override
    {
      return ModType::ADDRESS_BASE;
    }

    std::vector<ModType> getDependencies() const override
    {
      return {};
    }

    // has no dependencies, maybe -> however, might require stuff from the dllmain?
    void giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep) override {};

    bool initialize() override;

    /**con- and destructor**/
    AddressBase() { } // default cons, does nothing though

    /**additional functions for others**/

    // return version (will be received 
    const DWORD getBaseAddress() const
    {
      if (initialized)
      {
        return addressBase;
      }
      // called if not initialized
      throw std::exception("Address wasn't successfully initialized, but \"getAddressBase\" was still called.");
    }

    /**misc**/

    // prevent copy and assign (not sure how necessary)
    AddressBase(const AddressBase &base) = delete;
    virtual AddressBase& operator=(const AddressBase &base) final = delete;
  };
}

#endif //ADDRESSBASE
