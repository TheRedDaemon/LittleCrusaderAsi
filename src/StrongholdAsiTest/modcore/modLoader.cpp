
#include <string>

#include "modLoader.h"
#include "logUtility.h"

namespace modcore
{
  ModLoader::ModLoader()
  {
    // will load config and what ever is required at the start
    //... nothing here the moment

    // create, fill and order mods based one dependency
    // dummy map:
    std::unordered_map<MT, bool /* what ever will be required, bool is just fill*/> modConfigs{
      {MT::TEST1, false}  // if dep on Test2 is added in Test1 -> should create cyclic ref
    };
    // dummy map

    fillAndOrderModVector(modConfigs);

    // end by calling initialze on every mod:
    for (size_t i = 0; i < loadedMods.size(); i++)
    {
      loadedMods.at(i)->initialize();
    }
  }

  // will run backwards over mods and call cleanUp
  // destruction will be handled by smart pointers
  ModLoader::~ModLoader()
  {
    // call cleanUp in reverse dependency order (is this reverse order?)

    for (int i = loadedMods.size() - 1; i >= 0; i--)
    {
      loadedMods.at(i)->cleanUp();
    }
  }

  // using struct to have a sort object
  struct ModSortContainer
  {
    std::shared_ptr<Mod> mod;
    std::vector<MT> dependencies;
    std::vector<bool> dependenciesFulfilled;
    int finalPlace;
    bool done{ false };
  };

  void ModLoader::fillAndOrderModVector(const std::unordered_map<MT, bool /* what ever will be required, bool is just fill*/> &modConfigs)
  {
    //std::unordered_map<MT, std::vector<std::shared_ptr<Mod>>> modSortMap{};
    std::unordered_map<MT, std::shared_ptr<Mod>> modSortMap{};

    // normaly, the mod will run over all configs and add them 
    for (const auto& config : modConfigs)
    {
      fulfillDependencies(modConfigs, modSortMap, config.first, config.second);
    }
  }

  void ModLoader::fulfillDependencies(
    const std::unordered_map<MT, bool /* what ever will be required, bool is just fill*/> &modConfigs,
    std::unordered_map<MT, std::shared_ptr<Mod>> &modSortMap,
    MT neededMod, const bool &config /* dummy will be ref*/)
  {

    if (modSortMap.find(neededMod) != modSortMap.end())
    {
      if (modSortMap[neededMod] == nullptr)
      {
        throw std::exception(("Cylic dependency reference encountered! Dependency with id '"
                              + std::to_string(static_cast<int>(neededMod))
                              + "' was already requested.").c_str());
      } // no else, if mod inside, everything is okay
    }
    else
    {
      modSortMap[neededMod];  // default constructs shared_ptr to null

      std::shared_ptr<Mod> nextMod{ createMod(neededMod, config) }; // create mod only if needed, otherwise unused creates

      std::vector<MT> deps{ nextMod->getDependencies() };
      if (!deps.empty())
      {
        std::vector<std::weak_ptr<Mod>> neededDep{};

        for (MT dep : deps)
        {
          if (modConfigs.find(dep) != modConfigs.end())
          {
            fulfillDependencies(modConfigs, modSortMap, dep, modConfigs.at(dep));
          }
          else
          {
            // trying to call build in stuff (without external config?)
            fulfillDependencies(modConfigs, modSortMap, dep, false /* dummy */);
          }

          neededDep.push_back(modSortMap[dep]);
        }

        nextMod->giveDependencies(neededDep);
      } // if no dependency, it can be added

      // at this point, either everything is fulfilled or it broke anyway
      modSortMap[neededMod] = nextMod;
      loadedMods.push_back(nextMod);
    }
  }
}