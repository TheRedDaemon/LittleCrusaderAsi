
#include <algorithm>

#include "aicLoad.h"

namespace modclasses
{
  AICLoad::AICLoad(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
    // TODO?: it might be an idea to create a more general function for simple json value reads, but this depends on how samey they are...

    auto confIt = config.find("startState");
    if (confIt != config.end())
    {
      isChanged = confIt.value().get<bool>();
    }
    else
    {
      LOG(WARNING) << "AICLoad: No 'startState' found. Defaults to 'false'. Will not load AIC in game until requested.";
    }

    confIt = config.find("aicFolder");
    if (confIt != config.end())
    {
      aicFolder = confIt.value().get<std::string>();
    }
    else
    {
      LOG(WARNING) << "AICLoad: No 'aicFolder' found. Mod will look for the AICs in the mod root folder.";
    }

    confIt = config.find("loadList");
    if (confIt != config.end())
    {
      loadList = confIt.value().get<std::vector<std::string>>();
    }
    else
    {
      LOG(WARNING) << "AICLoad: No 'loadList' found. No AICs will be loaded.";
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
    return { ModType::ADDRESS_RESOLVER, ModType::KEYBOARD_INTERCEPTOR };
  }

  bool AICLoad::initialize()
  {
    auto addressResolver = getMod<AddressResolver>();
    auto keyInterceptor = getMod<KeyboardInterceptor>();

    if (addressResolver && keyInterceptor)
    {
      // request addresses
      if (addressResolver->requestAddresses(usedAddresses, *this))
      {
        // reserve map space (using number of aic values
        loadedAICValues.reserve(2704);

        // get pointer
        aicMemoryPtr = addressResolver->getAddressPointer<int32_t[2704]>(Address::AIC_IN_MEMORY, *this);

        // load aic data
        for (auto& aicName : loadList)
        {
          loadAICFile(aicName);
        }

        initialized = true;
      }
    }

    if (!initialized)
    {
      LOG(WARNING) << "AICLoad was not initialized.";
    }
    else
    {
      LOG(INFO) << "AICLoad initialized.";
    }

    return initialized;
  }

  void AICLoad::firstThreadAttachAfterModAttachEvent()
  {
    if (initialized)
    {
      // save vanilla values
      std::copy(std::begin(*aicMemoryPtr), std::end(*aicMemoryPtr), vanillaAIC.begin());

      if (vanillaAIC.at(0) != 12)
      {
        LOG(WARNING) << "First thread attached event was seemingly triggerd before AIC initialisation. AICLoad features are likely unreliable."
          << "Indicator: first int is not a 12." ;
      }


      // if it works, load data here
      // in theory, it should not cause direct harm -> if this fires to early, stronghold will place its own values over them

      LOG(INFO) << "AICLoad run through 'first thread attached'-event.";
    }
  }

  std::vector<AddressRequest> AICLoad::usedAddresses{
    {Address::AIC_IN_MEMORY, {{Version::NONE, 10816}}, true, AddressRisk::WARNING}
  };

  // keyboard functions

  // private functions

  const int32_t AICLoad::getAICFieldIndex(const AICharacterName aiName, const AIC field) const
  {
    // 169 for every ai
    return static_cast<int32_t>(aiName) * 169 + static_cast<int32_t>(field);
  }

  // small helper

  const std::string getAICString(const AIPersonalityFieldsEnum field)
  {
    Json jField = field;
    return jField.get<std::string>();
  }

  // enums require NONE at least
  template<typename T>
  const bool validEnumHelper(const AIPersonalityFieldsEnum field, const Json &value, int32_t &validValue, const std::string& warningEnd)
  {
    if (!value.is_string())
    {
      LOG(WARNING) << "Field '" << getAICString(field) << "' did not contain a string.";
      return false;
    }

    T enumValue{ value.get<T>() };
    if (enumValue == T::NONE) // every enum should have a none
    {
      LOG(WARNING) << "Field '" << getAICString(field) << "' did not contain a valid " << warningEnd << ".";
      return false;
    }

    validValue = static_cast<int32_t>(enumValue);
    return true;
  }

  // does not even need template
  const bool validIntRangeHelper(const AIPersonalityFieldsEnum field, const Json &value, int32_t &validValue, const int32_t lowerEndIncl,
                                 const int32_t upperEndIncl, const std::string& warningEndRange)
  {
    if (!value.is_number_integer())
    {
      LOG(WARNING) << "Field '" << getAICString(field) << "' did not contain a number.";
      return false;
    }

    int32_t numValue = value.get<int32_t>();
    if (numValue < lowerEndIncl || numValue > upperEndIncl)
    {
      LOG(WARNING) << "Field '" << getAICString(field) << "' is not a number " << warningEndRange << ".";
      return false;
    }

    validValue = numValue;
    return true;
  }

  // will use switch statement to decide, since many have the same range
  // could be a bit smaller...
  // smoe are marked with cheating, which mean I changed the checks so the vanilla values pass... why is this so?
  const bool AICLoad::isValidPersonalityValue(const AIC field, const Json &value, int32_t &validValue)
  {
    static const int32_t int32Max{ (std::numeric_limits<int32_t>::max)() };

    // only basic value tests
    switch (field)
    {
      case AIC::UNKNOWN_000:
      case AIC::UNKNOWN_001:
      case AIC::UNKNOWN_002:
      case AIC::UNKNOWN_003:
      case AIC::UNKNOWN_004:
      case AIC::UNKNOWN_005:
      case AIC::UNKNOWN_011:
      case AIC::POPULATION_PER_FARM:
      case AIC::POPULATION_PER_WOODCUTTER:
      case AIC::POPULATION_PER_QUARRY:
      case AIC::POPULATION_PER_IRONMINE:
      case AIC::POPULATION_PER_PITCHRIG:
      case AIC::BUILD_INTERVAL:
      case AIC::RESOURCE_REBUILD_DELAY:
      case AIC::MAX_FOOD:
      case AIC::TRADE_AMOUNT_FOOD:
      case AIC::TRADE_AMOUNT_EQUIPMENT:
      case AIC::UNKNOWN_040:
      case AIC::MINIMUM_GOODS_REQUIRED_AFTER_TRADE:
      case AIC::DOUBLE_RATIONS_FOOD_THRESHOLD:
      case AIC::MAX_WOOD:
      case AIC::MAX_STONE:
      case AIC::MAX_RESOURCE_OTHER:
      case AIC::MAX_EQUIPMENT:
      case AIC::MAX_BEER:
      case AIC::MAX_RESOURCE_VARIANCE:
      case AIC::RECRUIT_GOLD_THRESHOLD:
      case AIC::DEF_WALLPATROL_RALLYTIME:
      case AIC::DEF_WALLPATROL_GROUPS:
      case AIC::DEF_SIEGEENGINE_GOLD_THRESHOLD:
      case AIC::DEF_SIEGEENGINE_BUILD_DELAY:
      case AIC::UNKNOWN_072:
      case AIC::UNKNOWN_073:
      case AIC::SORTIE_UNIT_RANGED_MIN:
      case AIC::SORTIE_UNIT_MELEE_MIN:
      case AIC::DEF_DIGGING_UNIT_MAX:
      case AIC::RECRUIT_INTERVAL:
      case AIC::RECRUIT_INTERVAL_WEAK:
      case AIC::RECRUIT_INTERVAL_STRONG:
      case AIC::DEF_TOTAL:
      case AIC::OUTER_PATROL_GROUPS_COUNT:
      case AIC::OUTER_PATROL_RALLY_DELAY:
      case AIC::DEF_WALLS:
      case AIC::RAID_UNITS_BASE:
      case AIC::RAID_UNITS_RANDOM:
      case AIC::UNKNOWN_124:
      case AIC::ATT_FORCE_BASE:
      case AIC::ATT_FORCE_RANDOM:
      case AIC::ATT_FORCE_SUPPORT_ALLY_THRESHOLD:
      case AIC::UNKNOWN_129:
      case AIC::UNKNOWN_130:
      case AIC::ATT_UNIT_PATROL_RECOMMAND_DELAY:
      case AIC::UNKNOWN_132:
      case AIC::COW_THROW_INTERVAL:
      case AIC::UNKNOWN_142:
      case AIC::ATT_MAX_ENGINEERS:
      case AIC::ATT_DIGGING_UNIT_MAX:
      case AIC::ATT_UNIT_2_MAX:
      case AIC::ATT_MAX_ASSASSINS:
      case AIC::ATT_MAX_LADDERMEN:
      case AIC::ATT_MAX_TUNNELERS:
      case AIC::ATT_UNIT_PATROL_MAX:
      case AIC::ATT_UNIT_PATROL_GROUPS_COUNT:
      case AIC::ATT_UNIT_BACKUP_MAX:
      case AIC::ATT_UNIT_BACKUP_GROUPS_COUNT:
      case AIC::ATT_UNIT_ENGAGE_MAX:
      case AIC::ATT_UNIT_SIEGE_DEF_MAX:
      case AIC::ATT_UNIT_SIEGE_DEF_GROUPS_COUNT:
      case AIC::ATT_MAX_DEFAULT:
        return validIntRangeHelper(field, value, validValue, 0, int32Max, "0 to 2 ^ 31 - 1");
      case AIC::CRITICAL_POPULARITY:
      case AIC::LOWEST_POPULARITY:
      case AIC::HIGHEST_POPULARITY:
        return validIntRangeHelper(field, value, validValue, 0, 10000, "0 to 10000");
      case AIC::TAXES_MIN:
      case AIC::TAXES_MAX:  // despite saying 11 is max, vanilla includes taxesMax up to 12?
        return validIntRangeHelper(field, value, validValue, 0, 12, "0 to 11"); // cheating
      case AIC::FARM_1:
      case AIC::FARM_2:
      case AIC::FARM_3:
      case AIC::FARM_4:
      case AIC::FARM_5:
      case AIC::FARM_6:
      case AIC::FARM_7:
      case AIC::FARM_8:
        return validEnumHelper<FarmBuilding>(field, value, validValue, "farm");
      case AIC::MAX_QUARRIES:
      case AIC::MAX_IRONMINES:
      case AIC::MAX_WOODCUTTERS:
      case AIC::MAX_PITCHRIGS:  // vanilla has at least the abbot with a 0 here?
      case AIC::MAX_FARMS:
        return validIntRangeHelper(field, value, validValue, 0, int32Max, "1 to 2^31 -1"); // cheating
      case AIC::MINIMUM_APPLES:
      case AIC::MINIMUM_CHEESE:
      case AIC::MINIMUM_BREAD:
      case AIC::MINIMUM_WHEAT:
      case AIC::MINIMUM_HOP:
        return validIntRangeHelper(field, value, validValue, -1, int32Max, "-1 to 2^31 -1");
      case AIC::BLACKSMITH_SETTING:
        return validEnumHelper<BlacksmithSetting>(field, value, validValue, "blacksmith setting");
      case AIC::FLETCHER_SETTING:
        return validEnumHelper<FletcherSetting>(field, value, validValue, "fletcher setting");
      case AIC::POLETURNER_SETTING:
        return validEnumHelper<PoleturnerSetting>(field, value, validValue, "poleturner setting");
      case AIC::SELL_RESOURCE_01:
      case AIC::SELL_RESOURCE_02:
      case AIC::SELL_RESOURCE_03:
      case AIC::SELL_RESOURCE_04:
      case AIC::SELL_RESOURCE_05:
      case AIC::SELL_RESOURCE_06:
      case AIC::SELL_RESOURCE_07:
      case AIC::SELL_RESOURCE_08:
      case AIC::SELL_RESOURCE_09:
      case AIC::SELL_RESOURCE_10:
      case AIC::SELL_RESOURCE_11:
      case AIC::SELL_RESOURCE_12:
      case AIC::SELL_RESOURCE_13:
      case AIC::SELL_RESOURCE_14:
      case AIC::SELL_RESOURCE_15:
        return validEnumHelper<Resource>(field, value, validValue, "resource");
      case AIC::RECRUIT_PROB_DEF_DEFAULT:
      case AIC::RECRUIT_PROB_DEF_WEAK:
      case AIC::RECRUIT_PROB_DEF_STRONG:
      case AIC::RECRUIT_PROB_RAID_DEFAULT:
      case AIC::RECRUIT_PROB_RAID_WEAK:
      case AIC::RECRUIT_PROB_RAID_STRONG:
      case AIC::RECRUIT_PROB_ATTACK_DEFAULT:
      case AIC::RECRUIT_PROB_ATTACK_WEAK:
      case AIC::RECRUIT_PROB_ATTACK_STRONG:
      case AIC::ATT_FORCE_RALLY_PERCENTAGE:
        return validIntRangeHelper(field, value, validValue, 0, 100, "0 to 100");
      case AIC::SORTIE_UNIT_RANGED:
      case AIC::SORTIE_UNIT_MELEE:
      case AIC::DEF_UNIT_1:
      case AIC::DEF_UNIT_2:
      case AIC::DEF_UNIT_3:
      case AIC::DEF_UNIT_4:
      case AIC::DEF_UNIT_5:
      case AIC::DEF_UNIT_6:
      case AIC::DEF_UNIT_7:
      case AIC::DEF_UNIT_8:
      case AIC::RAID_UNIT_1:
      case AIC::RAID_UNIT_2:
      case AIC::RAID_UNIT_3:
      case AIC::RAID_UNIT_4:
      case AIC::RAID_UNIT_5:
      case AIC::RAID_UNIT_6:
      case AIC::RAID_UNIT_7:
      case AIC::RAID_UNIT_8:
      case AIC::ATT_UNIT_2:
      case AIC::ATT_UNIT_PATROL:
      case AIC::ATT_UNIT_BACKUP:
      case AIC::ATT_UNIT_ENGAGE:
      case AIC::ATT_UNIT_SIEGE_DEF:
      case AIC::ATT_UNIT_MAIN_1:
      case AIC::ATT_UNIT_MAIN_2:
      case AIC::ATT_UNIT_MAIN_3:
      case AIC::ATT_UNIT_MAIN_4:
        return validEnumHelper<Unit>(field, value, validValue, "unit");
      case AIC::DEF_DIGGING_UNIT:
      case AIC::ATT_DIGGING_UNIT:
        return validEnumHelper<DiggingUnit>(field, value, validValue, "digging unit");
      case AIC::OUTER_PATROL_GROUPS_MOVE: // this thing has to stay...
      {
        if (!value.is_string())
        {
          LOG(WARNING) << "Field '" << getAICString(field) << "' did not contain a string.";
          return false;
        }

        bool boolValue;
        std::string boolString{ value.get<std::string>() };
        try
        {
          // may produce errors
          std::transform(boolString.begin(), boolString.end(), boolString.begin(), std::tolower);
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
          LOG(WARNING) << "Field '" << getAICString(field) << "' is not a valid boolean string.";
          return false;
        }

        validValue = static_cast<int32_t>(boolValue);
        return true;
      }
      case AIC::HARASSING_SIEGE_ENGINE_1:
      case AIC::HARASSING_SIEGE_ENGINE_2:
      case AIC::HARASSING_SIEGE_ENGINE_3:
      case AIC::HARASSING_SIEGE_ENGINE_4:
      case AIC::HARASSING_SIEGE_ENGINE_5:
      case AIC::HARASSING_SIEGE_ENGINE_6:
      case AIC::HARASSING_SIEGE_ENGINE_7:
      case AIC::HARASSING_SIEGE_ENGINE_8:
        return validEnumHelper<HarassingSiegeEngine>(field, value, validValue, "harassing siege engine");
      case AIC::HARASSING_SIEGE_ENGINES_MAX:
        return validIntRangeHelper(field, value, validValue, 0, 10, "0 to 10");
      case AIC::SIEGE_ENGINE_1:
      case AIC::SIEGE_ENGINE_2:
      case AIC::SIEGE_ENGINE_3:
      case AIC::SIEGE_ENGINE_4:
      case AIC::SIEGE_ENGINE_5:
      case AIC::SIEGE_ENGINE_6:
      case AIC::SIEGE_ENGINE_7:
      case AIC::SIEGE_ENGINE_8:
        return validEnumHelper<SiegeEngine>(field, value, validValue, "siege engine");
      case AIC::ATT_MAIN_GROUPS_COUNT:
        // cheating... I think I also remember that someone wrote there is something broken, and more then 3 create issues..
        return validIntRangeHelper(field, value, validValue, 0, 5, "0 to 3");
      case AIC::TARGET_CHOICE:
        return validEnumHelper<TargetingType>(field, value, validValue, "targeting type");
      default:
      {
        LOG(ERROR) << "AICLoad: Personality value test received non valid field. This should not happen. Is a handler missing?";
        return false;
      }
    }
  }

  void AICLoad::loadAICFile(const std::string name)
  {
    bool runningOk{ true };
    std::string filePath = aicFolder.empty() ? name : aicFolder + "/" + name;

    try
    {
      std::ifstream aicStream(filePath); // reading config from file

      if (!aicStream.good())
      {
        LOG(WARNING) << "AICLoad: Unable to load file '" << name << "'. Is the name or the aic folder wrong?";
        return;
      }

      Json aicJson = Json::parse(aicStream);
 
      auto fileIt = aicJson.find("AICharacters");
      if (fileIt == aicJson.end())
      {
        LOG(WARNING) << "AICLoad: Did not found the field 'AICharacters'. Unable to load aic '" << name << "'.";
        return;
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

        std::string aiNameString{ charIt.value().get<std::string>() };
        AICharacterName aiName{ charIt.value().get<AICharacterName>() };
        if (aiName == AICharacterName::NONE)
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
          AIC aicField{ getEnumFromString<AIC>(valuePair.key()) };
          if (aicField == AIC::NONE)
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

          loadedAICValues[getAICFieldIndex(aiName, aicField)][name] = value;
        }
      }
      LOG(INFO) << "Loaded AIC '" << name << "'.";
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

    // clearing the structure of potential garbage
    if (!runningOk)
    {
      LOG(WARNING) << "Encountered unsolveable problems during load of AIC 'name'. Possible set values are removed.";

      for (auto& indexValueMapPair: loadedAICValues)
      {
        indexValueMapPair.second.erase(name);
      }
    }
  }
}