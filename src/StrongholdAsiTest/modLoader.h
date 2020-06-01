#ifndef MODLOADER
#define MODLOADER

#include <vector>
#include <memory>
#include <unordered_map>

#include "modBase.h"
#include "modTypes.h"

namespace modcore
{
  using Mod = modclasses::ModBase;
  using MT = modclasses::ModType;

  class ModLoader final // preventing inheritance, since destructor wont be virtual...
  {
  private:
    std::vector<std::shared_ptr<Mod>> loadedMods{};

  public:

    // config will be send as ref, but will be created in the Constructor and discarded afterwards, so create a lokal copy if needed
    ModLoader();

    ~ModLoader();

    /**misc**/

    // prevent copy and assign (not sure how necessary, or if I done it right...)
    ModLoader(const ModLoader &base) = delete;
    virtual ModLoader& operator=(const ModLoader &base) final = delete;
  
  private:

    void fillAndOrderModVector(const std::unordered_map<MT, bool /*object carrying needed config data*/> &modConfigs);
    void fulfillDependencies( // for recursive fill
      const std::unordered_map<MT, bool /* what ever will be required, bool is just fill*/> &modConfigs,
      std::unordered_map<MT, std::shared_ptr<Mod>> &modSortMap,
      MT neededMod, const bool &config /* dummy will be ref*/);

    const std::shared_ptr<Mod> createMod(MT modType, const bool &config /* some basic object, a json node maybe */);
  };
}

#endif //MODLOADER