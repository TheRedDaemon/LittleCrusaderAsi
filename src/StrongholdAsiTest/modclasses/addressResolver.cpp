
#include "addressResolver.h"

namespace modclasses
{

  AddressResolver::AddressResolver(Json config)
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

  void AddressResolver::giveDependencies(const std::vector<std::shared_ptr<ModBase>> dep)
  {
    if (dep.size() == 2)
    {
      for (const auto &mod : dep)
      {
        switch (mod->getModType())
        {
          case ModType::ADDRESS_BASE:
            addrBase = std::static_pointer_cast<AddressBase>(mod);
            break;
          case ModType::VERSION_GET:
            addrBase = std::static_pointer_cast<VersionGetter>(mod);
            break;
          default:
            break;
        }
      }
    }

    if (addrBase.expired() || verGet.expired())
    {
      LOG(WARNING) << "AddressResolver failed to receive dependencies.";
    }
  }

  bool AddressResolver::initialize()
  {
    if (auto addressBaseRec = addrBase.lock())
    {
      if (auto versionGetter = verGet.lock())
      {
        if (addressBaseRec->initialisationDone() && versionGetter->initialisationDone())
        {
          addressBase = addressBaseRec->getBaseAddress();
          version = versionGetter->getCurrentStrongholdVersion();

          initialized = true;
          if (!requestAddresses(versionGetter->returnUsedAddresses(), *versionGetter))
          {
            initialized = false;
          }
          else
          {
            LOG(INFO) << "AddressResolver initialized.";
          }
        }
      }
    }

    if (!initialized)
    {
      LOG(WARNING) << "AddressResolver was not initialized.";
    }

    return initialized;
  }

  template <typename T>
  const T* AddressResolver::getAddressPointer(const Address memAddr, const ModBase &requestingMod)
  {
    if (initialized)
    {

      DWORD address{ getAddress(memAddr) };
      if (const auto& modAddrIt = addressSortContainer.find(address); modAddrIt != addressSortContainer.end())
      {
        if (const auto& modIt = (modAddrIt->second).find(requestingMod.getModType()); modIt != (modAddrIt->second).end())
        {
          bool allowed{ false };
          for (const auto& reg : (modIt->second))
          {
            allowed = allowed || reg->address == memAddr;
          }

          if (allowed)
          {
            return reinterpret_cast<T*>(addressBase + address);
          }
        }
      }

      throw std::exception(("The address of type with id '" + std::to_string(static_cast<int>(memAddr))
                            + "' was not approved for mod with id '" + std::to_string(static_cast<int>(requestingMod.getModType())) + "'.").c_str());
    }

    throw std::exception("AddressResolver wasn't successfully initialized, but \"getAddressPointer\" was still called.");
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

    // because length 1 is only start address
    if (size_t length = req.length.at(version) - 1 > 0)
    {
      endAddress = startAddress + length;
    }

    return std::pair<DWORD, DWORD>(startAddress, endAddress);
  }

