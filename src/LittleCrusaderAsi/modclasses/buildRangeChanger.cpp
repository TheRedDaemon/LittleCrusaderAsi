
#include "buildRangeChanger.h"

namespace modclasses
{
  BuildRangeChanger::BuildRangeChanger(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
    auto confIt = config.find("startState");
    if (confIt != config.end())
    {
      isChanged = confIt.value().get<bool>();
    }

    confIt = config.find("newRanges");
    if (confIt != config.end())
    {
      for (const auto& range : confIt.value().items())
      {
        // don't know if easier way to parse from string back to enum exist
        Json key;
        key["key"] = range.key();
        BuildRange toChange{ key["key"].get<BuildRange>() };

        if (toChange != BuildRange::NONE)
        {
          int32_t newRange = range.value().get<int32_t>();
          defaultAndNewValues.try_emplace(toChange, std::pair<int32_t, int32_t>(0, newRange));
        }
      }
    }

    confIt = config.find("keyShortcut");
    if (confIt != config.end())
    {
      keyboardShortcut = confIt.value();
    }
  }

  std::vector<ModType> BuildRangeChanger::getDependencies() const
  {
    return { ModType::KEYBOARD_INTERCEPTOR, ModType::ADDRESS_RESOLVER };
  }

  bool BuildRangeChanger::initialize()
  {
    auto addressResolver = getMod<AddressResolver>();
    auto keyInterceptor = getMod<KeyboardInterceptor>();

    if (addressResolver && keyInterceptor)
    {
      // request addresses
      if (addressResolver->requestAddresses(usedAddresses, *this))
      {
        // get addresses and default values
        for (auto& range : defaultAndNewValues)
        {
          Address currentAddress{ Address::NONE };
          switch (range.first)
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
            castleRanges.try_emplace(range.first, addressResolver->getAddressPointer<int32_t>(currentAddress, *this));
            range.second.first = *castleRanges[range.first];
          }
        }

        if (!keyboardShortcut.empty())
        {
          // source: https://stackoverflow.com/questions/7582546/using-generic-stdfunction-objects-with-member-functions-in-one-class
          // one posts said, lambda is prefered, but I don't want to think about how to define currently
          std::function<void(const HWND, const bool, const bool)> func =
            std::bind(&BuildRangeChanger::switchRangeChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

          auto registerResult = keyInterceptor->registerFunction(func, keyboardShortcut);
          bool allRegistered{ true };
          for (bool ok : registerResult)
          {
            allRegistered = allRegistered && ok;
          }

          if (!allRegistered)
          {
            LOG(WARNING) << "BuildRangeChanger: At least one key combination was not registered.";
          }
        }

        if (isChanged)
        {
          isChanged = false;
          switchRangeChange(0, false, false);
        }

        // at this point, it should work
        initialized = true;
      }
    }

    if (!initialized)
    {
      LOG(WARNING) << "BuildRangeChanger was not initialized.";
    }
    else
    {
      LOG(INFO) << "BuildRangeChanger initialized.";
    }

    return initialized;
  }

  void BuildRangeChanger::switchRangeChange(const HWND, const bool keyUp, const bool repeat)
  {
    if (!(keyUp || repeat))
    {
      if (isChanged)
      {
        // back to default
        for (const auto& range : castleRanges)
        {
          *(range.second) = defaultAndNewValues[range.first].first;
        }
        isChanged = false;
      }
      else
      {
        // switch to new
        for (const auto& range : castleRanges)
        {
          *(range.second) = defaultAndNewValues[range.first].second;
        }
        isChanged = true;
      }
    }
  }

  std::vector<AddressRequest> BuildRangeChanger::usedAddresses{
    {Address::BUILDRANGE_CRUSADE_CASTLE_160, {{Version::NONE, 4}}, true, AddressRisk::WARNING},
    {Address::BUILDRANGE_CRUSADE_CASTLE_200, {{Version::NONE, 4}}, true, AddressRisk::WARNING},
    {Address::BUILDRANGE_CRUSADE_CASTLE_300, {{Version::NONE, 4}}, true, AddressRisk::WARNING},
    {Address::BUILDRANGE_CRUSADE_CASTLE_400, {{Version::NONE, 4}}, true, AddressRisk::WARNING}
  };
}