
#include "buildRangeChanger.h"

namespace modclasses
{
  BuildRangeChanger::BuildRangeChanger(const Json &config)
  {
    auto confIt = config.find("startState");
    if (confIt != config.end() && confIt.value().is_boolean())
    {
      isChanged = confIt.value().get<bool>();
    }

    // initial file ranges -> are first set as custom values
    confIt = config.find("fileRanges");
    if (confIt != config.end())
    {
      for (const auto& range : confIt.value().items())
      {
        BuildRange toChange{ getEnumFromString<BuildRange>(range.key()) };

        if (toChange != BuildRange::NONE)
        {
          BuildRangeData& currRange{ buildRanges[static_cast<int32_t>(toChange)] };
          currRange.fileValue = range.value().get<int32_t>();
          currRange.fileValueProvided = true;
        }
      }
    }

    confIt = config.find("keyShortcut");
    if (confIt != config.end())
    {
      keyboardShortcut = confIt.value();
    }
  }


  std::vector<ModID> BuildRangeChanger::getDependencies() const
  {
    return { KeyboardInterceptor::ID, AddressResolver::ID, BltOverlay::ID };
  }


  void BuildRangeChanger::initialize()
  {
    auto addressResolver = getMod<AddressResolver>();
    auto keyInterceptor = getMod<KeyboardInterceptor>();

    if (addressResolver && keyInterceptor)
    {
      // request addresses
      if (addressResolver->requestAddresses(usedAddresses, *this))
      {
        // get addresses and default values
        for (size_t i = 0; i < buildRanges.size(); i++)
        {
          BuildRangeData& currRange{ buildRanges[i] };
          // should be safe
          BuildRange currentRangeEnum{ static_cast<BuildRange>(i) };

          Address currentAddress{ Address::NONE };
          switch (currentRangeEnum)
          {
            case BuildRange::RANGE_160:
              currentAddress = Address::BUILDRANGE_CRUSADE_CASTLE_160;
              break;

            case BuildRange::RANGE_200:
              currentAddress = Address::BUILDRANGE_CRUSADE_CASTLE_200;
              break;

            case BuildRange::RANGE_300:
              currentAddress = Address::BUILDRANGE_CRUSADE_CASTLE_300;
              break;

            case BuildRange::RANGE_400:
              currentAddress = Address::BUILDRANGE_CRUSADE_CASTLE_400;
              break;
          }

          if (currentAddress != Address::NONE)
          {
            currRange.address = addressResolver->getAddressPointer<int32_t>(currentAddress, *this);
            currRange.defaultValue = *(currRange.address);
          }
          else
          {
          BltOverlay::sendToConsole("BuildRangeChanger: Failed to get proper range address. This should not happen.",
            el::Level::Error);
          }
        }

        if (!keyboardShortcut.empty())
        {
          // source: https://stackoverflow.com/questions/7582546/using-generic-stdfunction-objects-with-member-functions-in-one-class

          // method using std::bind:
          //std::function<void(const HWND, const bool, const bool)> func =
          //  std::bind(&BuildRangeChanger::switchRangeChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

          // method using lambda:
          std::function<void(const HWND, const bool, const bool)> func =
            [this](const HWND hw, const bool keyUp, const bool repeat) { this->switchRangeChange(hw, keyUp, repeat); };

          if (!keyInterceptor->registerFunction<true>(func, keyboardShortcut))
          {
            LOG(WARNING) << "BuildRangeChanger: At least one key combination was not registered.";
          }
        }

        resetValues(true);
        if (isChanged)
        {
          applyValues(true);
        }

        // now create the menu
        createMenu();

        // at this point it should work
        initialized = true;
      }
    }

    initialized ? BltOverlay::sendToConsole("BuildRangeChanger initialized.", el::Level::Info)
      : BltOverlay::sendToConsole("BuildRangeChanger was not initialized.", el::Level::Warning);
  }


  // public utility //


  void BuildRangeChanger::setBuildRange(BuildRange range, int32_t value)
  {
    BuildRangeData& currRange{ buildRanges[static_cast<int32_t>(range)] };
    currRange.currentCustomValue = value;

    if (isChanged)
    {
      *(currRange.address) = value;
    }
  }


