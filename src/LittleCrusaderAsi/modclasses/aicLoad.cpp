
#include <algorithm>
#include <limits>

#include "aicLoad.h"

namespace modclasses
{
  AICLoad::AICLoad(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
    // TODO?: it might be an idea to create a more general function for simple json value reads, but this depends on how samey they are...

    auto confIt = config.find("startState");
    if (confIt != config.end() && confIt.value().is_boolean())
    {
      isChanged = confIt.value().get<bool>();
    }
    else
    {
      LOG(WARNING) << "AICLoad: No valid 'startState' found. Needs to be boolean. Defaults to 'false'. Will not load AIC in game until requested.";
    }

    confIt = config.find("modFolderRelToStronghold");
    if (confIt != config.end() && confIt.value().is_string())
    {
      relModPath = confIt.value().get<std::string>();
    }
    else
    {
      LOG(WARNING) << "AICLoad: No valid 'modFolderRelToStronghold' found. Needs to be string. Mod assumes to be in the game root.";
    }


    confIt = config.find("aicFolder");
    if (confIt != config.end() && confIt.value().is_string())
    {
      aicFolder = confIt.value().get<std::string>();
    }
    else
    {
      LOG(WARNING) << "AICLoad: No valid 'aicFolder' found. Needs to be string. Mod will look for the AICs in the mod root folder.";
    }

    confIt = config.find("loadList");
    if (!(confIt == config.end() || !confIt.value().is_array() || confIt.value().empty() || !confIt.value()[0].is_string()))
    {
      loadList = confIt.value().get<std::vector<std::string>>();
    }
    else
    {
      LOG(WARNING) << "AICLoad: No valid 'loadList' found. Needs to be array of string. No AICs will be loaded.";
    }

    confIt = config.find("keyConfig");
    if (confIt != config.end())
    {
      auto keyIt = confIt.value().find("activate");
      if (keyIt != confIt.value().end())
      {
        keysActivate = keyIt.value();
      }
      else
      {
        LOG(WARNING) << "AICLoad: 'keyConfig': No 'activate' found. AIC load status can not be changed using only this mod and the keyboard.";
      }

      keyIt = confIt.value().find("reloadMain");
      if (keyIt != confIt.value().end())
      {
        keysReloadMain = keyIt.value();
      }
      else
      {
        LOG(WARNING) << "AICLoad: 'keyConfig': No 'reloadMain' found. Main AIC (first in load list) can not be directly reloaded using the keyboard.";
      }

      keyIt = confIt.value().find("reloadAll");
      if (keyIt != confIt.value().end())
      {
        keysReloadAll = keyIt.value();
      }
      else
      {
        LOG(WARNING) << "AICLoad: 'keyConfig': No 'reloadAll' found. The load list can not be directly reloaded using the keyboard.";
      }
    }
    else
    {
      LOG(WARNING) << "AICLoad: No 'keyConfig' found. No keyboard control will be provided.";
    }
  };


  // will need address and keyboard stuff
  std::vector<ModType> AICLoad::getDependencies() const
  {
    return { ModType::ADDRESS_RESOLVER, ModType::KEYBOARD_INTERCEPTOR, ModType::BLT_OVERLAY };
  }


  void AICLoad::initialize()
  {
    auto addressResolver = getMod<AddressResolver>();
    auto keyInterceptor = getMod<KeyboardInterceptor>();
    auto bltOverlay = getMod<BltOverlay>();

    // check mods and request addresses
    if (!(addressResolver && keyInterceptor && addressResolver->requestAddresses(usedAddresses, *this)))
    {
      // using overlay send to console
      BltOverlay::sendToConsole("AICLoad was not initialized.", el::Level::Warning);
      return;
    }

    // get pointer
    aicMemoryPtr = addressResolver->getAddressPointer<int32_t[2704]>(Address::AIC_IN_MEMORY, *this);

    // load aic data
    for (auto& aicName : loadList)
    {
      auto mapPointer{ loadAICFile(aicName, true) };

      // if result, then not null
      if (mapPointer)
      {
        loadedAICValues[aicName].swap(mapPointer);
      }
    }

    if (!keysActivate.empty())
    {
      std::function<void(const HWND, const bool, const bool)> func =
        [this](const HWND hw, const bool keyUp, const bool repeat){this->activateAICs(hw, keyUp, repeat);};

      if (!keyInterceptor->registerFunction<true>(func, keysActivate))
      {
        BltOverlay::sendToConsole("AICLoad: Activate: At least one key combination was not registered.", el::Level::Warning);
      }
    }

    if (!keysReloadMain.empty())
    {
      std::function<void(const HWND, const bool, const bool)> func =
        [this](const HWND hw, const bool keyUp, const bool repeat){this->reloadMainAIC(hw, keyUp, repeat);};

      if (!keyInterceptor->registerFunction<true>(func, keysReloadMain))
      {
        BltOverlay::sendToConsole("AICLoad: ReloadMainAIC: At least one key combination was not registered.", el::Level::Warning);
      }
    }

    if (!keysReloadAll.empty())
    {
      std::function<void(const HWND, const bool, const bool)> func =
        [this](const HWND hw, const bool keyUp, const bool repeat){this->reloadAllAIC(hw, keyUp, repeat);};

      if (!keyInterceptor->registerFunction<true>(func, keysReloadAll))
      {
        BltOverlay::sendToConsole("AICLoad: KeysReloadAll: At least one key combination was not registered.",  el::Level::Warning);
      }
    }

    if (bltOverlay)
    {
      bltOverlay->registerDDrawLoadEvent([this]()
      {
        this->initialAICLoad();
      });
    }
    else
    {
      LOG(WARNING) << "AICLoad: Unable to register initialAICLoad-event, because BltOverlay was not initialized.";
    }

    initialized = true;
    BltOverlay::sendToConsole("AICLoad initialized.", el::Level::Info);
  }


