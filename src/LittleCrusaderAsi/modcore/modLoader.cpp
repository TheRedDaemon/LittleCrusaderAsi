
#include <string>

#include "modLoader.h"
#include "logUtility.h"

namespace modcore
{
  ModLoader::ModLoader(HMODULE ownModule) : modKeeper{ std::make_shared<ModKeeper>(ownModule) }
  {
    // will load config and what ever is required at the start
    std::unordered_map<MT, Json> modConfigs{};
    try
    {
      std::ifstream configStream("modConfig.json"); // reading config from file
      if (!configStream.good())
      {
        throw std::exception("Unable to load config file. Does it exist?");
      }

      Json modConfig = Json::parse(configStream);

      for (auto& pair : modConfig.items())
      {
        Json key = pair.key();
        modConfigs[key] = pair.value();
      }
    }
    catch (const Json::parse_error& o_O) {
      LOG(ERROR) << "Config parse error: " << o_O.what();
      throw;
    }
    catch (const std::exception& o_O) {
      LOG(ERROR) << "Error during config load: " << o_O.what();
      throw;
    }
    LOG(INFO) << "Configuration loaded.";

    fillAndOrderModVector(modConfigs);
    LOG(INFO) << "Mods loaded.";

    // end by calling initialze on every mod:
    int initializedMods{ 0 };
    for (size_t i = 0; i < modKeeper->loadedMods.size(); i++)
    {
      if (modKeeper->loadedMods.at(i)->mod->callInitialize())
      {
        ++initializedMods;
      }
    }
    LOG(INFO) << initializedMods << " of " << modKeeper->loadedMods.size() << " mods initialized.";
  }

  void ModLoader::dllThreadAttachEvent()
  {
    if (!firstThreadAttachAfterDllAttachReceived)
    {
      firstThreadAttachAfterDllAttachReceived = true;

      // at the first attach event, fire this event to the mods
      for (size_t i = 0; i < modKeeper->loadedMods.size(); i++)
      {
        modKeeper->loadedMods.at(i)->mod->firstThreadAttachAfterModAttachEvent();
      }
    }
  }

  // will run backwards over mods and call cleanUp
  // destruction will be handled by smart pointers
  ModLoader::~ModLoader()
  {
    // call cleanUp in reverse dependency order (is this reverse order?)

    for (int i = modKeeper->loadedMods.size() - 1; i >= 0; i--)
    {
      modKeeper->loadedMods.at(i)->mod->cleanUp();
    }
    LOG(INFO) << "Cleaned up mods.";
  }

  void ModLoader::fillAndOrderModVector(const std::unordered_map<MT, Json> &modConfigs)
  {
    std::unordered_map<MT, std::shared_ptr<ModKeeper::ModContainer>> modSortMap{};

    // normaly, the mod will run over all configs and add them 
    for (const auto& config : modConfigs)
    {
      fulfillDependencies(modConfigs, modSortMap, config.first, config.second);
    }
  }

  void ModLoader::fulfillDependencies(
    const std::unordered_map<MT, Json> &modConfigs,
    std::unordered_map<MT, std::shared_ptr<ModKeeper::ModContainer>> &modSortMap,
    MT neededMod, const Json &config)
  {

    if (modSortMap.find(neededMod) != modSortMap.end())
    {
      if (!modSortMap[neededMod])
      {
        throw std::exception(("Cylic dependency reference encountered! Dependency with id '"
                              + std::to_string(static_cast<int>(neededMod))
                              + "' was already requested.").c_str());
      } // no else, if mod inside, everything is okay
    }
    else
    {
      modSortMap[neededMod];  // default constructs shared_ptr to null

      // create mod only if needed, otherwise unused creates
      std::shared_ptr<ModKeeper::ModContainer> nextMod = std::make_shared<ModKeeper::ModContainer>(neededMod, createMod(neededMod, config));

      std::vector<MT> deps{ nextMod->mod->getDependencies() };
      if (!deps.empty())
      {
        for (MT dep : deps)
        {
          if (modConfigs.find(dep) != modConfigs.end())
          {
            fulfillDependencies(modConfigs, modSortMap, dep, modConfigs.at(dep));
          }
          else
          {
            // trying to call build in stuff (without external config?)
            fulfillDependencies(modConfigs, modSortMap, dep, nullptr);
          }

          // add, that this mad wanted this dependency
          modSortMap[dep]->modsThatNeedThis.push_back(neededMod);
        }
      } // if no dependency, it can be added

      // at this point, either everything is fulfilled or it broke anyway
      modSortMap[neededMod] = nextMod;
      modKeeper->loadedMods.push_back(nextMod);
    }
  }
}