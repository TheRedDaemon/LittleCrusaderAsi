
#include <algorithm>
#include <limits>
#include <iomanip>

#include "aicLoad.h"

namespace modclasses
{
  AICLoad::AICLoad(const Json &config)
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

      keyIt = confIt.value().find("applyAICFiles");
      if (keyIt != confIt.value().end())
      {
        keysApply = keyIt.value();
      }
      else
      {
        LOG(WARNING) << "AICLoad: 'keyConfig': No 'applyAICFiles' found. The file AICs can not be applied using only this mod and the keyboard. "
          "However, should BltOverlay not exist, 'reloadMain' and 'reloadAll' will also do this.";
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
  std::vector<ModID> AICLoad::getDependencies() const
  {
    return { AddressResolver::ID, KeyboardInterceptor::ID, BltOverlay::ID };
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

    if (!keysApply.empty())
    {
      std::function<void(const HWND, const bool, const bool)> func =
        [this](const HWND hw, const bool keyUp, const bool repeat) { this->applyAICs(hw, keyUp, repeat); };

      if (!keyInterceptor->registerFunction<true>(func, keysApply))
      {
        BltOverlay::sendToConsole("AICLoad: ApplyAICFiles: At least one key combination was not registered.", el::Level::Warning);
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
      // better this way -> without being initialized, no menu would work
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
      // default custom values
      std::copy(vanillaAIC.begin(), vanillaAIC.end(), customAIC.begin());

      if (vanillaAIC.at(0) != 12)
      {
        BltOverlay::sendToConsole("First thread attached event was seemingly triggerd before AIC initialisation. AICLoad features are likely unreliable. "
                                  "Indicator: first int is not a 12.", el::Level::Warning);
      }

      // add menu
      createMenu();

      // load customAICs (file in this case)
      applyAICs(false);

      // if aics are requested at start, load them in
      if (isChanged)
      {
        setCustomAICs(true);
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
      int32_t index{ getAICFieldIndex(aiName, field) };
      this->customAIC[index] = newValue;
      if (isChanged)  // only apply if custom active
      {
        (*aicMemoryPtr)[index] = newValue;
      }
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
    int32_t index{ getAICFieldIndex(aiName, field) };
    this->customAIC[index] = value;
    if (isChanged)  // only apply if custom active
    {
      (*aicMemoryPtr)[index] = value;
    }
    return true;
  }


  // keyboard functions


  void AICLoad::activateAICs(const HWND, const bool keyUp, const bool repeat)
  {
    if (keyUp || repeat){ 
      return;
    }

    setCustomAICs(!isChanged);

    if (activationPtr.thisMenu)
    {
      *(activationPtr.header) = "File AIC active: " + std::string(this->isChanged ? "true" : "false");
      activationPtr.thisMenu->draw(true, false);
    }
  }


  void AICLoad::applyAICs(const HWND, const bool keyUp, const bool repeat)
  {
    if (keyUp || repeat)
    {
      return;
    }

    applyAICs();
  }


  void AICLoad::reloadMainAIC(const HWND, const bool keyUp, const bool repeat)
  {
    // ignores silently if no aics set (good idea? not sure)
    if (keyUp || repeat || loadList.size() == 0)
    {
      return;
    }

    std::string& mainName{
      sortMenuPtr.thisMenu ? std::get<0>(sortMenuPtr.menuCon->front()) : loadList[0]
    };
    size_t mainSize{ loadedAICValues.find(mainName) != loadedAICValues.end() ? loadedAICValues[mainName]->size() : 0 }; // in case it does not exist

    auto changedMain{ loadAICFile(mainName, false, mainSize) };
    if (!changedMain)
    {
      BltOverlay::sendToConsole("Error while reloading main. Loaded AIC data unchanged.", el::Level::Warning);
      return;
    }

    // swaps maps -> old should be deleted after this function
    loadedAICValues[mainName].swap(changedMain);

    // without the menu, these functions also applies the new AIC (and auto loads it) (else not)
    if (!sortMenuPtr.thisMenu)
    {
      applyAICs(false);

      if (isChanged)
      {
        setCustomAICs(true);
      }
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

    // without the menu, these functions also applies the new AIC (and auto loads it) (else not)
    if (!sortMenuPtr.thisMenu)
    {
      applyAICs(false);

      if (isChanged)
      {
        setCustomAICs(true);
      }
    }

    BltOverlay::sendToConsole("Reloaded all AICs.", el::Level::Info);
  }


  // private functions


  void AICLoad::setCustomAICs(bool status)
  {
    if (status == true) // apply them always (for applying changes)
    {
      std::copy(customAIC.begin(), customAIC.end(), std::begin(*aicMemoryPtr));
    }

    // only message if status changed
    if (isChanged != status)
    {
      if (status == false)  // apply vanilla values only in this case
      {
        std::copy(vanillaAIC.begin(), vanillaAIC.end(), std::begin(*aicMemoryPtr));
      }

      isChanged = status;
      BltOverlay::sendToConsole("Changed apply status of AICs to: " + std::string(isChanged ? "true" : "false"), el::Level::Info);
    }
  }


  void AICLoad::applyAICs()
  {
    applyAICs(false);
  }


  void AICLoad::applyAICs(bool reset)
  {
    if (reset)
    {
      std::copy(vanillaAIC.begin(), vanillaAIC.end(), customAIC.begin());
      BltOverlay::sendToConsole("Loaded default AICs into custom AIC values.", el::Level::Info);
      return;
    }

    // lambda to find stuff -> static, does not need extern
    // return pointer to value (or nullptr)
    static auto findValue{ [](const std::string& name, size_t fieldIndex,
      const std::unordered_map<std::string, std::unique_ptr<std::unordered_map<int32_t, int32_t>>>& aicMaps) -> int32_t*
    {
      if (auto nameIt{ aicMaps.find(name) }; nameIt != aicMaps.end())
      {
        auto& fields{ *(nameIt->second) };
        if (auto fieldIt{ fields.find(fieldIndex) }; fieldIt != fields.end())
        {
          return &(fieldIt->second);
        }
      }

      return nullptr;
    }};

    // runs in the following order: over every value index -> for every aic in importance order ( + vanilla)
    for (size_t i = 0; i < vanillaAIC.size(); i++)
    {
      int32_t* valuePtr{ nullptr };
      
      if (sortMenuPtr.thisMenu)
      {
        for (auto& curr : *(sortMenuPtr.menuCon))
        {
          // if not active -> skip
          if (!std::get<1>(curr))
          {
            continue;
          }

          valuePtr = findValue(std::get<0>(curr), i, loadedAICValues);

          if (valuePtr) // end fast
          {
            break;
          }
        }
      }
      else
      {
        // if menu does not work, use load list order
        for (auto& curr : loadList)
        {
          valuePtr = findValue(curr, i, loadedAICValues);

          if (valuePtr) // end fast
          {
            break;
          }
        }
      }

      if (!valuePtr)
      {
        valuePtr = &(vanillaAIC[i]);
      }

      customAIC[i] = *valuePtr;
    }

    BltOverlay::sendToConsole("Loaded file AICs into custom AIC values.", el::Level::Info);
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

    const int32_t* reactNum{ getReactionNumber(field) };
    if (!reactNum)
    {
      // if not found, there was already a log message
      return false;
    }

    switch (*reactNum)
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
  std::unique_ptr<std::unordered_map<int32_t, int32_t>> AICLoad::loadAICFile(const std::string &name,
    const bool fileRelativeToMod, const size_t mapInitSize) const
  {

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
        BltOverlay::sendToConsole("AICLoad: Unable to load file '" + name + "'. Is the name or the aic folder wrong?", el::Level::Warning);
        return {};
      }

      Json aicJson = Json::parse(aicStream);
 
      auto fileIt = aicJson.find("AICharacters");
      if (fileIt == aicJson.end())
      {
        BltOverlay::sendToConsole("AICLoad: Did not found the field 'AICharacters'. Unable to load aic '" + name + "'.", el::Level::Warning);
        return {};
      }

      // can savely create map here
      auto aicMap{ std::make_unique<std::unordered_map<int32_t, int32_t>>() };
      if (mapInitSize > 0)
      {
        aicMap->reserve(mapInitSize);
      }

      bool skippedStuff{ false };

      Json &aicArray = fileIt.value();
      for (const auto& charAIC : aicArray)
      {
        auto charIt = charAIC.find("Name");
        if (charIt == charAIC.end())
        {
          LOG(WARNING) << "AICLoad: Did not found the field 'Name' in one of the character-objects. Skipping the object.";
          skippedStuff = true;
          continue;
        }

        const std::string& aiNameString{ charIt.value().get_ref<const Json::string_t&>() };
        AINameEnum aiName{ AIName::GetEnumByName(charIt.value().get_ref<const Json::string_t&>()) };
        if (!aiName || aiName == AIName::None)
        {
          LOG(WARNING) << "AICLoad: The character name '" << aiNameString << "' is no valid name. Skipping the object.";
          skippedStuff = true;
          continue;
        }

        auto perIt = charAIC.find("Personality");
        if (perIt == charAIC.end())
        {
          LOG(WARNING) << "AICLoad: Did not found the field 'Personality' in object of character '" << aiNameString << "'. Skipping the object.";
          skippedStuff = true;
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
            skippedStuff = true;
            continue;
          }

          int32_t value;
          if (!isValidPersonalityValue(aicField, valuePair.value(), value))
          {
            LOG(WARNING) << "AICLoad: The value of field '" << valuePair.key() << "' of the character '" << aiNameString << "' is not valid. "
              << "Skipping the field.";
            skippedStuff = true;
            continue;
          }

          (*aicMap)[getAICFieldIndex(aiName, aicField)] = value;
        }
      }

      if (skippedStuff)
      {
        BltOverlay::sendToConsole("During loading of '" + name + "' fields were skipped. See file log.", el::Level::Warning);
      }

      BltOverlay::sendToConsole("Loaded AIC: " + name, el::Level::Info);
      return aicMap;
    }
    catch (const Json::parse_error& o_O)
    {
      LOG(ERROR) << "AIC parse error: " << o_O.what();
    }
    catch (const std::exception& o_O)
    {
      LOG(ERROR) << "Error during aic load: " << o_O.what();
    }

    BltOverlay::sendToConsole("Encountered unsolveable problems during load of AIC '" + name + "'. File is ignored.", el::Level::Warning);
    return {};
  }


  std::string AICLoad::saveCustomAIC(const std::string& fileName, const bool fileRelativeToMod)
  {
    if (fileName.empty())
    {
      lastSaveName = "";
      return "No input.";
    }

    bool overwrite{ lastSaveName == fileName ? true : false };

    std::string filePath;
    if (!(fileRelativeToMod || relModPath.empty()))
    {
      filePath += relModPath + "/";
    }
    filePath += aicFolder.empty() ? fileName : aicFolder + "/" + fileName;
    
    try
    {
      if (!overwrite)
      {
        // test if file already exists
        // Source: 
        if (struct stat buf; stat(filePath.c_str(), &buf) != -1)
        {
          lastSaveName = fileName;
          return "Exists. Repeat for overwrite.";
        }
      }

      std::ofstream aicOut(filePath);

      if (!aicOut.good())
      {
        return "Invalid name or path.";
      }

      // keeps insertion order unlike normal json container
      // used to keep value order (likely order people are used to)
      OrderedJson output;

      // adding dummy AIC description
      output["AICShortDescription"] = {
        { "German"     ,  "" },
        { "English"    ,  "" },
        { "Polish"     ,  "" },
        { "Russian"    ,  "" },
        { "Chinese"    ,  "" },
        { "Hungarian"  ,  "" }
      };

      size_t counter{ 0 };
      AIName::WithEachEnumInValueOrder([this, &output, &counter](const std::string& name, AINameEnum nameEnum)
      {
        // filter AICNames that should not be inlcluded
        if (nameEnum == AIName::None || !(this->saveArray[nameEnum->getValue()]))
        {
          return;
        }

        OrderedJson& thisChar{ output["AICharacters"][counter] };
        ++counter;  // prepare for next round already
        
        thisChar["Name"] = name;
        thisChar["CustomName"] = "";  // empty -> not used here for anything
        

        OrderedJson& personality{ thisChar["Personality"] };
        AIC::WithEachEnumInValueOrder([this, &personality, nameEnum](const std::string& aicName, AICEnum aicEnum)
        {
          if (aicEnum == AIC::None)
          {
            return;
          }

          // get sort value
          const int32_t* reactNum{ this->getReactionNumber(aicEnum) };
          if (!reactNum)
          {
            // if not found, there was already a log message
            return;
          }

          OrderedJson& jsonField{ personality[aicName] };
          int32_t value{ this->customAIC[this->getAICFieldIndex(nameEnum, aicEnum)] };

          switch (*reactNum)
          {
            case 0:
            case 1:
            case 2:
            case 4:
            case 5:
            case 10:
            case 15:
            case 17:
              jsonField = value;
              break;
            case 3:
              jsonField = this->getNameOrNULL<Farm>(value); // might produce with UCP incompatible AIC... Change?
              break;
            case 6:
              jsonField = this->getNameOrNULL<Smith>(value);
              break;
            case 7:
              jsonField = this->getNameOrNULL<Fletcher>(value);
              break;
            case 8:
              jsonField = this->getNameOrNULL<Pole>(value);
              break;
            case 9:
              jsonField = this->getNameOrNULL<Resource>(value);
              break;
            case 11:
              jsonField = this->getNameOrNULL<Unit>(value);
              break;
            case 12:
              jsonField = this->getNameOrNULL<DUnit>(value);
              break;
            case 13:  // our little special one -> is this even only true or false? (only test for 0 or diff)
              jsonField = value ? "True" : "False"; // uses bool string(!)
              break;
            case 14:
              jsonField = this->getNameOrNULL<HSE>(value);
              break;
            case 16:
              jsonField = this->getNameOrNULL<SE>(value);
              break;
            case 18:
              jsonField = this->getNameOrNULL<Target>(value);
              break;
            default:
            {
              LOG(ERROR) << "AICLoad: Personality value test found no valid handling. "
                "This should not happen. Is a handler missing?";
              break;
            }
          }
        });
      });

      aicOut << std::setw(4) << output << std::endl;

      lastSaveName = ""; // set last name to empty
      return "Done";
    }
    catch (const std::exception& o_O)
    {
      BltOverlay::sendToConsole(o_O.what(), el::Level::Warning);
      return "Error. See log.";
    }
  }


  // NOTE: no special meaning, just sorted by how I process them somewhere else
  const int32_t* AICLoad::getReactionNumber(AICEnum field) const
  {
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

      // these are all Unit -> is it required to separate them in Ranged and Melee?
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
    if (it != switchMap.end())
    {
      return &(it->second);
    }

    LOG(ERROR) << "AICLoad: Personality value test received non valid field. This should not happen. Is a handler missing?";
    return nullptr;
  }


  // creates templates to deal with different enum menus
  // if I would use objects instead of the static class approach, something might ba possible
  // however, as it stands, I can not send a static class ref
  template<typename T>
  void AICLoad::createEnumMenuHelper(AINameEnum aiName, AICEnum field, MenuBase &charMenu)
  {
    std::vector<std::pair<std::string, int32_t>> valueVector{};
    T::WithEachEnumInValueOrder([&valueVector](const std::string &aicName, typename T::EnumType valueEnum)
    {
      if (valueEnum == T::None)
      {
        return;
      }

      valueVector.push_back(std::make_pair(aicName, valueEnum->getValue()));
    });

    charMenu.createMenu<ChoiceInputMenu, false>(
      std::string(field->getName()),
      nullptr,
      getNameOrNULL<T>(vanillaAIC[getAICFieldIndex(aiName, field)]),
      std::move(valueVector),
      [this, aiName, field](int32_t newValue, std::string& resultMessage,
        bool onlyUpdateCurrent, std::string& currentValue)
      {
        if (onlyUpdateCurrent)
        {
          currentValue = this->getNameOrNULL<T>(this->customAIC[this->getAICFieldIndex(aiName, field)]);
          return false;
        }

        // no need for extra check -> should be ok by definition
        if (this->setPersonalityValueUnchecked(aiName, field, newValue))
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


  void AICLoad::createMenu()
  {
    auto bltOverlay{ getMod<BltOverlay>() };
    if (!bltOverlay)
    {
      BltOverlay::sendToConsole("AICLoad: Unable to get BltOverlay for menu create.", el::Level::Warning);
    }

    MenuBase* basPtr{ bltOverlay->getMainMenu() };
    if (!basPtr)
    {
      BltOverlay::sendToConsole("AICLoad: Unable to get main menu.", el::Level::Warning);
    }

    MenuBase& aicMenu{ 
      (*basPtr).createMenu<MainMenu, true>(
        "AIC Load",
        nullptr,
        true
      )
    };

    aicMenu.createMenu<MainMenu, false>(
      "File AIC Active: " + std::string(this->isChanged ? "True" : "False"),
      [this](bool, std::string& header)
      {
        this->setCustomAICs(!(this->isChanged));
        header = "File AIC Active: " + std::string(this->isChanged ? "True" : "False");
        return false;
      },
      false,
      &activationPtr
    );

    aicMenu.createMenu<MainMenu, false>(
      "Apply File AICs",
      [this](bool, std::string&)
      {
        this->applyAICs();
        if (this->isChanged)
        {
          this->setCustomAICs(true);  // reload if active
        }
        return false;
      },
      false
    );


    // init reload vector
    std::vector<std::pair<std::string, int32_t>> initialAICList{};
    for (size_t i = 0; i < loadList.size(); i++)
    {
      initialAICList.emplace_back(loadList[i], i);  // save index
    }

    // init load order sort container
    SortableListMenu::SortMenuContainer sortInitCon{};
    for (auto& aic : loadList)
    {
      sortInitCon.emplace_back(aic, true, 0); // value not important
    }

    // load menu
    aicMenu.createMenu<MainMenu, true>(
      "AIC File Load Menu",
      nullptr,
      true
    )
      // load new AIC menu
      .createMenu<FreeInputMenu, false>(
        "Load New AIC",
        nullptr,
        false,
        [this](const std::string& fileName, std::string& resultMessage)
        {
          if (this->loadedAICValues.find(fileName) != this->loadedAICValues.end())
          {
            resultMessage = "Exists. Use reload.";
            return;
          }

          auto newFile{ this->loadAICFile(fileName, false) };
          if (!newFile)
          {
            resultMessage = "Failed. See log.";
            return;
          }

          this->loadedAICValues[fileName].swap(newFile);  // add new
          if (this->sortMenuPtr.thisMenu) // if exists -> I do not know how needed this is, since it can not fail, only break
          {
            this->sortMenuPtr.menuCon->emplace_back(fileName, true, 0); // default active
          }
          this->loadList.push_back(fileName);
          if (this->reloadMenuPtr.thisMenu) // if exists -> I do not know how needed this is, since it can not fail, only break
          {
            this->reloadMenuPtr.choicePairs->emplace_back(this->loadList.back(), this->loadList.size() - 1);
          }

          resultMessage = "Loaded.";
        }
      )
      // reload menu
      .createMenu<ChoiceInputMenu, false>(
        "Reload AIC",
        nullptr,
        std::move(initialAICList),
        [this](int32_t value, std::string& resultMessage)
        {
          std::string& aicName{ loadList[value] };
          auto it{ loadedAICValues.find(aicName) };
          if (it == loadedAICValues.end())
          {
            resultMessage = "Not loaded.";  // should not really happen
            return;
          }

          auto changedMain{ loadAICFile(aicName, false, loadedAICValues[aicName]->size()) };
          if (!changedMain)
          {
            resultMessage = "Failed. See log.";
            return;
          }

          // swaps maps -> old should be deleted after this function
          loadedAICValues[aicName].swap(changedMain);
          resultMessage = "Loaded.";
        },
        &reloadMenuPtr
      )
      // create sort menu for AIC load
      .createMenu<SortableListMenu, false>(
        "File Apply Order",
        nullptr,
        std::move(sortInitCon),
        nullptr,
        nullptr,
        &sortMenuPtr
      )  
      .createMenu<MainMenu, false>(
        "Reload Main File",
        [this](bool, std::string&)
        {
          this->reloadMainAIC(0, false, false);
          return false;
        },
        true
      )
      .createMenu<MainMenu, false>(
        "Reload All Files",
        [this](bool, std::string&)
        {
          this->reloadAllAIC(0, false, false);
          return false;
        },
        true
      );

    aicMenu.createMenu<MainMenu, false>(
      "Custom Values To Default",
      [this](bool, std::string&)
      {
        this->applyAICs(true);
        if (this->isChanged)
        {
          this->setCustomAICs(true);  // reload if active
        }
        return false;
      },
      false
    );


    // create basic menus and get ref to editor menu
    MenuBase& editorMenu{
      aicMenu.createMenu<MainMenu, true>(
        "Editor",
        nullptr,
        true
      )
    };

    // create individual editor menus (AIName * AIC)
    AIName::WithEachEnumInValueOrder([this, &editorMenu](const std::string &name, AINameEnum nameEnum)
    {
      if (nameEnum == AIName::None)
      {
        return;
      }

      MenuBase& charMenu{
        editorMenu.createMenu<MainMenu, true>(
          std::string(name),
          nullptr,
          false
        )
      };

      AIC::WithEachEnumInValueOrder([this, &charMenu, nameEnum](const std::string &aicName, AICEnum aicEnum)
      {
        if (aicEnum == AIC::None)
        {
          return;
        }

        // get sort value
        const int32_t* reactNum{ this->getReactionNumber(aicEnum) };
        if (!reactNum)
        {
          // if not found, there was already a log message
          return;
        }

        switch (*reactNum)
        {
          case 0:
          case 1:
          case 2:
          case 4:
          case 5:
          case 10:
          case 15:
          case 17:
          {
            charMenu.createMenu<FreeInputMenu, false>(
              std::string(aicName),
              nullptr,
              true,
              std::to_string(this->vanillaAIC[this->getAICFieldIndex(nameEnum, aicEnum)]),
              [this, nameEnum, aicEnum](const std::string& currentInput,
                std::string& resultMessage, bool onlyUpdateCurrent, std::string& currentValue)
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
                    if (this->setPersonalityValue(nameEnum, aicEnum, inputValue))
                    {
                      resultMessage = "Success.";
                    }
                    else
                    {
                      resultMessage = "Failed. See log.";
                    }
                  }
                }

                currentValue = std::to_string(
                  this->customAIC[this->getAICFieldIndex(nameEnum, aicEnum)]);
              }
            );

            break;
          }
          case 3:
            createEnumMenuHelper<Farm>(nameEnum, aicEnum, charMenu);
            break;
          case 6:
            createEnumMenuHelper<Smith>(nameEnum, aicEnum, charMenu);
            break;
          case 7:
            createEnumMenuHelper<Fletcher>(nameEnum, aicEnum, charMenu);
            break;
          case 8:
            createEnumMenuHelper<Pole>(nameEnum, aicEnum, charMenu);
            break;
          case 9:
            createEnumMenuHelper<Resource>(nameEnum, aicEnum, charMenu);
            break;
          case 11:
            createEnumMenuHelper<Unit>(nameEnum, aicEnum, charMenu);
            break;
          case 12:
            createEnumMenuHelper<DUnit>(nameEnum, aicEnum, charMenu);
            break;
          case 13:  // our little special one -> is this even only true or false? (only test for 0 or diff)
          {
            charMenu.createMenu<ChoiceInputMenu, false>(
              std::string(aicName),
              nullptr,
              vanillaAIC[getAICFieldIndex(nameEnum, aicEnum)] ? "true" : "false",
              std::vector<std::pair<std::string, int32_t>>{
                { "false", 0 },
                { "true", 1 }
              },
              [this, nameEnum, aicEnum](int32_t newValue, std::string& resultMessage,
                bool onlyUpdateCurrent, std::string& currentValue)
              {
                if (onlyUpdateCurrent)
                {
                  currentValue = this->customAIC[this->getAICFieldIndex(nameEnum, aicEnum)] ? "true" : "false";
                  return false;
                }

                // no need for extra check -> should be ok by definition
                if (this->setPersonalityValueUnchecked(nameEnum, aicEnum, newValue))
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

            break;
          }
          case 14:
            createEnumMenuHelper<HSE>(nameEnum, aicEnum, charMenu);
            break;
          case 16:
            createEnumMenuHelper<SE>(nameEnum, aicEnum, charMenu);
            break;
          case 18:
            createEnumMenuHelper<Target>(nameEnum, aicEnum, charMenu);
            break;
          default:
          {
            LOG(ERROR) << "AICLoad: Personality value test found no valid handling. "
              "This should not happen. Is a handler missing?";
            break;
          }
        }
      });
    });

    // create save menus (could maybe later be moved in the create AIC editor loop)
    saveArray.fill(true); // initial fill

    MenuBase& saveMenu{
      aicMenu.createMenu<MainMenu, true>(
        "Save Custom AIC",
        nullptr,
        true
      )
    };

    MenuBase& customizeSaveMenu{
      saveMenu.createMenu<MainMenu, true>(
        "Customize",
        nullptr,
        true
      )
    };

    AIName::WithEachEnumInValueOrder([this, &customizeSaveMenu](const std::string& name, AINameEnum nameEnum)
    {
      if (nameEnum == AIName::None)
      {
        return;
      }

      customizeSaveMenu.createMenu<MainMenu, false>(
        "Save " + name + ": " + std::string(this->saveArray.at(nameEnum->getValue()) ? "Yes" : "No"),
        [this, nameEnum](bool, std::string& header)
        {
          int32_t aiValue{ nameEnum->getValue() };
          this->saveArray.at(aiValue) = !(this->saveArray.at(aiValue));
          header = "Save " + nameEnum->getName() + ": " + 
            std::string(this->saveArray.at(aiValue) ? "Yes" : "No");
          return false;
        },
        true
      );
    });

    saveMenu.createMenu<FreeInputMenu, false>(
      "Save",
      nullptr,
      false,
      [this](const std::string& fileName, std::string& resultMessage)
      {
        resultMessage = std::move(this->saveCustomAIC(fileName, false));
      }
    );
  }


  std::vector<AddressRequest> AICLoad::usedAddresses{
    {Address::AIC_IN_MEMORY, {{Version::NONE, 10816}}, true, AddressRisk::WARNING}
  };
}