  void AICLoad::initialAICLoad()
  {
    if (initialized)
    {
      // save vanilla values
      std::copy(std::begin(*aicMemoryPtr), std::end(*aicMemoryPtr), vanillaAIC.begin());

      if (vanillaAIC.at(0) != 12)
      {
        BltOverlay::sendToConsole("First thread attached event was seemingly triggerd before AIC initialisation. AICLoad features are likely unreliable. "
                                  "Indicator: first int is not a 12.", el::Level::Warning);
      }

      // add menu stuff (test)
      if (auto bltOverlay = getMod<BltOverlay>(); bltOverlay)
      {
        MenuBase* basPtr{ bltOverlay->getMainMenu() };
        if (basPtr)
        {
          MenuBase& base{ *basPtr };

          base.createMenu<MainMenu, true>(
            "AIC Load",
            nullptr,
            true
          )
            // NOTE -> Main Menu is descendable, but child will be hidden by func
            // they could be added though
            .createMenu<MainMenu, false>(
              "Reload All",
              [this](bool, std::string&)
              {
                this->reloadAllAIC(0, false, false);
                return false;
              },
              true
            )
            .createMenu<MainMenu, false>(
              "Reload Main",
              [this](bool, std::string&)
              {
                this->reloadMainAIC(0, false, false);
                return false;
              },
              true
            )
            .createMenu<MainMenu, false>(
              "AIC active: " + std::string(this->isChanged ? "true" : "false"),
              [this](bool, std::string& header)
              {
                this->activateAICs(0, false, false);
                header = "AIC active: " + std::string(this->isChanged ? "true" : "false");
                return false;
              },
              true
            )
            .createMenu<MainMenu, true>(
              "Editor",
              nullptr,
              true
            )
              .createMenu<MainMenu, true>(
                "Rat",
                nullptr,
                false
              )
                .createMenu<ChoiceInputMenu, false>(
                  "Farm1",
                  nullptr,
                  getNameOrNULL<Farm>(vanillaAIC[getAICFieldIndex(AIName::RAT, AIC::FARM_1)]),
                  std::vector<std::pair<std::string, int32_t>>{
                    { Farm::NO_FARM->getName(), Farm::NO_FARM->getValue() },
                    { Farm::WHEAT_FARM->getName(), Farm::WHEAT_FARM->getValue() },
                    { Farm::HOP_FARM->getName(), Farm::HOP_FARM->getValue() },
                    { Farm::APPLE_FARM->getName(), Farm::APPLE_FARM->getValue() },
                    { Farm::DAIRY_FARM->getName(), Farm::DAIRY_FARM->getValue() }
                  },
                  [this](int32_t newValue, std::string& resultMessage, bool onlyUpdateCurrent, std::string& currentValue)
                  {
                    if (onlyUpdateCurrent)
                    {
                      currentValue = getNameOrNULL<Farm>((*(this->aicMemoryPtr))[getAICFieldIndex(AIName::RAT, AIC::FARM_1)]);
                      return false;
                    }

                    // no need for extra check -> should be ok by definition
                    if (this->setPersonalityValueUnchecked(AIName::RAT, AIC::FARM_1, newValue))
                    {
                      resultMessage = "Success.";
                      return true;
                    }
                    else
                    {
                      resultMessage = "Failed.";
                      return false;
                    }
                  }
                );
        }
        else
        {
          BltOverlay::sendToConsole("AICLoad: Unable to get main menu.", el::Level::Warning);
        }
      }
      else
      {
        BltOverlay::sendToConsole("AICLoad: Unable to get BltOverlay for menu create.", el::Level::Warning);
      }


      // if aics are requested at start, load them in
      if (isChanged)
      {
        applyAICs();
        BltOverlay::sendToConsole("Activated AICs on start.", el::Level::Info);
      }

      // keyboard calls should not be a problem, they overwrite each other at worst -> could however corrupt the vanilla save

      BltOverlay::sendToConsole("AICLoad run through initial AIC loading.", el::Level::Info);
    }
  }


