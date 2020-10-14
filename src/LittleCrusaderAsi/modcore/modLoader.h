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

  public:

    // config will be send as ref, but will be created in the Constructor and discarded afterwards, so create a lokal copy if needed
    ModLoader();

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