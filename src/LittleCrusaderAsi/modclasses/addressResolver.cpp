
#include "addressResolver.h"

namespace modclasses
{

  AddressResolver::AddressResolver(const std::weak_ptr<modcore::ModKeeper> modKeeper, const Json &config) : ModBase(modKeeper)
  {
    auto confIt = config.find("rejectLevel");
    if (confIt != config.end())
    {
      AddressRisk configConflictLevel{ confIt.value().get<AddressRisk>() };

      if (configConflictLevel != AddressRisk::NONE)
      {
        conflictLevel = configConflictLevel;
      }
    }
  }

  std::vector<ModType> AddressResolver::getDependencies() const
  {
    return { ModType::ADDRESS_BASE, ModType::VERSION_GET };
  }

  bool AddressResolver::initialize()
  {
    auto addressBaseMod = getMod<AddressBase>();
    auto versionGetterMod = getMod<VersionGetter>();

    if (addressBaseMod && versionGetterMod)
    {
      addressBase = addressBaseMod->getBaseAddress();
      version = versionGetterMod->getCurrentStrongholdVersion();

      initialized = true;
      if (!requestAddresses(versionGetterMod->returnUsedAddresses(), *versionGetterMod))
      {
        initialized = false;
      }
      else
      {
        LOG(INFO) << "AddressResolver initialized.";
      }
    }

    if (!initialized)
    {
      LOG(WARNING) << "AddressResolver was not initialized.";
    }

    return initialized;
  }

  /**********************************************************************************/
  // addressRequestHandling

  // TODO?: maybe add a debug log here? but it wouldn't know the mods...
  const bool AddressResolver::checkIfAddressRiskViolated(AddressRisk newRisk, AddressRisk oldRisk)
  {
    bool violated{ false };

    switch (conflictLevel)
    {
      case AddressRisk::SAFE:
        violated = true;  // all overlaps are rejected
        break;

      case AddressRisk::WARNING:
        violated = (oldRisk != AddressRisk::SAFE && newRisk != AddressRisk::SAFE);
        break;

      case AddressRisk::FUNCTION_WARNING:
        violated = ((oldRisk == AddressRisk::FUNCTION_WARNING || oldRisk == AddressRisk::CRITICAL) &&
          (newRisk == AddressRisk::FUNCTION_WARNING || newRisk == AddressRisk::CRITICAL));
        break;

      case AddressRisk::CRITICAL:
        violated = (oldRisk == AddressRisk::CRITICAL && newRisk == AddressRisk::CRITICAL);
        break;

      default:
        throw std::exception("The chosen ConflictLevel was NONE or not handled. This should never happen.");
    }

    return violated;
  }

  const std::pair<DWORD, DWORD> AddressResolver::getStartAndEndAddress(const AddressRequest& req)
  {
    DWORD startAddress{ 0x0 };
    DWORD endAddress{ 0x0 };

    startAddress = getAddress(req.address);

    size_t length;
    if (const auto& specIt = req.length.find(version); specIt != req.length.end())
    {
      length = specIt->second;
    }
    else if (const auto& defIt = req.length.find(Version::NONE); defIt != req.length.end())
    {
      length = defIt->second;
    }
    else
    {
      throw std::exception(("No length value specified for request of address '" + std::to_string(static_cast<int>(req.address)) + "'.").data());
    } 

    // since length 1 is start address
    if (length - 1 > 0)
    {
      endAddress = startAddress + length - 1;
    }
    else
    {
      endAddress = startAddress;
    }

    return std::pair<DWORD, DWORD>(startAddress, endAddress);
  }