  // functions for others to use

  
  const bool AICLoad::setPersonalityValue(AINameEnum aiName, AICEnum field, const Json value)
  {
    if (aiName == AIName::NONE || field == AIC::NONE || !initialized)
    {
      return false;
    }

    int32_t newValue;
    if (isValidPersonalityValue(field, value, newValue))
    {
      // support for versions with only 8 KIs would need extra handling here
      (*aicMemoryPtr)[getAICFieldIndex(aiName, field)] = newValue;
      return true;
    }

    return false;
  }


  const bool AICLoad::setPersonalityValueUnchecked(AINameEnum aiName, AICEnum field, const int32_t value)
  {
    if (aiName == AIName::NONE || field == AIC::NONE || !initialized)
    {
      return false;
    }
    
    // support for versions with only 8 KIs would need extra handling here
    (*aicMemoryPtr)[getAICFieldIndex(aiName, field)] = value;
    return true;
  }


  // keyboard functions


  void AICLoad::activateAICs(const HWND, const bool keyUp, const bool repeat)
  {
    if (keyUp || repeat){ 
      return;
    }

    if (isChanged)
    {
      // back to vanilla
      std::copy(vanillaAIC.begin(), vanillaAIC.end(), std::begin(*aicMemoryPtr));
    }
    else
    {
      // load in aics
      applyAICs();
    }

    isChanged = !isChanged;

    std::string msg{ isChanged ? "true" : "false" };
    BltOverlay::sendToConsole("Changed load status of file AICs to: " + msg, el::Level::Info);
  }


  void AICLoad::reloadMainAIC(const HWND, const bool keyUp, const bool repeat)
  {
    // ignores silently if no aics set (good idea? not sure)
    if (keyUp || repeat || loadList.size() == 0)
    {
      return;
    }

    // loadList is currently unchangeable, so doing this should be ok for now
    std::string &mainName = loadList[0];
    size_t mainSize{ loadedAICValues.find(mainName) != loadedAICValues.end() ? loadedAICValues[mainName]->size() : 0 }; // in case it does not exist

    auto changedMain{ loadAICFile(mainName, false, mainSize) };
    if (!changedMain)
    {
      BltOverlay::sendToConsole("Error while reloading main. Loaded AIC data unchanged.", el::Level::Warning);
      return;
    }

    // swaps maps -> old should be deleted after this function
    loadedAICValues[mainName].swap(changedMain);

    if (isChanged)
    {
      // bit expensive here...
      std::copy(vanillaAIC.begin(), vanillaAIC.end(), std::begin(*aicMemoryPtr)); // reset to vanilla
      applyAICs();
    }

    BltOverlay::sendToConsole("Reloaded main AIC: " + mainName, el::Level::Info);
  }


  void AICLoad::reloadAllAIC(const HWND, const bool keyUp, const bool repeat)
  {
    // ignores silently if no aics set (good idea? not sure)
    if (keyUp || repeat || loadList.size() == 0)
    {
      return;
    }

    // same load function as in the initialization
    for (auto& aicName : loadList)
    {
      auto mapPointer{ loadAICFile(aicName, false) };

      // if result, then not null
      if (mapPointer)
      {
        loadedAICValues[aicName].swap(mapPointer);
      }
    }

    if (isChanged)
    {
      // bit expensive here...
      std::copy(vanillaAIC.begin(), vanillaAIC.end(), std::begin(*aicMemoryPtr)); // reset to vanilla
      applyAICs();
    }

    BltOverlay::sendToConsole("Reloaded all AICs.", el::Level::Info);
  }


  // private functions


  void AICLoad::applyAICs()
  {
    for (auto it = loadList.rbegin(); it != loadList.rend(); ++it)
    {
      // silently ignore missing AIC files -> warnings should already be in log file
      if (loadedAICValues.find(*it) != loadedAICValues.end())
      {
        for (auto& valuePair : *(loadedAICValues[*it]))
        {
          (*aicMemoryPtr)[valuePair.first] = valuePair.second;
        }
      }
    }
  }


  const int32_t AICLoad::getAICFieldIndex(AINameEnum aiName, AICEnum field) const
  {
    // 169 for every ai, minus 169 to get in range (aiName min 1, max 16; field min 0, max 168)
    return aiName->getValue() * 169 + field->getValue() - 169;
  }


