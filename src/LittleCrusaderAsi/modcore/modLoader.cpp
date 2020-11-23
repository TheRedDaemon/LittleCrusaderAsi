
#include "modLoader.h"

namespace modcore
{
  ModLoader::ModLoader(HMODULE ownModule)
  {
    // set own Module handle
    ModMan::handle = ownModule;

    // will load config and what ever is required at the start
    std::unordered_map<ModID, Json> modConfigs{};
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
        ModID key{ ModManager::GetEnumByName(pair.key()) };
        if (!key)
        {
          LOG(WARNING) << "No mod registration for name '" << pair.key() << "' found.";
          continue; // ignore
        }

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
    for (Mod* modPtr : ModMan::orderedMods)
    {
      if (modPtr->callInitialize())
      {
        ++initializedMods;
      }
    }

    LOG(INFO) << initializedMods << " of " << ModMan::orderedMods.size() << " mods initialized.";
  }

  void ModLoader::dllThreadAttachEvent()
  {
    // at the first attach event, fire this event to the mods
    for (Mod* modPtr : ModMan::orderedMods)
    {
      modPtr->threadAttachAfterModAttachEvent();
    }
  }

  // will run backwards over mods and call cleanUp
  // destruction will be handled by smart pointers
  ModLoader::~ModLoader()
  {
    // call cleanUp in reverse dependency order (is this reverse order?)

    for (int i = ModMan::orderedMods.size() - 1; i >= 0; i--)
    {
      ModMan::orderedMods.at(i)->cleanUp();
    }
    LOG(INFO) << "Cleaned up mods.";
  }

  void ModLoader::fillAndOrderModVector(const std::unordered_map<ModID, Json> &modConfigs)
  {
    // normaly, the mod will run over all configs and add them 
    for (const auto& config : modConfigs)
    {
      fulfillDependencies(modConfigs, config.first, config.second);
    }
  }

  void ModLoader::fulfillDependencies(
    const std::unordered_map<ModID, Json> &modConfigs,
    ModID neededMod, const Json &config)
  {

    if (auto it{ ModMan::loadedMods.find(neededMod) }; it != ModMan::loadedMods.end())
    {
      if (!(it->second.mod))
      {
        throw std::exception(("Cylic dependency reference encountered! Dependency with id '"
                              + neededMod->getName()
                              + "' was already requested.").c_str());
      } // no else, if mod inside, everything is okay
    }
    else
    {
      // create mod entry:
      ModMan::loadedMods[neededMod];

      // create mod only if needed, otherwise unused creates
      auto nextMod{ (neededMod->getValue())(config) };

      std::vector<ModID> deps{ nextMod->getDependencies() };
      if (!deps.empty())
      {
        for (ModID dep : deps)
        {
          if (modConfigs.find(dep) != modConfigs.end())
          {
            fulfillDependencies(modConfigs, dep, modConfigs.at(dep));
          }
          else
          {
            // trying to call build in stuff (without external config?)
            fulfillDependencies(modConfigs, dep, nullptr);
          }

          // add, that this mod wanted this dependency
          ModMan::loadedMods[dep].modsThatNeedThis.push_back(neededMod);
        }
      } // if no dependency, it can be added

      // at this point, either everything is fulfilled or it broke anyway
      ModMan::loadedMods[neededMod].mod = nextMod;
      ModMan::orderedMods.push_back(&(*nextMod));
    }
  }
}