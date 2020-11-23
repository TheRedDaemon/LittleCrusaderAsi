#ifndef MODLOADER
#define MODLOADER

#include "../modclasses/modBase.h"

namespace modcore
{
  using Json = nlohmann::json;
  using Mod = modclasses::ModBase;

  class ModLoader final // preventing inheritance, since destructor wont be virtual...
  {

  public:

    // config will be send as ref, but will be created in the Constructor and discarded afterwards, so create a lokal copy if needed
    ModLoader(HMODULE ownModule);

    // When the thread attach message arives, at least some data (aic was where I noticed this) is already loaded, unlike after DLL_PROCESS_ATTACH
    // This should likely by replaced by something more controlled, like an event handler
    // NOTE: currently left for convenience
    void dllThreadAttachEvent();

    ~ModLoader();

    /**misc**/

    // prevent copy and assign (not sure how necessary, or if I done it right...)
    ModLoader(const ModLoader &base) = delete;
    virtual ModLoader& operator=(const ModLoader &base) final = delete;
  
  private:

    void fillAndOrderModVector(const std::unordered_map<ModID, Json> &modConfigs);
    void fulfillDependencies( // for recursive fill
      const std::unordered_map<ModID, Json> &modConfigs,
      ModID neededMod, const Json &config);
  };
}

#endif //MODLOADER