  // small helper (maybe I should not use this file contained functions too much...)

  // enums require NONE at least
  template<typename T>
  static const bool validEnumHelper(AICEnum field, const Json &value, int32_t &validValue, const std::string& warningEnd)
  {
    if (!value.is_string())
    {
      LOG(WARNING) << "Field '" << field->getName() << "' did not contain a string.";
      return false;
    }


    typename T::EnumType valueEnum{ T::GetEnumByName(value.get_ref<const Json::string_t&>()) };
    if (!valueEnum || valueEnum == T::NONE) // every enum should have a none
    {
      LOG(WARNING) << "Field '" << field->getName() << "' did not contain a valid " << warningEnd << ".";
      return false;
    }

    validValue = valueEnum->getValue();
    return true;
  }


  // does not even need template
  static const bool validIntRangeHelper(AICEnum field, const Json &value, int32_t &validValue, const int32_t lowerEndIncl,
                                 const int32_t upperEndIncl, const std::string& warningEndRange)
  {
    if (!value.is_number_integer())
    {
      LOG(WARNING) << "Field '" << field->getName() << "' did not contain a number.";
      return false;
    }

    int32_t numValue = value.get<int32_t>();
    if (numValue < lowerEndIncl || numValue > upperEndIncl)
    {
      LOG(WARNING) << "Field '" << field->getName() << "' is not a number " << warningEndRange << ".";
      return false;
    }

    validValue = numValue;
    return true;
  }