  // fills addressesWhereToAddRequest, addToNewAddressStart and addToNewAddressEnd
  // also return a bool that says "true" if the risk level was violated
  // TODO?: the unordered_maps are still redundantly filled, any worthwhile check possible?
  // NOTE: length 1 addresses and start/end overlaps will create an overhead that needs to be filtered in the return
  const bool AddressResolver::checkRiskAndOverlaps(
    const ModType requestModType, const AddressRequest &newToAddReq, const std::pair<DWORD, DWORD> newAddrStartEnd,
    const std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &reqInPlace, std::unordered_set<DWORD> &addressesWhereToAddRequest,
    std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &addToNewAddressStart, std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &addToNewAddressEnd)
  {
    bool riskToHigh{ false };
    auto otherReqSetIter = reqInPlace.begin();
    while (!riskToHigh && otherReqSetIter != reqInPlace.end())
    {

      auto otherReq = (otherReqSetIter->second).begin();
      while (!riskToHigh && otherReq != (otherReqSetIter->second).end())
      {

        const std::pair<DWORD, DWORD> otherRange{ getStartAndEndAddress(**otherReq) };
        bool overlap{ !(newAddrStartEnd.first > otherRange.second || newAddrStartEnd.second < otherRange.first) };

        if (overlap && &newToAddReq != (*otherReq)) // only test for conflict if different requests (just shows that there are some issues)
        {
          riskToHigh = checkIfAddressRiskViolated(newToAddReq.addressRisk, (*otherReq)->addressRisk);

          if (!riskToHigh)
          {
            bool startInside{ newAddrStartEnd.first > otherRange.first }; // if both firsts are the same, treat them as outside -> add to already there
            bool endInside{ newAddrStartEnd.second < otherRange.second }; // if both ends are the same, treat them as outside -> add to already there

            // NOTE: 1 length addresses and start/end overlaps will create an overhead, that needs to be filtered in the return

            if (startInside && endInside) // new start inside + new end inside -> add request pointer old to new start and new end
            {
              addToNewAddressStart[otherReqSetIter->first].insert(*otherReq);
              addToNewAddressEnd[otherReqSetIter->first].insert(*otherReq);
            }
            else if (startInside && !endInside) // new start inside + new end outside -> add request pointer old to new start and new request pointer to old end
            {
              addToNewAddressStart[otherReqSetIter->first].insert(*otherReq);
              addressesWhereToAddRequest.insert(otherRange.second);
            }
            else if (!startInside && endInside) // new start outside + new end inside -> add request pointer old to new end and new request pointer to old start
            {
              addressesWhereToAddRequest.insert(otherRange.first);
              addToNewAddressEnd[otherReqSetIter->first].insert(*otherReq);
            }
            else // (!startInside && !endInside) // new start outside + new end outside -> add new request pointer to old start and old end
            {
              addressesWhereToAddRequest.insert(otherRange.first);
              addressesWhereToAddRequest.insert(otherRange.second);
            }
          }
        }

        otherReq = std::next(otherReq);
      }

      otherReqSetIter = std::next(otherReqSetIter);
    }

    if (riskToHigh)
    {
      LOG(WARNING) << "Risk level violated trying to request address with id '" << std::to_string(static_cast<int>(newToAddReq.address))
        << "' for mod with id '" << std::to_string(static_cast<int>(requestModType)) << "'.";
    }

    return riskToHigh;
  }