  void BuildRangeChanger::setRangeChangeStatus(bool active)
  {
    if (isChanged != active)
    {
      applyValues(active);
    }
  }


  // keyboard //


  void BuildRangeChanger::switchRangeChange(const HWND, const bool keyUp, const bool repeat)
  {
    if (!(keyUp || repeat))
    {
      applyValues(!isChanged);
    }
  }


  // intern //


  void BuildRangeChanger::resetValues(bool toFileValues)
  {
    for (auto& range : buildRanges)
    {
      range.currentCustomValue = toFileValues && range.fileValueProvided ? range.fileValue : range.defaultValue;
    }
  }


  void BuildRangeChanger::applyValues(bool custom)
  {
    for (const auto& range : buildRanges)
    {
      *(range.address) = custom ? range.currentCustomValue : range.defaultValue;
    }

    if (isChanged != custom){
      isChanged = custom;
      BltOverlay::sendToConsole("BuildRangeChanger status: " + std::string(isChanged ? "active" : "deactivated"), el::Level::Info);
    }
  };


  void BuildRangeChanger::createMenu()
  {
    auto bltOverlay{ getMod<BltOverlay>() };
    if (!bltOverlay)
    {
      BltOverlay::sendToConsole("BuildRangeChanger: Unable to get BltOverlay for menu create.", el::Level::Warning);
    }

    MenuBase* basPtr{ bltOverlay->getMainMenu() };
    if (!basPtr)
    {
      BltOverlay::sendToConsole("BuildRangeChanger: Unable to get main menu.", el::Level::Warning);
    }

    MenuBase& editorMenu{
      (*basPtr).createMenu<MainMenu, true>(
        "Build Range Changer",
        nullptr,
        true
      )
        // NOTE: currently, the 
        .createMenu<MainMenu, false>(
          "Custom Values: " + std::string(isChanged ? "True" : "False"),
          [this](bool, std::string& header)
          {
            this->applyValues(!(this->isChanged));
            header = "Custom Values: " + std::string(this->isChanged ? "True" : "False");
            return false;
          },
          true
        )
        .createMenu<MainMenu, false>(
          "Reset Custom To Default",
          [this](bool, std::string&)
          {
            this->resetValues(false);
            if (this->isChanged)
            {
              this->applyValues(true);
            }
            return false;
          },
          true
        )
        .createMenu<MainMenu, false>(
          "Reset Custom To Config",
          [this](bool, std::string&)
          {
            this->resetValues(true);
            if (this->isChanged)
            {
              this->applyValues(true);
            }
            return false;
          },
          true
        )
        .createMenu<MainMenu, true>(
          "Custom Value Editor",
          nullptr,
          true
        )
    };

    for (size_t i = 0; i < buildRanges.size(); i++)
    {
      BuildRange range{ static_cast<BuildRange>(i) };
      
      editorMenu.createMenu<FreeInputMenu, false>(
        "Map Size: " + getStringFromEnum(range),
        nullptr,
        true,
        std::to_string(buildRanges[i].defaultValue),
        [this, range](const std::string& currentInput,
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
              this->setBuildRange(range, inputValue);
              resultMessage = "Success.";
            }
          }

          // shows custom value:
          currentValue = std::to_string(this->buildRanges[static_cast<int32_t>(range)].currentCustomValue);
        }
      );
    }
  }


  std::vector<AddressRequest> BuildRangeChanger::usedAddresses{
    {Address::BUILDRANGE_CRUSADE_CASTLE_160, {{Version::NONE, 4}}, true, AddressRisk::WARNING},
    {Address::BUILDRANGE_CRUSADE_CASTLE_200, {{Version::NONE, 4}}, true, AddressRisk::WARNING},
    {Address::BUILDRANGE_CRUSADE_CASTLE_300, {{Version::NONE, 4}}, true, AddressRisk::WARNING},
    {Address::BUILDRANGE_CRUSADE_CASTLE_400, {{Version::NONE, 4}}, true, AddressRisk::WARNING}
  };
}