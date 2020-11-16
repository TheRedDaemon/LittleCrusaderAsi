
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
                std::string(AIName::Rat->getName()),
                nullptr,
                false
              )
                .createMenu<ChoiceInputMenu, false>(
                  std::string(AIC::Farm1->getName()),
                  nullptr,
                  getNameOrNULL<Farm>(vanillaAIC[getAICFieldIndex(AIName::Rat, AIC::Farm1)]),
                  std::vector<std::pair<std::string, int32_t>>{
                    { Farm::NoFarm->getName(), Farm::NoFarm->getValue() },
                    { Farm::WheatFarm->getName(), Farm::WheatFarm->getValue() },
                    { Farm::HopFarm->getName(), Farm::HopFarm->getValue() },
                    { Farm::AppleFarm->getName(), Farm::AppleFarm->getValue() },
                    { Farm::DairyFarm->getName(), Farm::DairyFarm->getValue() }
                  },
                  [this](int32_t newValue, std::string& resultMessage, bool onlyUpdateCurrent, std::string& currentValue)
                  {
                    if (onlyUpdateCurrent)
                    {
                      currentValue = getNameOrNULL<Farm>((*(this->aicMemoryPtr))[getAICFieldIndex(AIName::Rat, AIC::Farm1)]);
                      return false;
                    }

                    // no need for extra check -> should be ok by definition
                    if (this->setPersonalityValueUnchecked(AIName::Rat, AIC::Farm1, newValue))
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
                )
                .createMenu<FreeInputMenu, false>(
                  std::string(AIC::MinimumHop->getName()),
                  nullptr,
                  true,
                  std::to_string(vanillaAIC[getAICFieldIndex(AIName::Rat, AIC::MinimumHop)]),
                  [this](const std::string &currentInput, std::string& resultMessage,
                    bool onlyUpdateCurrent, std::string& currentValue)
                  {
                    if (!onlyUpdateCurrent)
                    {
                      bool error{ false };
                      int32_t inputValue{ 0 };
                      try
                      {
                        inputValue = std::stoi(currentInput);
                      }
                      catch (const std::invalid_argument&)
                      {
                        resultMessage = "Invalid input.";
                        error = true;
                      }
                      catch (const std::out_of_range&)
                      {
                        resultMessage = "Number to big.";
                        error = true;
                      }
                      catch (const std::exception&)
                      {
                        resultMessage = "Parse error.";
                        error = true;
                      }

                      // need extra check
                      if (!error)
                      {
                        if (this->setPersonalityValue(AIName::Rat, AIC::MinimumHop, inputValue))
                        {
                          resultMessage = "Success.";
                        }
                        else
                        {
                          resultMessage = "Failed. See log.";
                        }
                      }
                    }

                    currentValue = std::to_string((*(this->aicMemoryPtr))[getAICFieldIndex(AIName::Rat, AIC::MinimumHop)]);
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
    if (aiName == AIName::None || field == AIC::None || !initialized)
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
    if (aiName == AIName::None || field == AIC::None || !initialized)
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
      BltOverlay::sendToConsole("Field '" + field->getName() + "' did not contain a string.", el::Level::Warning);
      return false;
    }


    typename T::EnumType valueEnum{ T::GetEnumByName(value.get_ref<const Json::string_t&>()) };
    if (!valueEnum || valueEnum == T::None) // every enum should have a none
    {
      BltOverlay::sendToConsole("Field '" + field->getName() + "' did not contain a valid " + warningEnd + ".",
        el::Level::Warning);
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
      BltOverlay::sendToConsole("Field '" + field->getName() + "' did not contain a number.", el::Level::Warning);
      return false;
    }

    int32_t numValue = value.get<int32_t>();
    if (numValue < lowerEndIncl || numValue > upperEndIncl)
    {
      BltOverlay::sendToConsole("Field '" + field->getName() + "' is not a number " + warningEndRange + ".", el::Level::Warning);
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
      { AIC::Unknown000                       ->getValue() , 0  },
      { AIC::Unknown001                       ->getValue() , 0  },
      { AIC::Unknown002                       ->getValue() , 0  },
      { AIC::Unknown003                       ->getValue() , 0  },
      { AIC::Unknown004                       ->getValue() , 0  },
      { AIC::Unknown005                       ->getValue() , 0  },
      { AIC::Unknown011                       ->getValue() , 0  },
      { AIC::PopulationPerFarm                ->getValue() , 0  },
      { AIC::PopulationPerWoodcutter          ->getValue() , 0  },
      { AIC::PopulationPerQuarry              ->getValue() , 0  },
      { AIC::PopulationPerIronmine            ->getValue() , 0  },
      { AIC::PopulationPerPitchrig            ->getValue() , 0  },
      { AIC::BuildInterval                    ->getValue() , 0  },
      { AIC::ResourceRebuildDelay             ->getValue() , 0  },
      { AIC::MaxFood                          ->getValue() , 0  },
      { AIC::TradeAmountFood                  ->getValue() , 0  },
      { AIC::TradeAmountEquipment             ->getValue() , 0  },
      { AIC::Unknown040                       ->getValue() , 0  },
      { AIC::MinimumGoodsRequiredAfterTrade   ->getValue() , 0  },
      { AIC::DoubleRationsFoodThreshold       ->getValue() , 0  },
      { AIC::MaxWood                          ->getValue() , 0  },
      { AIC::MaxStone                         ->getValue() , 0  },
      { AIC::MaxResourceOther                 ->getValue() , 0  },
      { AIC::MaxEquipment                     ->getValue() , 0  },
      { AIC::MaxBeer                          ->getValue() , 0  },
      { AIC::MaxResourceVariance              ->getValue() , 0  },
      { AIC::RecruitGoldThreshold             ->getValue() , 0  },
      { AIC::DefWallPatrolRallytime           ->getValue() , 0  },
      { AIC::DefWallPatrolGroups              ->getValue() , 0  },
      { AIC::DefSiegeEngineGoldThreshold      ->getValue() , 0  },
      { AIC::DefSiegeEngineBuildDelay         ->getValue() , 0  },
      { AIC::Unknown072                       ->getValue() , 0  },
      { AIC::Unknown073                       ->getValue() , 0  },
      { AIC::SortieUnitRangedMin              ->getValue() , 0  },
      { AIC::SortieUnitMeleeMin               ->getValue() , 0  },
      { AIC::DefDiggingUnitMax                ->getValue() , 0  },
      { AIC::RecruitInterval                  ->getValue() , 0  },
      { AIC::RecruitIntervalWeak              ->getValue() , 0  },
      { AIC::RecruitIntervalStrong            ->getValue() , 0  },
      { AIC::DefTotal                         ->getValue() , 0  },
      { AIC::OuterPatrolGroupsCount           ->getValue() , 0  },
      { AIC::OuterPatrolRallyDelay            ->getValue() , 0  },
      { AIC::DefWalls                         ->getValue() , 0  },
      { AIC::RaidUnitsBase                    ->getValue() , 0  },
      { AIC::RaidUnitsRandom                  ->getValue() , 0  },
      { AIC::Unknown124                       ->getValue() , 0  },
      { AIC::AttForceBase                     ->getValue() , 0  },
      { AIC::AttForceRandom                   ->getValue() , 0  },
      { AIC::AttForceSupportAllyThreshold     ->getValue() , 0  },
      { AIC::Unknown129                       ->getValue() , 0  },
      { AIC::Unknown130                       ->getValue() , 0  },
      { AIC::AttUnitPatrolRecommandDelay      ->getValue() , 0  },
      { AIC::Unknown132                       ->getValue() , 0  },
      { AIC::CowThrowInterval                 ->getValue() , 0  },
      { AIC::Unknown142                       ->getValue() , 0  },
      { AIC::AttMaxEngineers                  ->getValue() , 0  },
      { AIC::AttDiggingUnitMax                ->getValue() , 0  },
      { AIC::AttUnit2Max                      ->getValue() , 0  },
      { AIC::AttMaxAssassins                  ->getValue() , 0  },
      { AIC::AttMaxLaddermen                  ->getValue() , 0  },
      { AIC::AttMaxTunnelers                  ->getValue() , 0  },
      { AIC::AttUnitPatrolMax                 ->getValue() , 0  },
      { AIC::AttUnitPatrolGroupsCount         ->getValue() , 0  },
      { AIC::AttUnitBackupMax                 ->getValue() , 0  },
      { AIC::AttUnitBackupGroupsCount         ->getValue() , 0  },
      { AIC::AttUnitEngageMax                 ->getValue() , 0  },
      { AIC::AttUnitSiegeDefMax               ->getValue() , 0  },
      { AIC::AttUnitSiegeDefGroupsCount       ->getValue() , 0  },
      { AIC::AttMaxDefault                    ->getValue() , 0  },
      
      { AIC::CriticalPopularity               ->getValue() , 1  },
      { AIC::LowestPopularity                 ->getValue() , 1  },
      { AIC::HighestPopularity                ->getValue() , 1  },
      
      { AIC::TaxesMin                         ->getValue() , 2  },
      { AIC::TaxesMax                         ->getValue() , 2  },
   
      { AIC::Farm1                            ->getValue() , 3  },
      { AIC::Farm2                            ->getValue() , 3  },
      { AIC::Farm3                            ->getValue() , 3  },
      { AIC::Farm4                            ->getValue() , 3  },
      { AIC::Farm5                            ->getValue() , 3  },
      { AIC::Farm6                            ->getValue() , 3  },
      { AIC::Farm7                            ->getValue() , 3  },
      { AIC::Farm8                            ->getValue() , 3  },
      
      { AIC::MaxQuarries                      ->getValue() , 4  },
      { AIC::MaxIronmines                     ->getValue() , 4  },
      { AIC::MaxWoodcutters                   ->getValue() , 4  },
      { AIC::MaxPitchrigs                     ->getValue() , 4  },
      { AIC::MaxFarms                         ->getValue() , 4  },
     
      { AIC::MinimumApples                    ->getValue() , 5  },
      { AIC::MinimumCheese                    ->getValue() , 5  },
      { AIC::MinimumBread                     ->getValue() , 5  },
      { AIC::MinimumWheat                     ->getValue() , 5  },
      { AIC::MinimumHop                       ->getValue() , 5  },
    
      { AIC::BlacksmithSetting                ->getValue() , 6  },
      { AIC::FletcherSetting                  ->getValue() , 7  },
      { AIC::PoleturnerSetting                ->getValue() , 8  },
    
      { AIC::SellResource01                   ->getValue() , 9  },
      { AIC::SellResource02                   ->getValue() , 9  },
      { AIC::SellResource03                   ->getValue() , 9  },
      { AIC::SellResource04                   ->getValue() , 9  },
      { AIC::SellResource05                   ->getValue() , 9  },
      { AIC::SellResource06                   ->getValue() , 9  },
      { AIC::SellResource07                   ->getValue() , 9  },
      { AIC::SellResource08                   ->getValue() , 9  },
      { AIC::SellResource09                   ->getValue() , 9  },
      { AIC::SellResource10                   ->getValue() , 9  },
      { AIC::SellResource11                   ->getValue() , 9  },
      { AIC::SellResource12                   ->getValue() , 9  },
      { AIC::SellResource13                   ->getValue() , 9  },
      { AIC::SellResource14                   ->getValue() , 9  },
      { AIC::SellResource15                   ->getValue() , 9  },

      { AIC::RecruitProbDefDefault            ->getValue() , 10 },
      { AIC::RecruitProbDefWeak               ->getValue() , 10 },
      { AIC::RecruitProbDefStrong             ->getValue() , 10 },
      { AIC::RecruitProbRaidDefault           ->getValue() , 10 },
      { AIC::RecruitProbRaidWeak              ->getValue() , 10 },
      { AIC::RecruitProbRaidStrong            ->getValue() , 10 },
      { AIC::RecruitProbAttackDefault         ->getValue() , 10 },
      { AIC::RecruitProbAttackWeak            ->getValue() , 10 },
      { AIC::RecruitProbAttackStrong          ->getValue() , 10 },
      { AIC::AttForceRallyPercentage          ->getValue() , 10 },

      { AIC::SortieUnitRanged                 ->getValue() , 11 },
      { AIC::SortieUnitMelee                  ->getValue() , 11 },
      { AIC::DefUnit1                         ->getValue() , 11 },
      { AIC::DefUnit2                         ->getValue() , 11 },
      { AIC::DefUnit3                         ->getValue() , 11 },
      { AIC::DefUnit4                         ->getValue() , 11 },
      { AIC::DefUnit5                         ->getValue() , 11 },
      { AIC::DefUnit6                         ->getValue() , 11 },
      { AIC::DefUnit7                         ->getValue() , 11 },
      { AIC::DefUnit8                         ->getValue() , 11 },
      { AIC::RaidUnit1                        ->getValue() , 11 },
      { AIC::RaidUnit2                        ->getValue() , 11 },
      { AIC::RaidUnit3                        ->getValue() , 11 },
      { AIC::RaidUnit4                        ->getValue() , 11 },
      { AIC::RaidUnit5                        ->getValue() , 11 },
      { AIC::RaidUnit6                        ->getValue() , 11 },
      { AIC::RaidUnit7                        ->getValue() , 11 },
      { AIC::RaidUnit8                        ->getValue() , 11 },
      { AIC::AttUnit2                         ->getValue() , 11 },
      { AIC::AttUnitPatrol                    ->getValue() , 11 },
      { AIC::AttUnitBackup                    ->getValue() , 11 },
      { AIC::AttUnitEngage                    ->getValue() , 11 },
      { AIC::AttUnitSiegeDef                  ->getValue() , 11 },
      { AIC::AttUnitMain1                     ->getValue() , 11 },
      { AIC::AttUnitMain2                     ->getValue() , 11 },
      { AIC::AttUnitMain3                     ->getValue() , 11 },
      { AIC::AttUnitMain4                     ->getValue() , 11 },

      { AIC::DefDiggingUnit                   ->getValue() , 12 },
      { AIC::AttDiggingUnit                   ->getValue() , 12 },

      { AIC::OuterPatrolGroupsMove            ->getValue() , 13 },

      { AIC::HarassingSiegeEngine1            ->getValue() , 14 },
      { AIC::HarassingSiegeEngine2            ->getValue() , 14 },
      { AIC::HarassingSiegeEngine3            ->getValue() , 14 },
      { AIC::HarassingSiegeEngine4            ->getValue() , 14 },
      { AIC::HarassingSiegeEngine5            ->getValue() , 14 },
      { AIC::HarassingSiegeEngine6            ->getValue() , 14 },
      { AIC::HarassingSiegeEngine7            ->getValue() , 14 },
      { AIC::HarassingSiegeEngine8            ->getValue() , 14 },

      { AIC::HarassingSiegeEnginesMax         ->getValue() , 15 },
 
      { AIC::SiegeEngine1                     ->getValue() , 16 },
      { AIC::SiegeEngine2                     ->getValue() , 16 },
      { AIC::SiegeEngine3                     ->getValue() , 16 },
      { AIC::SiegeEngine4                     ->getValue() , 16 },
      { AIC::SiegeEngine5                     ->getValue() , 16 },
      { AIC::SiegeEngine6                     ->getValue() , 16 },
      { AIC::SiegeEngine7                     ->getValue() , 16 },
      { AIC::SiegeEngine8                     ->getValue() , 16 },

      { AIC::AttMainGroupsCount               ->getValue() , 17 },

      { AIC::TargetChoice                     ->getValue() , 18 }
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
          BltOverlay::sendToConsole("Field '" + field->getName() + "' did not contain a string or boolean.",
            el::Level::Warning);
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
          BltOverlay::sendToConsole("Field '" + field->getName() + "' is not a valid boolean string.",
            el::Level::Warning);
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
        if (!aiName || aiName == AIName::None)
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
          if (!aicField || aicField == AIC::None)
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