  const bool AddressResolver::requestAddresses(std::vector<AddressRequest> &addrReq, const ModBase &requestingMod)
  {
    if (initialized)
    {
      // save ref to unordered address map, to remove in error case
      std::unordered_map<DWORD, std::unordered_map<ModType, std::unordered_set<AddressRequest*>>*> addedAddresses; // used to identify maps
      std::unordered_set<AddressRequest*> addedRequests; // used to only delete newly requested addresses

      size_t i{ 0 };
      bool conflict{ false };
      while (!conflict && i < addrReq.size())
      {
        AddressRequest& req = addrReq[i];
        std::pair<DWORD, DWORD> addrStartEnd;

        std::unordered_set<DWORD> addressesWhereToAddRequest;
        std::unordered_map<ModType, std::unordered_set<AddressRequest*>> addToAddressStart;
        std::unordered_map<ModType, std::unordered_set<AddressRequest*>> addToAddressEnd;

        try
        {
          addrStartEnd = getStartAndEndAddress(req);
        }
        catch (const std::exception& o_O)
        {
          LOG(WARNING) << o_O.what();
          conflict = true;
        }

        if (!conflict)
        {
          // create if not exist // start
          if (const auto& addrIt = addressSortContainer.find(addrStartEnd.first); addrIt == addressSortContainer.end())
          {
            addressSortContainer[addrStartEnd.first][requestingMod.getModType()].insert(&req);
            addedAddresses.try_emplace(addrStartEnd.first, &addressSortContainer.at(addrStartEnd.first)); // add them here already, so there is a hint for removal in conflict case
          }
          // end
          if (const auto& addrIt = addressSortContainer.find(addrStartEnd.second); addrIt == addressSortContainer.end())
          {
            addressSortContainer[addrStartEnd.second][requestingMod.getModType()].insert(&req);;
            addedAddresses.try_emplace(addrStartEnd.second, &addressSortContainer.at(addrStartEnd.second)); // add them here already, so there is a hint for removal in conflict case
          }

          auto currentIt = addressSortContainer.find(addrStartEnd.first);
          const auto& endIt = std::next(addressSortContainer.find(addrStartEnd.second));
          do
          {
            conflict = checkRiskAndOverlaps(requestingMod.getModType(), req, addrStartEnd, currentIt->second,
                                            addressesWhereToAddRequest, addToAddressStart, addToAddressEnd);

            currentIt = std::next(currentIt);
          }
          while (currentIt != endIt && !conflict);

          // check if capsuled by other
          if (!conflict && currentIt != addressSortContainer.end())
          {
            conflict = checkRiskAndOverlaps(requestingMod.getModType(), req, addrStartEnd, currentIt->second,
                                            addressesWhereToAddRequest, addToAddressStart, addToAddressEnd);
          }

          if (!conflict)
          {
            // now adding the found values
            for (const DWORD addrWhere : addressesWhereToAddRequest)
            {
              auto& modMap = addressSortContainer.at(addrWhere);
              modMap[requestingMod.getModType()].insert(&req);
              addedAddresses.try_emplace(addrWhere, &modMap);
            }

            auto& modMapStart = addressSortContainer.at(addrStartEnd.first);
            for (const auto& slotStart : addToAddressStart)
            {
              modMapStart[slotStart.first].insert(slotStart.second.begin(), slotStart.second.end());
            }

            auto& modMapEnd = addressSortContainer.at(addrStartEnd.second);
            for (const auto& slotEnd : addToAddressEnd)
            {
              modMapEnd[slotEnd.first].insert(slotEnd.second.begin(), slotEnd.second.end());
            }

            // add start and end position, this position should add them regardless if they are newly added or taken from a old position
            // just shows that I don't really trust the stuff here -> needs testing, a loooooot
            addedAddresses.try_emplace(addrStartEnd.first, &addressSortContainer.at(addrStartEnd.first));
            addedAddresses.try_emplace(addrStartEnd.second, &addressSortContainer.at(addrStartEnd.second));

            addedRequests.insert(&req); // only one address request is really added per while run
          }
        }
        ++i;
      }

      // only add write permission after all addresses are succesfully added
      // even on conflicts, write permissions aren't removed, because one would have to know the page structures to not block others
      DWORD oldAddressProtection{ 0 };
      i = 0;  // reset counter
      while (!conflict && i < addrReq.size())
      {
        AddressRequest& req = addrReq[i];

        if (req.allowWrite)
        {
          // since the addresses where successfully added, this shouldn't cause an error
          std::pair<DWORD, DWORD> addrStartEnd = getStartAndEndAddress(req);

          // i assume switch to execute_readwrite (or to copy on write?)
          if (!VirtualProtect(reinterpret_cast<DWORD*>(addressBase + addrStartEnd.first),
                              // plus 1, because uses other definition? needs at least length 1 (start end same would be error)
                              addrStartEnd.second - addrStartEnd.first + 1,
                              PAGE_EXECUTE_READWRITE, &oldAddressProtection))
          {
            LOG(WARNING) << "Couldn't allow write access of memory. Error code: " << GetLastError();
            conflict = true;
          }
        }
        ++i;
      }

      // run over all addresses that where added in the request
      if (conflict)
      {
        for (auto mapOfMods : addedAddresses)
        {
          bool allIn{ true };
          std::vector<AddressRequest*> toRemove;
          for (const auto& addr : mapOfMods.second->at(requestingMod.getModType()))
          {
            bool in{ addedRequests.find(addr) != addedRequests.end() };

            if (in)
            {
              toRemove.push_back(addr);
            }

            allIn = allIn && in;
          }

          if (allIn)
          {
            mapOfMods.second->erase(requestingMod.getModType());

            if (mapOfMods.second->empty())
            {
              // removes address entry if no entry left
              // important, pointers to this part will be invalid after this
              addressSortContainer.erase(mapOfMods.first);
            }
          }
          else
          {
            for (const auto rem : toRemove)
            {
              mapOfMods.second->at(requestingMod.getModType()).erase(rem);
            }
          }
        }

        LOG(WARNING) << "Failed granting access of addresses to mod with id '" << std::to_string(static_cast<int>(requestingMod.getModType())) << "'.";
        return false;
      }

      return true;
    }

    throw std::exception("AddressResolver wasn't successfully initialized, but \"requestAddresses\" was still called.");
  }
}