  // will use switch statement to decide, since many have the same range
  // could be a bit smaller...
  // some are marked with cheating, which mean I changed the checks so the vanilla values pass... why is this so?
  const bool AICLoad::isValidPersonalityValue(AICEnum field, const Json &value, int32_t &validValue) const
  {
    // limit
    static const int32_t int32Max{ (std::numeric_limits<int32_t>::max)() };

    // static map for function types, returns int for switch
    // only basic value tests
    // mapping int32_t ids to int32_t -> could also use switch again, hm?
    // issue -> there was no constructor for const T* const value types it seems (map might work, but again, I could use the switch again
    static const std::unordered_map<int32_t, int32_t> switchMap{
      { AIC::UNKNOWN_000                        ->getValue() , 0  },
      { AIC::UNKNOWN_001                        ->getValue() , 0  },
      { AIC::UNKNOWN_002                        ->getValue() , 0  },
      { AIC::UNKNOWN_003                        ->getValue() , 0  },
      { AIC::UNKNOWN_004                        ->getValue() , 0  },
      { AIC::UNKNOWN_005                        ->getValue() , 0  },
      { AIC::UNKNOWN_011                        ->getValue() , 0  },
      { AIC::POPULATION_PER_FARM                ->getValue() , 0  },
      { AIC::POPULATION_PER_WOODCUTTER          ->getValue() , 0  },
      { AIC::POPULATION_PER_QUARRY              ->getValue() , 0  },
      { AIC::POPULATION_PER_IRONMINE            ->getValue() , 0  },
      { AIC::POPULATION_PER_PITCHRIG            ->getValue() , 0  },
      { AIC::BUILD_INTERVAL                     ->getValue() , 0  },
      { AIC::RESOURCE_REBUILD_DELAY             ->getValue() , 0  },
      { AIC::MAX_FOOD                           ->getValue() , 0  },
      { AIC::TRADE_AMOUNT_FOOD                  ->getValue() , 0  },
      { AIC::TRADE_AMOUNT_EQUIPMENT             ->getValue() , 0  },
      { AIC::UNKNOWN_040                        ->getValue() , 0  },
      { AIC::MINIMUM_GOODS_REQUIRED_AFTER_TRADE ->getValue() , 0  },
      { AIC::DOUBLE_RATIONS_FOOD_THRESHOLD      ->getValue() , 0  },
      { AIC::MAX_WOOD                           ->getValue() , 0  },
      { AIC::MAX_STONE                          ->getValue() , 0  },
      { AIC::MAX_RESOURCE_OTHER                 ->getValue() , 0  },
      { AIC::MAX_EQUIPMENT                      ->getValue() , 0  },
      { AIC::MAX_BEER                           ->getValue() , 0  },
      { AIC::MAX_RESOURCE_VARIANCE              ->getValue() , 0  },
      { AIC::RECRUIT_GOLD_THRESHOLD             ->getValue() , 0  },
      { AIC::DEF_WALLPATROL_RALLYTIME           ->getValue() , 0  },
      { AIC::DEF_WALLPATROL_GROUPS              ->getValue() , 0  },
      { AIC::DEF_SIEGEENGINE_GOLD_THRESHOLD     ->getValue() , 0  },
      { AIC::DEF_SIEGEENGINE_BUILD_DELAY        ->getValue() , 0  },
      { AIC::UNKNOWN_072                        ->getValue() , 0  },
      { AIC::UNKNOWN_073                        ->getValue() , 0  },
      { AIC::SORTIE_UNIT_RANGED_MIN             ->getValue() , 0  },
      { AIC::SORTIE_UNIT_MELEE_MIN              ->getValue() , 0  },
      { AIC::DEF_DIGGING_UNIT_MAX               ->getValue() , 0  },
      { AIC::RECRUIT_INTERVAL                   ->getValue() , 0  },
      { AIC::RECRUIT_INTERVAL_WEAK              ->getValue() , 0  },
      { AIC::RECRUIT_INTERVAL_STRONG            ->getValue() , 0  },
      { AIC::DEF_TOTAL                          ->getValue() , 0  },
      { AIC::OUTER_PATROL_GROUPS_COUNT          ->getValue() , 0  },
      { AIC::OUTER_PATROL_RALLY_DELAY           ->getValue() , 0  },
      { AIC::DEF_WALLS                          ->getValue() , 0  },
      { AIC::RAID_UNITS_BASE                    ->getValue() , 0  },
      { AIC::RAID_UNITS_RANDOM                  ->getValue() , 0  },
      { AIC::UNKNOWN_124                        ->getValue() , 0  },
      { AIC::ATT_FORCE_BASE                     ->getValue() , 0  },
      { AIC::ATT_FORCE_RANDOM                   ->getValue() , 0  },
      { AIC::ATT_FORCE_SUPPORT_ALLY_THRESHOLD   ->getValue() , 0  },
      { AIC::UNKNOWN_129                        ->getValue() , 0  },
      { AIC::UNKNOWN_130                        ->getValue() , 0  },
      { AIC::ATT_UNIT_PATROL_RECOMMAND_DELAY    ->getValue() , 0  },
      { AIC::UNKNOWN_132                        ->getValue() , 0  },
      { AIC::COW_THROW_INTERVAL                 ->getValue() , 0  },
      { AIC::UNKNOWN_142                        ->getValue() , 0  },
      { AIC::ATT_MAX_ENGINEERS                  ->getValue() , 0  },
      { AIC::ATT_DIGGING_UNIT_MAX               ->getValue() , 0  },
      { AIC::ATT_UNIT_2_MAX                     ->getValue() , 0  },
      { AIC::ATT_MAX_ASSASSINS                  ->getValue() , 0  },
      { AIC::ATT_MAX_LADDERMEN                  ->getValue() , 0  },
      { AIC::ATT_MAX_TUNNELERS                  ->getValue() , 0  },
      { AIC::ATT_UNIT_PATROL_MAX                ->getValue() , 0  },
      { AIC::ATT_UNIT_PATROL_GROUPS_COUNT       ->getValue() , 0  },
      { AIC::ATT_UNIT_BACKUP_MAX                ->getValue() , 0  },
      { AIC::ATT_UNIT_BACKUP_GROUPS_COUNT       ->getValue() , 0  },
      { AIC::ATT_UNIT_ENGAGE_MAX                ->getValue() , 0  },
      { AIC::ATT_UNIT_SIEGE_DEF_MAX             ->getValue() , 0  },
      { AIC::ATT_UNIT_SIEGE_DEF_GROUPS_COUNT    ->getValue() , 0  },
      { AIC::ATT_MAX_DEFAULT                    ->getValue() , 0  },
      
      { AIC::CRITICAL_POPULARITY                ->getValue() , 1  },
      { AIC::LOWEST_POPULARITY                  ->getValue() , 1  },
      { AIC::HIGHEST_POPULARITY                 ->getValue() , 1  },
      
      { AIC::TAXES_MIN                          ->getValue() , 2  },
      { AIC::TAXES_MAX                          ->getValue() , 2  },
   
      { AIC::FARM_1                             ->getValue() , 3  },
      { AIC::FARM_2                             ->getValue() , 3  },
      { AIC::FARM_3                             ->getValue() , 3  },
      { AIC::FARM_4                             ->getValue() , 3  },
      { AIC::FARM_5                             ->getValue() , 3  },
      { AIC::FARM_6                             ->getValue() , 3  },
      { AIC::FARM_7                             ->getValue() , 3  },
      { AIC::FARM_8                             ->getValue() , 3  },
      
      { AIC::MAX_QUARRIES                       ->getValue() , 4  },
      { AIC::MAX_IRONMINES                      ->getValue() , 4  },
      { AIC::MAX_WOODCUTTERS                    ->getValue() , 4  },
      { AIC::MAX_PITCHRIGS                      ->getValue() , 4  },
      { AIC::MAX_FARMS                          ->getValue() , 4  },
     
      { AIC::MINIMUM_APPLES                     ->getValue() , 5  },
      { AIC::MINIMUM_CHEESE                     ->getValue() , 5  },
      { AIC::MINIMUM_BREAD                      ->getValue() , 5  },
      { AIC::MINIMUM_WHEAT                      ->getValue() , 5  },
      { AIC::MINIMUM_HOP                        ->getValue() , 5  },
    
      { AIC::BLACKSMITH_SETTING                 ->getValue() , 6  },
      { AIC::FLETCHER_SETTING                   ->getValue() , 7  },
      { AIC::POLETURNER_SETTING                 ->getValue() , 8  },
    
      { AIC::SELL_RESOURCE_01                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_02                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_03                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_04                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_05                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_06                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_07                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_08                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_09                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_10                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_11                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_12                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_13                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_14                   ->getValue() , 9  },
      { AIC::SELL_RESOURCE_15                   ->getValue() , 9  },

      { AIC::RECRUIT_PROB_DEF_DEFAULT           ->getValue() , 10 },
      { AIC::RECRUIT_PROB_DEF_WEAK              ->getValue() , 10 },
      { AIC::RECRUIT_PROB_DEF_STRONG            ->getValue() , 10 },
      { AIC::RECRUIT_PROB_RAID_DEFAULT          ->getValue() , 10 },
      { AIC::RECRUIT_PROB_RAID_WEAK             ->getValue() , 10 },
      { AIC::RECRUIT_PROB_RAID_STRONG           ->getValue() , 10 },
      { AIC::RECRUIT_PROB_ATTACK_DEFAULT        ->getValue() , 10 },
      { AIC::RECRUIT_PROB_ATTACK_WEAK           ->getValue() , 10 },
      { AIC::RECRUIT_PROB_ATTACK_STRONG         ->getValue() , 10 },
      { AIC::ATT_FORCE_RALLY_PERCENTAGE         ->getValue() , 10 },

      { AIC::SORTIE_UNIT_RANGED                 ->getValue() , 11 },
      { AIC::SORTIE_UNIT_MELEE                  ->getValue() , 11 },
      { AIC::DEF_UNIT_1                         ->getValue() , 11 },
      { AIC::DEF_UNIT_2                         ->getValue() , 11 },
      { AIC::DEF_UNIT_3                         ->getValue() , 11 },
      { AIC::DEF_UNIT_4                         ->getValue() , 11 },
      { AIC::DEF_UNIT_5                         ->getValue() , 11 },
      { AIC::DEF_UNIT_6                         ->getValue() , 11 },
      { AIC::DEF_UNIT_7                         ->getValue() , 11 },
      { AIC::DEF_UNIT_8                         ->getValue() , 11 },
      { AIC::RAID_UNIT_1                        ->getValue() , 11 },
      { AIC::RAID_UNIT_2                        ->getValue() , 11 },
      { AIC::RAID_UNIT_3                        ->getValue() , 11 },
      { AIC::RAID_UNIT_4                        ->getValue() , 11 },
      { AIC::RAID_UNIT_5                        ->getValue() , 11 },
      { AIC::RAID_UNIT_6                        ->getValue() , 11 },
      { AIC::RAID_UNIT_7                        ->getValue() , 11 },
      { AIC::RAID_UNIT_8                        ->getValue() , 11 },
      { AIC::ATT_UNIT_2                         ->getValue() , 11 },
      { AIC::ATT_UNIT_PATROL                    ->getValue() , 11 },
      { AIC::ATT_UNIT_BACKUP                    ->getValue() , 11 },
      { AIC::ATT_UNIT_ENGAGE                    ->getValue() , 11 },
      { AIC::ATT_UNIT_SIEGE_DEF                 ->getValue() , 11 },
      { AIC::ATT_UNIT_MAIN_1                    ->getValue() , 11 },
      { AIC::ATT_UNIT_MAIN_2                    ->getValue() , 11 },
      { AIC::ATT_UNIT_MAIN_3                    ->getValue() , 11 },
      { AIC::ATT_UNIT_MAIN_4                    ->getValue() , 11 },

      { AIC::DEF_DIGGING_UNIT                   ->getValue() , 12 },
      { AIC::ATT_DIGGING_UNIT                   ->getValue() , 12 },

      { AIC::OUTER_PATROL_GROUPS_MOVE           ->getValue() , 13 },

      { AIC::HARASSING_SIEGE_ENGINE_1           ->getValue() , 14 },
      { AIC::HARASSING_SIEGE_ENGINE_2           ->getValue() , 14 },
      { AIC::HARASSING_SIEGE_ENGINE_3           ->getValue() , 14 },
      { AIC::HARASSING_SIEGE_ENGINE_4           ->getValue() , 14 },
      { AIC::HARASSING_SIEGE_ENGINE_5           ->getValue() , 14 },
      { AIC::HARASSING_SIEGE_ENGINE_6           ->getValue() , 14 },
      { AIC::HARASSING_SIEGE_ENGINE_7           ->getValue() , 14 },
      { AIC::HARASSING_SIEGE_ENGINE_8           ->getValue() , 14 },

      { AIC::HARASSING_SIEGE_ENGINES_MAX        ->getValue() , 15 },
 
      { AIC::SIEGE_ENGINE_1                     ->getValue() , 16 },
      { AIC::SIEGE_ENGINE_2                     ->getValue() , 16 },
      { AIC::SIEGE_ENGINE_3                     ->getValue() , 16 },
      { AIC::SIEGE_ENGINE_4                     ->getValue() , 16 },
      { AIC::SIEGE_ENGINE_5                     ->getValue() , 16 },
      { AIC::SIEGE_ENGINE_6                     ->getValue() , 16 },
      { AIC::SIEGE_ENGINE_7                     ->getValue() , 16 },
      { AIC::SIEGE_ENGINE_8                     ->getValue() , 16 },

      { AIC::ATT_MAIN_GROUPS_COUNT              ->getValue() , 17 },

      { AIC::TARGET_CHOICE                      ->getValue() , 18 }
    };

    auto it{ switchMap.find(field->getValue()) };
    if (it == switchMap.end())
    {
      LOG(ERROR) << "AICLoad: Personality value test received non valid field. This should not happen. Is a handler missing?";
      return false;
    }

    switch (it->second)
    {
      case 0: // full 0 to int range
        return validIntRangeHelper(field, value, validValue, 0, int32Max, "0 to 2 ^ 31 - 1");
      case 1: // up to 10000
        return validIntRangeHelper(field, value, validValue, 0, 10000, "0 to 10000");
      case 2: // tax -> despite saying 11 is max, vanilla includes taxesMax up to 12?
        return validIntRangeHelper(field, value, validValue, 0, 12, "0 to 11"); // cheating to allow vanilla
      case 3: // Farms
        return validEnumHelper<Farm>(field, value, validValue, "farm");
      case 4: // max buildings
        return validIntRangeHelper(field, value, validValue, 0, int32Max, "1 to 2^31 -1"); // cheating to allow about pitchrig 0
      case 5: // min food kinda
        return validIntRangeHelper(field, value, validValue, -1, int32Max, "-1 to 2^31 -1");
      case 6:
        return validEnumHelper<Smith>(field, value, validValue, "blacksmith setting");
      case 7:
        return validEnumHelper<Fletcher>(field, value, validValue, "fletcher setting");
      case 8:
        return validEnumHelper<Pole>(field, value, validValue, "poleturner setting");
      case 9: // resource sell
        return validEnumHelper<Resource>(field, value, validValue, "resource");
      case 10: // percent stuff
        return validIntRangeHelper(field, value, validValue, 0, 100, "0 to 100");
      case 11:
        return validEnumHelper<Unit>(field, value, validValue, "unit");
      case 12:
        return validEnumHelper<DUnit>(field, value, validValue, "digging unit");
      case 13: // this thing has to stay..., outer troups move
      {
        if (value.is_boolean())
        {
          validValue = static_cast<int32_t>(value.get<bool>());
          return true;
        }

        if (!value.is_string())
        {
          LOG(WARNING) << "Field '" << field->getName() << "' did not contain a string or boolean.";
          return false;
        }

        bool boolValue;
        std::string boolString{ value.get<std::string>() };
        try
        {
          // may produce errors (stringcode, etc.), however, the error will hopefully be catched
          std::transform(boolString.begin(), boolString.end(), boolString.begin(), [](char c)
          {
            return static_cast<char>(std::tolower(c));
          });
          if (boolString == "true")
          {
            boolValue = true;
          }
          else if (boolString == "false")
          {
            boolValue = false;
          }
          else
          {
            throw std::exception();
          }
        }
        catch (const std::exception&) // hope this catches
        {
          LOG(WARNING) << "Field '" << field->getName() << "' is not a valid boolean string.";
          return false;
        }

        validValue = static_cast<int32_t>(boolValue);
        return true;
      }
      case 14: // HarassingSiegeEngine
        return validEnumHelper<HSE>(field, value, validValue, "harassing siege engine");
      case 15: // HarassingSiegeEngine max
        return validIntRangeHelper(field, value, validValue, 0, 10, "0 to 10");
      case 16: // SiegeEngine
        return validEnumHelper<SE>(field, value, validValue, "siege engine");
      case 17: // ATT_MAIN_GROUPS_COUNT
        // cheating... I think I also remember that someone wrote there is something broken, and more then 3 create issues..
        return validIntRangeHelper(field, value, validValue, 0, 5, "0 to 3");
      case 18:
        return validEnumHelper<Target>(field, value, validValue, "targeting type");
      default:
      {
        LOG(ERROR) << "AICLoad: Personality value test found no valid handling. This should not happen. Is a handler missing?";
        return false;
      }
    }
  }


