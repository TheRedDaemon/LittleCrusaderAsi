#ifndef MODLOADER
#define MODLOADER

#include <vector>
#include <memory>
#include <unordered_map>

#include "../modclasses/modBase.h"
#include "../modclasses/enumheaders/modTypes.h"

#include "modKeeper.h"

// json
#include "../dependencies/jsonParser/json.hpp"

namespace modcore
{
  using Json = nlohmann::json;
  using Mod = modclasses::ModBase;
  using MT = modclasses::ModType;

  class ModLoader final // preventing inheritance, since destructor wont be virtual...
  {
  private:

    std::shared_ptr<ModKeeper> modKeeper = std::make_shared<ModKeeper>();
    bool firstThreadAttachAfterDllAttachReceived{ false };

  public:

    // config will be send as ref, but will be created in the Constructor and discarded afterwards, so create a lokal copy if needed
    ModLoader();

    // When the first thread attach message arives, at least some data (aic was where I noticed this) is already loaded, unlike after DLL_PROCESS_ATTACH
    // I will use the first as an event handler for now
    // BUT: I don't know what causes this, the optimium would be stronghold, this would be reliable
    // This should likely by replaced by something more controlled, like an event handler
    void dllThreadAttachEvent();

    ~ModLoader();

    /**misc**/

    // prevent copy and assign (not sure how necessary, or if I done it right...)
    ModLoader(const ModLoader &base) = delete;
    virtual ModLoader& operator=(const ModLoader &base) final = delete;
  
  private:

    void fillAndOrderModVector(const std::unordered_map<MT, Json> &modConfigs);
    void fulfillDependencies( // for recursive fill
      const std::unordered_map<MT, Json> &modConfigs,
      std::unordered_map<MT, std::shared_ptr<ModKeeper::ModContainer>> &modSortMap,
      MT neededMod, const Json &config);

    const std::shared_ptr<Mod> createMod(MT modType, const Json &config);
  };
}

#endif //MODLOADER