  // fills addressesWhereToAddRequest, addToNewAddressStart and addToNewAddressEnd
  // also return a bool that says "true" if the risk level was violated
  // TODO?: the unordered_maps are still redundantly filled, any worthwhile check possible?
  // NOTE: length 1 addresses and start/end overlaps will create an overhead that needs to be filtered in the return
  const bool AddressResolver::checkRiskAndOverlaps(
    const ModType newToAddType, const AddressRequest &newToAddReq, const std::pair<DWORD, DWORD> newAddrStartEnd,
    const std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &reqInPlace, std::unordered_set<DWORD> &addressesWhereToAddRequest,
    std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &addToNewAddressStart, std::unordered_map<ModType, std::unordered_set<AddressRequest*>> &addToNewAddressEnd)
  {
    bool riskToHigh{ false };
    size_t i{ 0 };
    while (!riskToHigh && i < reqInPlace.size())
    {
      const auto& otherReqSet = std::next(reqInPlace.begin(), i);

      size_t j{ 0 };
      while (!riskToHigh && j < (otherReqSet->second).size())
      {
        const auto& otherReq = *std::next((otherReqSet->second).begin(), j);

        const std::pair<DWORD, DWORD> otherRange{ getStartAndEndAddress(*otherReq) };
        bool overlap{ newAddrStartEnd.first <= otherRange.second || newAddrStartEnd.second >= otherRange.first };

        if (overlap)
        {
          riskToHigh = checkIfAddressRiskViolated(newToAddReq.addressRisk, otherReq->addressRisk);

          if (!riskToHigh)
          {
            bool startInside{ newAddrStartEnd.first > otherRange.first }; // if both firsts are the same, treat them as outside -> add to already there
            bool endInside{ newAddrStartEnd.second < otherRange.second }; // if both ends are the same, treat them as outside -> add to already there

            // NOTE: 1 length addresses and start/end overlaps will create an overhead, that needs to be filtered in the return

            if (startInside && endInside) // new start inside + new end inside -> add request pointer old to new start and new end
            {
              addToNewAddressStart[otherReqSet->first].insert(otherReq);
              addToNewAddressEnd[otherReqSet->first].insert(otherReq);
            }
            else if (startInside && !endInside) // new start inside + new end outside -> add request pointer old to new start and new request pointer to old end
            {
              addToNewAddressStart[otherReqSet->first].insert(otherReq);
              addressesWhereToAddRequest.insert(otherRange.second);
            }
            else if (!startInside && endInside) // new start outside + new end inside -> add request pointer old to new end and new request pointer to old start
            {
              addressesWhereToAddRequest.insert(otherRange.first);
              addToNewAddressEnd[otherReqSet->first].insert(otherReq);
            }
            else // (!startInside && !endInside) // new start outside + new end outside -> add new request pointer to old start and old end
            {
              addressesWhereToAddRequest.insert(otherRange.first);
              addressesWhereToAddRequest.insert(otherRange.second);
            }
          }
        }
        ++j;
      }
      ++i;
    }

    return riskToHigh;
  }

  const bool AddressResolver::requestAddresses(std::vector<AddressRequest> &addrReq, const ModBase &requestingMod)
  {
    if (initialized)
    {

      // save ref to unordered address map, to remove in error case
      std::vector<std::unordered_map<ModType, std::unordered_set<AddressRequest*>>*> addedAddresses; // used to identify maps
      std::unordered_set<AddressRequest*> addedRequests; // used to only delete newly requested addresses

      // TODO
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

          // TODO -> check iteration
          // only start
          if (addrStartEnd.second == 0x0)
          {
            //search if already in
            if (const auto& addrIt = addressSortContainer.find(addrStartEnd.first); addrIt != addressSortContainer.end())
            {
              addrIt;
            }
            else
            {
              auto& addrCon = addressSortContainer[addrStartEnd.first];
              addrCon[requestingMod.getModType()] = &req;

              // if an overlap exists, it needs to be either the next node itself, or another, that should have left the note, therefore we take the next
              // we created it, so the address needs to be there
              if (const auto& nextIt = std::next(addressSortContainer.find(addrStartEnd.first)); nextIt != addressSortContainer.end())
              {
                for (const auto& otherReq : nextIt->second)
                {

                }
              }

              addedAddresses.push_back(&addrCon);  // add as ref in case of delete
            }
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
          for (const auto& addr : mapOfMods->at(requestingMod.getModType()))
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
            mapOfMods->erase(requestingMod.getModType());
          }
          else
          {
            for (const auto rem : toRemove)
            {
              mapOfMods->at(requestingMod.getModType()).erase(rem);
            }
          }
        }

        LOG(WARNING) << "Failed granting access of addresses to mod with id '" << std::to_string(static_cast<int>(requestingMod.getModType())) << "'.";
        return false;
      }
    }

    throw std::exception("AddressResolver wasn't successfully initialized, but \"requestAddresses\" was still called.");
  }
}