  std::unique_ptr<std::unordered_map<int32_t, int32_t>> AICLoad::loadAICFile(const std::string &name, const bool fileRelativeToMod) const
  {
    return loadAICFile(name, fileRelativeToMod, 0);
  }


  // returns empty pointer in unrecoverable error case
  std::unique_ptr<std::unordered_map<int32_t, int32_t>> AICLoad::loadAICFile(const std::string &name, const bool fileRelativeToMod, const size_t mapInitSize) const
  {
    auto aicMap{ std::make_unique<std::unordered_map<int32_t, int32_t>>() };
    if (mapInitSize > 0)
    {
      aicMap->reserve(mapInitSize);
    }

    bool runningOk{ true };

    // NOTE: key functions etc. happen from the folder of stronghold, not the plugin
    std::string filePath;
    if (!(fileRelativeToMod || relModPath.empty()))
    {
      filePath += relModPath + "/";
    }
    filePath += aicFolder.empty() ? name : aicFolder + "/" + name;

    try
    {
      std::ifstream aicStream(filePath); // reading config from file

      if (!aicStream.good())
      {
        LOG(WARNING) << "AICLoad: Unable to load file '" << name << "'. Is the name or the aic folder wrong?";
        return std::unique_ptr<std::unordered_map<int32_t, int32_t>>();
      }

      Json aicJson = Json::parse(aicStream);
 
      auto fileIt = aicJson.find("AICharacters");
      if (fileIt == aicJson.end())
      {
        LOG(WARNING) << "AICLoad: Did not found the field 'AICharacters'. Unable to load aic '" << name << "'.";
        return std::unique_ptr<std::unordered_map<int32_t, int32_t>>();
      }

      Json &aicArray = fileIt.value();
      for (const auto& charAIC : aicArray)
      {
        auto charIt = charAIC.find("Name");
        if (charIt == charAIC.end())
        {
          LOG(WARNING) << "AICLoad: Did not found the field 'Name' in one of the character-objects. Skipping the object.";
          continue;
        }

        const std::string& aiNameString{ charIt.value().get_ref<const Json::string_t&>() };
        AINameEnum aiName{ AIName::GetEnumByName(charIt.value().get_ref<const Json::string_t&>()) };
        if (!aiName || aiName == AIName::NONE)
        {
          LOG(WARNING) << "AICLoad: The character name '" << aiNameString << "' is no valid name. Skipping the object.";
          continue;
        }

        auto perIt = charAIC.find("Personality");
        if (perIt == charAIC.end())
        {
          LOG(WARNING) << "AICLoad: Did not found the field 'Personality' in object of character '" << aiNameString << "'. Skipping the object.";
          continue;
        }

        // it might also be possible to directly convert to a multimap, but the info which values are strange might be harder to receive
        for (auto& valuePair : perIt.value().items())
        {
          AICEnum aicField{ AIC::GetEnumByName(valuePair.key()) };
          if (!aicField || aicField == AIC::NONE)
          {
            LOG(WARNING) << "AICLoad: The field '" << valuePair.key() << "' of the character '" << aiNameString << "' is no valid AIC field. "
              << "Skipping the field.";
            continue;
          }

          int32_t value;
          if (!isValidPersonalityValue(aicField, valuePair.value(), value))
          {
            LOG(WARNING) << "AICLoad: The value of field '" << valuePair.key() << "' of the character '" << aiNameString << "' is not valid. "
              << "Skipping the field.";
            continue;
          }

          (*aicMap)[getAICFieldIndex(aiName, aicField)] = value;
        }
      }

      BltOverlay::sendToConsole("Loaded AIC: " + name, el::Level::Info);
    }
    catch (const Json::parse_error& o_O)
    {
      LOG(ERROR) << "AIC parse error: " << o_O.what();
      runningOk = false;
    }
    catch (const std::exception& o_O)
    {
      LOG(ERROR) << "Error during aic load: " << o_O.what();
      runningOk = false;
    }

    // extra if big problem occured
    if (!runningOk)
    {
      LOG(WARNING) << "Encountered unsolveable problems during load of AIC '" << name << "'. File is ignored.";
      aicMap.reset();
    }

    return aicMap;
  }

  std::vector<AddressRequest> AICLoad::usedAddresses{
    {Address::AIC_IN_MEMORY, {{Version::NONE, 10816}}, true, AddressRisk::WARNING}
  };
}