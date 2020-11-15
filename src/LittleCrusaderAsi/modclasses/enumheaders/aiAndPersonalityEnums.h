#ifndef AIANDPERSONALITYENUMS
#define AIANDPERSONALITYENUMS

// json
#include "../../dependencies/JsonForModernC++/json.hpp"

namespace modclasses
{
  // NOTE: all enum values where taken from the Unoffical Crusader Patch (https://github.com/Sh0wdown/UnofficialCrusaderPatch)
  // so the credit for this values goes to them
  // Version of the ENUMs should be from the UCP 2.14

  // while it seems tempting to not use enum classes here, I will not (until I am too annoyed)

  //* CharNames, including the ingame ids *//
  enum class AICharacterName
  {
    NONE = 0,
    RAT = 1,
    SNAKE = 2,
    PIG = 3,
    WOLF = 4,
    SALADIN = 5,
    CALIPH = 6,
    SULTAN = 7,
    RICHARD = 8,
    FREDERICK = 9,
    PHILLIP = 10,
    WAZIR = 11,
    EMIR = 12,
    NIZAR = 13,
    SHERIFF = 14,
    MARSHAL = 15,
    ABBOT = 16
  };

  // used to parse string to enum
  NLOHMANN_JSON_SERIALIZE_ENUM(AICharacterName, {
    {AICharacterName::NONE, nullptr},
    {AICharacterName::RAT, "Rat"},
    {AICharacterName::SNAKE , "Snake"},
    {AICharacterName::PIG, "Pig"},
    {AICharacterName::WOLF, "Wolf"},
    {AICharacterName::SALADIN, "Saladin"},
    {AICharacterName::CALIPH, "Caliph"},
    {AICharacterName::SULTAN, "Sultan"},
    {AICharacterName::RICHARD, "Richard"},
    {AICharacterName::FREDERICK, "Frederick"},
    {AICharacterName::PHILLIP, "Phillip"},
    {AICharacterName::WAZIR, "Wazir"},
    {AICharacterName::EMIR, "Emir"},
    {AICharacterName::NIZAR, "Nizar"},
    {AICharacterName::SHERIFF, "Sheriff"},
    {AICharacterName::MARSHAL, "Marshal"},
    {AICharacterName::ABBOT, "Abbot"}
                               })

  //* Farm-Ids, used to define which are build *//
  enum class FarmBuilding
  {
    NONE = 0xFFFF,  // since 0 is already used
    NO_FARM = 0x00,
    WHEAT_FARM = 0x1E,
    HOP_FARM = 0x1F,
    APPLE_FARM = 0x20,
    DAIRY_FARM = 0x21
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(FarmBuilding, {
    {FarmBuilding::NONE, nullptr},
    {FarmBuilding::NO_FARM, "None"},
    {FarmBuilding::WHEAT_FARM, "WheatFarm"},
    {FarmBuilding::HOP_FARM, "HopFarm"},
    {FarmBuilding::APPLE_FARM, "AppleFarm"},
    {FarmBuilding::DAIRY_FARM, "DairyFarm"}
  })

  //* Resource-Ids *//

  enum class Resource
  {
    NONE = 0xFFFF,
    NO_RESOURCE = 0,
    WOOD = 0x02,
    HOP = 0x03,
    STONE = 0x04,
    IRON = 0x06,
    PITCH = 0x07,
    WHEAT = 0x09,
    BREAD = 0x0A,
    CHEESE = 0x0B,
    MEAT = 0x0C,
    APPLES = 0x0D,
    BEER = 0x0E,
    FLOUR = 0x10,
    BOWS = 0x11,
    CROSSBOWS = 0x12,
    SPEARS = 0x13,
    PIKES = 0x14,
    MACES = 0x15,
    SWORDS = 0x16,
    LEATHERARMORS = 0x17,
    IRONARMORS = 0x18
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(Resource, {
    {Resource::NONE, nullptr},
    {Resource::NO_RESOURCE, "None"},
    {Resource::WOOD, "Wood"},
    {Resource::HOP, "Hop"},
    {Resource::STONE, "Stone"},
    {Resource::IRON, "Iron"},
    {Resource::PITCH, "Pitch"},
    {Resource::WHEAT, "Wheat"},
    {Resource::BREAD, "Bread"},
    {Resource::CHEESE, "Cheese"},
    {Resource::MEAT, "Meat"},
    {Resource::APPLES, "Apples"},
    {Resource::BEER, "Beer"},
    {Resource::FLOUR, "Flour"},
    {Resource::BOWS, "Bows"},
    {Resource::CROSSBOWS, "Crossbows"},
    {Resource::SPEARS, "Spears"},
    {Resource::PIKES, "Pikes"},
    {Resource::MACES, "Maces"},
    {Resource::SWORDS, "Swords"},
    {Resource::LEATHERARMORS, "LeatherArmors"},
    {Resource::IRONARMORS, "IronArmors"}
  })

  //* Siege-Engine-Ids *//

  enum class SiegeEngine
  {
    NONE = 0xFFFF,
    NO_SIEGE_ENGINE = 0x00,
    CATAPULT = 0x27,
    TREBUCHET = 0x28,
    MANGONEL = 0x29,
    TENT = 0x32,
    SIEGETOWER = 0x3A,
    BATTERINGRAM = 0x3B,
    SHIELD = 0x3C,
    TOWERBALLISTA = 0x3D,
    FIREBALLISTA = 0x4D
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(SiegeEngine, {
    {SiegeEngine::NONE, nullptr},
    {SiegeEngine::NO_SIEGE_ENGINE, "None"},
    {SiegeEngine::CATAPULT, "Catapult"},
    {SiegeEngine::TREBUCHET, "Trebuchet"},
    {SiegeEngine::MANGONEL, "Mangonel"},
    {SiegeEngine::TENT, "Tent"},
    {SiegeEngine::SIEGETOWER, "SiegeTower"},
    {SiegeEngine::BATTERINGRAM, "BatteringRam"},
    {SiegeEngine::SHIELD, "Shield"},
    {SiegeEngine::TOWERBALLISTA, "TowerBallista"},
    {SiegeEngine::FIREBALLISTA, "FireBallista"}
  })

  enum class HarassingSiegeEngine
  {
    NONE = 0xFFFF,
    NO_SIEGE_ENGINE = 0x00,
    CATAPULT = 0xBE,
    FIREBALLISTA = 0x166
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(HarassingSiegeEngine, {
    {HarassingSiegeEngine::NONE, nullptr},
    {HarassingSiegeEngine::NO_SIEGE_ENGINE, "None"},
    {HarassingSiegeEngine::CATAPULT, "Catapult"},
    {HarassingSiegeEngine::FIREBALLISTA, "FireBallista"}
  })

  //* Target IDs -> what will the ai attack *//

  enum class TargetingType
  {
    NONE = 0xFFFF,  // wrong input
    GOLD = 0, // Highest gold
    BALANCED = 1, // average of weakest / closest enemy
    CLOSEST = 2,  // closest enemy
    ANY = 3, // Nothing specified
    PLAYER = 4  // player or if no players are left for the closest AI
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(TargetingType, {
    {TargetingType::NONE, nullptr},
    {TargetingType::GOLD, "Gold"},
    {TargetingType::BALANCED, "Balanced"},
    {TargetingType::CLOSEST, "Closest"},
    {TargetingType::ANY, "Any"},
    {TargetingType::PLAYER, "Player"}
  })

  //* Settings for weapon workshops *//

  enum class FletcherSetting
  {
    NONE = 0xFFFF,  // wrong input
    BOWS = 0x11,
    CROSSBOWS = 0x12,
    BOTH = -999
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(FletcherSetting, {
    {FletcherSetting::NONE, nullptr},
    {FletcherSetting::BOWS, "Bows"},
    {FletcherSetting::CROSSBOWS, "Crossbows"},
    {FletcherSetting::BOTH, "Both"}
  })

  enum class PoleturnerSetting
  {
    NONE = 0xFFFF,  // wrong input
    SPEARS = 0x13,
    PIKES = 0x14,
    BOTH = -999
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(PoleturnerSetting, {
    {PoleturnerSetting::NONE, nullptr},
    {PoleturnerSetting::SPEARS, "Spears"},
    {PoleturnerSetting::PIKES, "Pikes"},
    {PoleturnerSetting::BOTH, "Both"}
  })

  enum class BlacksmithSetting
  {
    NONE = 0xFFFF,  // wrong input
    MACES = 0x15,
    SWORDS = 0x16,
    BOTH = -999
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(BlacksmithSetting, {
    {BlacksmithSetting::NONE, nullptr},
    {BlacksmithSetting::MACES, "Maces"},
    {BlacksmithSetting::SWORDS, "Swords"},
    {BlacksmithSetting::BOTH, "Both"}
  })

  //* Unit and npc ids *//

  enum class Unit
  {
    NONE = 0xFFFF,  // wrong input
    NO_UNIT = 0x00,
    TUNNELER = 0x05,
    EUROPARCHER = 0x16,
    CROSSBOWMAN = 0x17,
    SPEARMAN = 0x18,
    PIKEMAN = 0x19,
    MACEMAN = 0x1A,
    SWORDSMAN = 0x1B,
    KNIGHT = 0x1C,
    LADDERMAN = 0x1D,
    ENGINEER = 0x1E,
    MONK = 0x25,
    ARABARCHER = 0x46,
    SLAVE = 0x47,
    SLINGER = 0x48,
    ASSASSIN = 0x49,
    HORSEARCHER = 0x4A,
    ARABSWORDSMAN = 0x4B,
    FIRETHROWER = 0x4C
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(Unit, {
    {Unit::NONE, nullptr},
    {Unit::NO_UNIT, "None"},
    {Unit::TUNNELER, "Tunneler"},
    {Unit::EUROPARCHER, "EuropArcher"},
    {Unit::CROSSBOWMAN, "Crossbowman"},
    {Unit::SPEARMAN, "Spearman"},
    {Unit::PIKEMAN, "Pikeman"},
    {Unit::MACEMAN, "Maceman"},
    {Unit::SWORDSMAN, "Swordsman"},
    {Unit::KNIGHT, "Knight"},
    {Unit::LADDERMAN, "Ladderman"},
    {Unit::ENGINEER, "Engineer"},
    {Unit::MONK, "Monk"},
    {Unit::ARABARCHER, "ArabArcher"},
    {Unit::SLAVE, "Slave"},
    {Unit::SLINGER, "Slinger"},
    {Unit::ASSASSIN, "Assassin"},
    {Unit::HORSEARCHER, "HorseArcher"},
    {Unit::ARABSWORDSMAN, "ArabSwordsman"},
    {Unit::FIRETHROWER, "FireThrower"}
  })

  enum class DiggingUnit
  {
    NONE = 0xFFFF,  // wrong input
    NO_DIGGING_UNIT = 0x00,
    EUROPARCHER = 0x16,
    SPEARMAN = 0x18,
    PIKEMAN = 0x19,
    MACEMAN = 0x1A,
    ENGINEER = 0x1E,
    SLAVE = 0x47
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(DiggingUnit, {
    {DiggingUnit::NONE, nullptr},
    {DiggingUnit::NO_DIGGING_UNIT, "None"},
    {DiggingUnit::EUROPARCHER, "EuropArcher"},
    {DiggingUnit::SPEARMAN, "Spearman"},
    {DiggingUnit::PIKEMAN, "Pikeman"},
    {DiggingUnit::MACEMAN, "Maceman"},
    {DiggingUnit::ENGINEER, "Engineer"},
    {DiggingUnit::SLAVE, "Slave"}
  })

  enum class MeleeUnit
  {
    NONE = 0xFFFF,  // wrong input
    NO_MELEE_UNIT = 0x00,
    TUNNELER = 0x05,
    SPEARMAN = 0x18,
    PIKEMAN = 0x19,
    MACEMAN = 0x1A,
    SWORDSMAN = 0x1B,
    KNIGHT = 0x1C,
    //LADDERMAN = 0x1D,
    //ENGINEER = 0x1E,
    MONK = 0x25,
    SLAVE = 0x47,
    ASSASSIN = 0x49,
    ARABSWORDSMAN = 0x4B
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(MeleeUnit, {
    {MeleeUnit::NONE, nullptr},
    {MeleeUnit::NO_MELEE_UNIT, "None"},
    {MeleeUnit::TUNNELER, "Tunneler"},
    {MeleeUnit::SPEARMAN, "Spearman"},
    {MeleeUnit::PIKEMAN, "Pikeman"},
    {MeleeUnit::MACEMAN, "Maceman"},
    {MeleeUnit::SWORDSMAN, "Swordsman"},
    {MeleeUnit::KNIGHT, "Knight"},
    //{MeleeUnit::LADDERMAN, "Ladderman"},
    //{MeleeUnit::ENGINEER, "Engineer"},
    {MeleeUnit::MONK, "Monk"},
    {MeleeUnit::SLAVE, "Slave"},
    {MeleeUnit::ASSASSIN, "Assassin"},
    {MeleeUnit::ARABSWORDSMAN, "ArabSwordsman"}
  })

  enum class RangedUnit
  {
    NONE = 0xFFFF,  // wrong input
    NO_RANGED_UNIT = 0x00,
    EUROPARCHER = 0x16,
    CROSSBOWMAN = 0x17,
    ARABARCHER = 0x46,
    SLINGER = 0x48,
    HORSEARCHER = 0x4A,
    FIRETHROWER = 0x4C
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(RangedUnit, {
    {RangedUnit::NONE, nullptr},
    {RangedUnit::NO_RANGED_UNIT, "None"},
    {RangedUnit::EUROPARCHER, "EuropArcher"},
    {RangedUnit::CROSSBOWMAN, "Crossbowman"},
    {RangedUnit::ARABARCHER, "ArabArcher"},
    {RangedUnit::SLINGER, "Slinger"},
    {RangedUnit::HORSEARCHER, "HorseArcher"},
    {RangedUnit::FIRETHROWER, "FireThrower"}
  })

  enum class NPC
  {
    NONE = 0x00,
    PEASANT = 1,
    BURNINGMAN = 2,
    LUMBERJACK = 3,
    FLETCHER = 4,
    TUNNELER = 5,
    HUNTER = 6,
    UNKNOWN_7 = 7,
    UNKNOWN_8 = 8,
    UNKNOWN_9 = 9,
    PITCHWORKER = 10,
    UNKNOWN_11 = 11,
    UNKNOWN_12 = 12,
    UNKNOWN_13 = 13,
    UNKNOWN_14 = 14,
    CHILD = 15,
    BAKER = 16,
    WOMAN = 17,
    POLETURNER = 18,
    SMITH = 19,
    ARMORER = 20,
    TANNER = 21,
    EUROPARCHER = 22,
    CROSSBOWMAN = 23,
    SPEARMAN = 24,
    PIKEMAN = 25,
    MACEMAN = 26,
    SWORDSMAN = 27,
    KNIGHT = 28,
    LADDERMAN = 29,
    ENGINEER = 30,
    UNKNOWN_31 = 31,
    UNKNOWN_32 = 32,
    PRIEST = 33,
    UNKNOWN_34 = 34,
    DRUNKARD = 35,
    INNKEEPER = 36,
    MONK = 37,
    UNKNOWN_38 = 38,
    CATAPULT = 39,
    TREBUCHET = 40,
    MANGONEL = 41,
    UNKNOWN_42 = 42,
    UNKNOWN_43 = 43,
    ANTELOPE = 44,
    LION = 45,
    RABBIT = 46,
    CAMEL = 47,
    UNKNOWN_48 = 48,
    UNKNOWN_49 = 49,
    SIEGETENT = 50,
    UNKNOWN_51 = 51,
    UNKNOWN_52 = 52,
    FIREMAN = 53,
    GHOST = 54,
    LORD = 55,
    LADY = 56,
    JESTER = 57,
    SIEGETOWER = 58,
    RAM = 59,
    SHIELD = 60,
    BALLISTA = 61,
    CHICKEN = 62,
    UNKNOWN_63 = 63,
    UNKNOWN_64 = 64,
    JUGGLER = 65,
    FIREEATER = 66,
    DOG = 67,
    UNKNOWN_68 = 68,
    UNKNOWN_69 = 69,
    ARABARCHER = 70,
    SLAVE = 71,
    SLINGER = 72,
    ASSASSIN = 73,
    HORSEARCHER = 74,
    ARABSWORDSMAN = 75,
    FIRETHROWER = 76,
    FIREBALLISTA = 77
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(NPC, {
    {NPC::NONE, nullptr},
    {NPC::PEASANT, "Peasant"},
    {NPC::BURNINGMAN, "BurningMan"},
    {NPC::LUMBERJACK, "Lumberjack"},
    {NPC::FLETCHER, "Fletcher"},
    {NPC::TUNNELER, "Tunneler"},
    {NPC::HUNTER, "Hunter"},
    {NPC::UNKNOWN_7, "Unknown7"},
    {NPC::UNKNOWN_8, "Unknown8"},
    {NPC::UNKNOWN_9, "Unknown9"},
    {NPC::PITCHWORKER, "PitchWorker"},
    {NPC::UNKNOWN_11, "Unknown11"},
    {NPC::UNKNOWN_12, "Unknown12"},
    {NPC::UNKNOWN_13, "Unknown13"},
    {NPC::UNKNOWN_14, "Unknown14"},
    {NPC::CHILD, "Child"},
    {NPC::BAKER, "Baker"},
    {NPC::WOMAN, "Woman"},
    {NPC::POLETURNER, "Poleturner"},
    {NPC::SMITH, "Smith"},
    {NPC::ARMORER, "Armorer"},
    {NPC::TANNER, "Tanner"},
    {NPC::EUROPARCHER, "EuropArcher"},
    {NPC::CROSSBOWMAN, "CrossbowMan"},
    {NPC::SPEARMAN, "Spearman"},
    {NPC::PIKEMAN, "Pikeman"},
    {NPC::MACEMAN, "Maceman"},
    {NPC::SWORDSMAN, "Swordsman"},
    {NPC::KNIGHT, "Knight"},
    {NPC::LADDERMAN, "Ladderman"},
    {NPC::ENGINEER, "Engineer"},
    {NPC::UNKNOWN_31, "Unknown31"},
    {NPC::UNKNOWN_32, "Unknown32"},
    {NPC::PRIEST, "Priest"},
    {NPC::UNKNOWN_34, "Unknown34"},
    {NPC::DRUNKARD, "Drunkard"},
    {NPC::INNKEEPER, "Innkeeper"},
    {NPC::MONK, "Monk"},
    {NPC::UNKNOWN_38, "Unknown38"},
    {NPC::CATAPULT, "Catapult"},
    {NPC::TREBUCHET, "Trebuchet"},
    {NPC::MANGONEL, "Mangonel"},
    {NPC::UNKNOWN_42, "Unknown42"},
    {NPC::UNKNOWN_43, "Unknown43"},
    {NPC::ANTELOPE, "Antelope"},
    {NPC::LION, "Lion"},
    {NPC::RABBIT, "Rabbit"},
    {NPC::CAMEL, "Camel"},
    {NPC::UNKNOWN_48, "Unknown48"},
    {NPC::UNKNOWN_49, "Unknown49"},
    {NPC::SIEGETENT, "SiegeTent"},
    {NPC::UNKNOWN_51, "Unknown51"},
    {NPC::UNKNOWN_52, "Unknown52"},
    {NPC::FIREMAN, "Fireman"},
    {NPC::GHOST, "Ghost"},
    {NPC::LORD, "Lord"},
    {NPC::LADY, "Lady"},
    {NPC::JESTER, "Jester"},
    {NPC::SIEGETOWER, "Siegetower"},
    {NPC::RAM, "Ram"},
    {NPC::SHIELD, "Shield"},
    {NPC::BALLISTA, "Ballista"},
    {NPC::CHICKEN, "Chicken"},
    {NPC::UNKNOWN_63, "Unknown63"},
    {NPC::UNKNOWN_64, "Unknown64"},
    {NPC::JUGGLER, "Juggler"},
    {NPC::FIREEATER, "FireEater"},
    {NPC::DOG, "Dog"},
    {NPC::UNKNOWN_68, "Unknown68"},
    {NPC::UNKNOWN_69, "Unknown69"},
    {NPC::ARABARCHER, "ArabArcher"},
    {NPC::SLAVE, "Slave"},
    {NPC::SLINGER, "Slinger"},
    {NPC::ASSASSIN, "Assassin"},
    {NPC::HORSEARCHER, "HorseArcher"},
    {NPC::ARABSWORDSMAN, "ArabSwordsman"},
    {NPC::FIRETHROWER, "FireThrower"},
    {NPC::FIREBALLISTA, "FireBallista"}
  })

  //* Unit and npc ids *//

  enum class AIPersonalityFieldsEnum
  {
    UNKNOWN_000,
    UNKNOWN_001,
    UNKNOWN_002,
    UNKNOWN_003,
    UNKNOWN_004,
    UNKNOWN_005,
    CRITICAL_POPULARITY,
    LOWEST_POPULARITY,
    HIGHEST_POPULARITY,
    TAXES_MIN,
    TAXES_MAX,
    UNKNOWN_011,
    FARM_1,
    FARM_2,
    FARM_3,
    FARM_4,
    FARM_5,
    FARM_6,
    FARM_7,
    FARM_8,
    POPULATION_PER_FARM,
    POPULATION_PER_WOODCUTTER,
    POPULATION_PER_QUARRY,
    POPULATION_PER_IRONMINE,
    POPULATION_PER_PITCHRIG,
    MAX_QUARRIES,
    MAX_IRONMINES,
    MAX_WOODCUTTERS,
    MAX_PITCHRIGS,
    MAX_FARMS,
    BUILD_INTERVAL,
    RESOURCE_REBUILD_DELAY,
    MAX_FOOD,
    MINIMUM_APPLES,
    MINIMUM_CHEESE,
    MINIMUM_BREAD,
    MINIMUM_WHEAT,
    MINIMUM_HOP,
    TRADE_AMOUNT_FOOD,
    TRADE_AMOUNT_EQUIPMENT,
    UNKNOWN_040,
    MINIMUM_GOODS_REQUIRED_AFTER_TRADE,
    DOUBLE_RATIONS_FOOD_THRESHOLD,
    MAX_WOOD,
    MAX_STONE,
    MAX_RESOURCE_OTHER,
    MAX_EQUIPMENT,
    MAX_BEER,
    MAX_RESOURCE_VARIANCE,
    RECRUIT_GOLD_THRESHOLD,
    BLACKSMITH_SETTING,
    FLETCHER_SETTING,
    POLETURNER_SETTING,
    SELL_RESOURCE_01,
    SELL_RESOURCE_02,
    SELL_RESOURCE_03,
    SELL_RESOURCE_04,
    SELL_RESOURCE_05,
    SELL_RESOURCE_06,
    SELL_RESOURCE_07,
    SELL_RESOURCE_08,
    SELL_RESOURCE_09,
    SELL_RESOURCE_10,
    SELL_RESOURCE_11,
    SELL_RESOURCE_12,
    SELL_RESOURCE_13,
    SELL_RESOURCE_14,
    SELL_RESOURCE_15,
    DEF_WALLPATROL_RALLYTIME,
    DEF_WALLPATROL_GROUPS,
    DEF_SIEGEENGINE_GOLD_THRESHOLD,
    DEF_SIEGEENGINE_BUILD_DELAY,
    UNKNOWN_072,
    UNKNOWN_073,
    RECRUIT_PROB_DEF_DEFAULT,
    RECRUIT_PROB_DEF_WEAK,
    RECRUIT_PROB_DEF_STRONG,
    RECRUIT_PROB_RAID_DEFAULT,
    RECRUIT_PROB_RAID_WEAK,
    RECRUIT_PROB_RAID_STRONG,
    RECRUIT_PROB_ATTACK_DEFAULT,
    RECRUIT_PROB_ATTACK_WEAK,
    RECRUIT_PROB_ATTACK_STRONG,
    SORTIE_UNIT_RANGED_MIN,
    SORTIE_UNIT_RANGED,
    SORTIE_UNIT_MELEE_MIN,
    SORTIE_UNIT_MELEE,
    DEF_DIGGING_UNIT_MAX,
    DEF_DIGGING_UNIT,
    RECRUIT_INTERVAL,
    RECRUIT_INTERVAL_WEAK,
    RECRUIT_INTERVAL_STRONG,
    DEF_TOTAL,
    OUTER_PATROL_GROUPS_COUNT,
    OUTER_PATROL_GROUPS_MOVE,
    OUTER_PATROL_RALLY_DELAY,
    DEF_WALLS,
    DEF_UNIT_1,
    DEF_UNIT_2,
    DEF_UNIT_3,
    DEF_UNIT_4,
    DEF_UNIT_5,
    DEF_UNIT_6,
    DEF_UNIT_7,
    DEF_UNIT_8,
    RAID_UNITS_BASE,
    RAID_UNITS_RANDOM,
    RAID_UNIT_1,
    RAID_UNIT_2,
    RAID_UNIT_3,
    RAID_UNIT_4,
    RAID_UNIT_5,
    RAID_UNIT_6,
    RAID_UNIT_7,
    RAID_UNIT_8,
    HARASSING_SIEGE_ENGINE_1,
    HARASSING_SIEGE_ENGINE_2,
    HARASSING_SIEGE_ENGINE_3,
    HARASSING_SIEGE_ENGINE_4,
    HARASSING_SIEGE_ENGINE_5,
    HARASSING_SIEGE_ENGINE_6,
    HARASSING_SIEGE_ENGINE_7,
    HARASSING_SIEGE_ENGINE_8,
    HARASSING_SIEGE_ENGINES_MAX,
    UNKNOWN_124,
    ATT_FORCE_BASE,
    ATT_FORCE_RANDOM,
    ATT_FORCE_SUPPORT_ALLY_THRESHOLD,
    ATT_FORCE_RALLY_PERCENTAGE,
    UNKNOWN_129,
    UNKNOWN_130,
    ATT_UNIT_PATROL_RECOMMAND_DELAY,
    UNKNOWN_132,
    SIEGE_ENGINE_1,
    SIEGE_ENGINE_2,
    SIEGE_ENGINE_3,
    SIEGE_ENGINE_4,
    SIEGE_ENGINE_5,
    SIEGE_ENGINE_6,
    SIEGE_ENGINE_7,
    SIEGE_ENGINE_8,
    COW_THROW_INTERVAL,
    UNKNOWN_142,
    ATT_MAX_ENGINEERS,
    ATT_DIGGING_UNIT,
    ATT_DIGGING_UNIT_MAX,
    ATT_UNIT_2,
    ATT_UNIT_2_MAX,
    ATT_MAX_ASSASSINS,
    ATT_MAX_LADDERMEN,
    ATT_MAX_TUNNELERS,
    ATT_UNIT_PATROL,
    ATT_UNIT_PATROL_MAX,
    ATT_UNIT_PATROL_GROUPS_COUNT,
    ATT_UNIT_BACKUP,
    ATT_UNIT_BACKUP_MAX,
    ATT_UNIT_BACKUP_GROUPS_COUNT,
    ATT_UNIT_ENGAGE,
    ATT_UNIT_ENGAGE_MAX,
    ATT_UNIT_SIEGE_DEF,
    ATT_UNIT_SIEGE_DEF_MAX,
    ATT_UNIT_SIEGE_DEF_GROUPS_COUNT,
    ATT_UNIT_MAIN_1,
    ATT_UNIT_MAIN_2,
    ATT_UNIT_MAIN_3,
    ATT_UNIT_MAIN_4,
    ATT_MAX_DEFAULT,
    ATT_MAIN_GROUPS_COUNT,
    TARGET_CHOICE,
    NONE  // set at the end, so the first enum stays value 0
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(AIPersonalityFieldsEnum, {
    {AIPersonalityFieldsEnum::NONE, nullptr},
    {AIPersonalityFieldsEnum::UNKNOWN_000, "Unknown000"},
    {AIPersonalityFieldsEnum::UNKNOWN_001, "Unknown001"},
    {AIPersonalityFieldsEnum::UNKNOWN_002, "Unknown002"},
    {AIPersonalityFieldsEnum::UNKNOWN_003, "Unknown003"},
    {AIPersonalityFieldsEnum::UNKNOWN_004, "Unknown004"},
    {AIPersonalityFieldsEnum::UNKNOWN_005, "Unknown005"},
    {AIPersonalityFieldsEnum::CRITICAL_POPULARITY, "CriticalPopularity"},
    {AIPersonalityFieldsEnum::LOWEST_POPULARITY, "LowestPopularity"},
    {AIPersonalityFieldsEnum::HIGHEST_POPULARITY, "HighestPopularity"},
    {AIPersonalityFieldsEnum::TAXES_MIN, "TaxesMin"},
    {AIPersonalityFieldsEnum::TAXES_MAX, "TaxesMax"},
    {AIPersonalityFieldsEnum::UNKNOWN_011, "Unknown011"},
    {AIPersonalityFieldsEnum::FARM_1, "Farm1"},
    {AIPersonalityFieldsEnum::FARM_2, "Farm2"},
    {AIPersonalityFieldsEnum::FARM_3, "Farm3"},
    {AIPersonalityFieldsEnum::FARM_4, "Farm4"},
    {AIPersonalityFieldsEnum::FARM_5, "Farm5"},
    {AIPersonalityFieldsEnum::FARM_6, "Farm6"},
    {AIPersonalityFieldsEnum::FARM_7, "Farm7"},
    {AIPersonalityFieldsEnum::FARM_8, "Farm8"},
    {AIPersonalityFieldsEnum::POPULATION_PER_FARM, "PopulationPerFarm"},
    {AIPersonalityFieldsEnum::POPULATION_PER_WOODCUTTER, "PopulationPerWoodcutter"},
    {AIPersonalityFieldsEnum::POPULATION_PER_QUARRY, "PopulationPerQuarry"},
    {AIPersonalityFieldsEnum::POPULATION_PER_IRONMINE, "PopulationPerIronmine"},
    {AIPersonalityFieldsEnum::POPULATION_PER_PITCHRIG, "PopulationPerPitchrig"},
    {AIPersonalityFieldsEnum::MAX_QUARRIES, "MaxQuarries"},
    {AIPersonalityFieldsEnum::MAX_IRONMINES, "MaxIronmines"},
    {AIPersonalityFieldsEnum::MAX_WOODCUTTERS, "MaxWoodcutters"},
    {AIPersonalityFieldsEnum::MAX_PITCHRIGS, "MaxPitchrigs"},
    {AIPersonalityFieldsEnum::MAX_FARMS, "MaxFarms"},
    {AIPersonalityFieldsEnum::BUILD_INTERVAL, "BuildInterval"},
    {AIPersonalityFieldsEnum::RESOURCE_REBUILD_DELAY, "ResourceRebuildDelay"},
    {AIPersonalityFieldsEnum::MAX_FOOD, "MaxFood"},
    {AIPersonalityFieldsEnum::MINIMUM_APPLES, "MinimumApples"},
    {AIPersonalityFieldsEnum::MINIMUM_CHEESE, "MinimumCheese"},
    {AIPersonalityFieldsEnum::MINIMUM_BREAD, "MinimumBread"},
    {AIPersonalityFieldsEnum::MINIMUM_WHEAT, "MinimumWheat"},
    {AIPersonalityFieldsEnum::MINIMUM_HOP, "MinimumHop"},
    {AIPersonalityFieldsEnum::TRADE_AMOUNT_FOOD, "TradeAmountFood"},
    {AIPersonalityFieldsEnum::TRADE_AMOUNT_EQUIPMENT, "TradeAmountEquipment"},
    {AIPersonalityFieldsEnum::UNKNOWN_040, "Unknown040"},
    {AIPersonalityFieldsEnum::MINIMUM_GOODS_REQUIRED_AFTER_TRADE, "MinimumGoodsRequiredAfterTrade"},
    {AIPersonalityFieldsEnum::DOUBLE_RATIONS_FOOD_THRESHOLD, "DoubleRationsFoodThreshold"},
    {AIPersonalityFieldsEnum::MAX_WOOD, "MaxWood"},
    {AIPersonalityFieldsEnum::MAX_STONE, "MaxStone"},
    {AIPersonalityFieldsEnum::MAX_RESOURCE_OTHER, "MaxResourceOther"},
    {AIPersonalityFieldsEnum::MAX_EQUIPMENT, "MaxEquipment"},
    {AIPersonalityFieldsEnum::MAX_BEER, "MaxBeer"},
    {AIPersonalityFieldsEnum::MAX_RESOURCE_VARIANCE, "MaxResourceVariance"},
    {AIPersonalityFieldsEnum::RECRUIT_GOLD_THRESHOLD, "RecruitGoldThreshold"},
    {AIPersonalityFieldsEnum::BLACKSMITH_SETTING, "BlacksmithSetting"},
    {AIPersonalityFieldsEnum::FLETCHER_SETTING, "FletcherSetting"},
    {AIPersonalityFieldsEnum::POLETURNER_SETTING, "PoleturnerSetting"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_01, "SellResource01"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_02, "SellResource02"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_03, "SellResource03"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_04, "SellResource04"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_05, "SellResource05"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_06, "SellResource06"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_07, "SellResource07"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_08, "SellResource08"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_09, "SellResource09"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_10, "SellResource10"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_11, "SellResource11"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_12, "SellResource12"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_13, "SellResource13"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_14, "SellResource14"},
    {AIPersonalityFieldsEnum::SELL_RESOURCE_15, "SellResource15"},
    {AIPersonalityFieldsEnum::DEF_WALLPATROL_RALLYTIME, "DefWallPatrolRallyTime"},
    {AIPersonalityFieldsEnum::DEF_WALLPATROL_GROUPS, "DefWallPatrolGroups"},
    {AIPersonalityFieldsEnum::DEF_SIEGEENGINE_GOLD_THRESHOLD, "DefSiegeEngineGoldThreshold"},
    {AIPersonalityFieldsEnum::DEF_SIEGEENGINE_BUILD_DELAY, "DefSiegeEngineBuildDelay"},
    {AIPersonalityFieldsEnum::UNKNOWN_072, "Unknown072"},
    {AIPersonalityFieldsEnum::UNKNOWN_073, "Unknown073"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_DEF_DEFAULT, "RecruitProbDefDefault"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_DEF_WEAK, "RecruitProbDefWeak"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_DEF_STRONG, "RecruitProbDefStrong"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_RAID_DEFAULT, "RecruitProbRaidDefault"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_RAID_WEAK, "RecruitProbRaidWeak"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_RAID_STRONG, "RecruitProbRaidStrong"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_ATTACK_DEFAULT, "RecruitProbAttackDefault"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_ATTACK_WEAK, "RecruitProbAttackWeak"},
    {AIPersonalityFieldsEnum::RECRUIT_PROB_ATTACK_STRONG, "RecruitProbAttackStrong"},
    {AIPersonalityFieldsEnum::SORTIE_UNIT_RANGED_MIN, "SortieUnitRangedMin"},
    {AIPersonalityFieldsEnum::SORTIE_UNIT_RANGED, "SortieUnitRanged"},
    {AIPersonalityFieldsEnum::SORTIE_UNIT_MELEE_MIN, "SortieUnitMeleeMin"},
    {AIPersonalityFieldsEnum::SORTIE_UNIT_MELEE, "SortieUnitMelee"},
    {AIPersonalityFieldsEnum::DEF_DIGGING_UNIT_MAX, "DefDiggingUnitMax"},
    {AIPersonalityFieldsEnum::DEF_DIGGING_UNIT, "DefDiggingUnit"},
    {AIPersonalityFieldsEnum::RECRUIT_INTERVAL, "RecruitInterval"},
    {AIPersonalityFieldsEnum::RECRUIT_INTERVAL_WEAK, "RecruitIntervalWeak"},
    {AIPersonalityFieldsEnum::RECRUIT_INTERVAL_STRONG, "RecruitIntervalStrong"},
    {AIPersonalityFieldsEnum::DEF_TOTAL, "DefTotal"},
    {AIPersonalityFieldsEnum::OUTER_PATROL_GROUPS_COUNT, "OuterPatrolGroupsCount"},
    {AIPersonalityFieldsEnum::OUTER_PATROL_GROUPS_MOVE, "OuterPatrolGroupsMove"},
    {AIPersonalityFieldsEnum::OUTER_PATROL_RALLY_DELAY, "OuterPatrolRallyDelay"},
    {AIPersonalityFieldsEnum::DEF_WALLS, "DefWalls"},
    {AIPersonalityFieldsEnum::DEF_UNIT_1, "DefUnit1"},
    {AIPersonalityFieldsEnum::DEF_UNIT_2, "DefUnit2"},
    {AIPersonalityFieldsEnum::DEF_UNIT_3, "DefUnit3"},
    {AIPersonalityFieldsEnum::DEF_UNIT_4, "DefUnit4"},
    {AIPersonalityFieldsEnum::DEF_UNIT_5, "DefUnit5"},
    {AIPersonalityFieldsEnum::DEF_UNIT_6, "DefUnit6"},
    {AIPersonalityFieldsEnum::DEF_UNIT_7, "DefUnit7"},
    {AIPersonalityFieldsEnum::DEF_UNIT_8, "DefUnit8"},
    {AIPersonalityFieldsEnum::RAID_UNITS_BASE, "RaidUnitsBase"},
    {AIPersonalityFieldsEnum::RAID_UNITS_RANDOM, "RaidUnitsRandom"},
    {AIPersonalityFieldsEnum::RAID_UNIT_1, "RaidUnit1"},
    {AIPersonalityFieldsEnum::RAID_UNIT_2, "RaidUnit2"},
    {AIPersonalityFieldsEnum::RAID_UNIT_3, "RaidUnit3"},
    {AIPersonalityFieldsEnum::RAID_UNIT_4, "RaidUnit4"},
    {AIPersonalityFieldsEnum::RAID_UNIT_5, "RaidUnit5"},
    {AIPersonalityFieldsEnum::RAID_UNIT_6, "RaidUnit6"},
    {AIPersonalityFieldsEnum::RAID_UNIT_7, "RaidUnit7"},
    {AIPersonalityFieldsEnum::RAID_UNIT_8, "RaidUnit8"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINE_1, "HarassingSiegeEngine1"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINE_2, "HarassingSiegeEngine2"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINE_3, "HarassingSiegeEngine3"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINE_4, "HarassingSiegeEngine4"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINE_5, "HarassingSiegeEngine5"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINE_6, "HarassingSiegeEngine6"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINE_7, "HarassingSiegeEngine7"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINE_8, "HarassingSiegeEngine8"},
    {AIPersonalityFieldsEnum::HARASSING_SIEGE_ENGINES_MAX, "HarassingSiegeEnginesMax"},
    {AIPersonalityFieldsEnum::UNKNOWN_124, "Unknown124"},
    {AIPersonalityFieldsEnum::ATT_FORCE_BASE, "AttForceBase"},
    {AIPersonalityFieldsEnum::ATT_FORCE_RANDOM, "AttForceRandom"},
    {AIPersonalityFieldsEnum::ATT_FORCE_SUPPORT_ALLY_THRESHOLD, "AttForceSupportAllyThreshold"},
    {AIPersonalityFieldsEnum::ATT_FORCE_RALLY_PERCENTAGE, "AttForceRallyPercentage"},
    {AIPersonalityFieldsEnum::UNKNOWN_129, "Unknown129"},
    {AIPersonalityFieldsEnum::UNKNOWN_130, "Unknown130"},
    {AIPersonalityFieldsEnum::ATT_UNIT_PATROL_RECOMMAND_DELAY, "AttUnitPatrolRecommandDelay"},
    {AIPersonalityFieldsEnum::UNKNOWN_132, "Unknown132"},
    {AIPersonalityFieldsEnum::SIEGE_ENGINE_1, "SiegeEngine1"},
    {AIPersonalityFieldsEnum::SIEGE_ENGINE_2, "SiegeEngine2"},
    {AIPersonalityFieldsEnum::SIEGE_ENGINE_3, "SiegeEngine3"},
    {AIPersonalityFieldsEnum::SIEGE_ENGINE_4, "SiegeEngine4"},
    {AIPersonalityFieldsEnum::SIEGE_ENGINE_5, "SiegeEngine5"},
    {AIPersonalityFieldsEnum::SIEGE_ENGINE_6, "SiegeEngine6"},
    {AIPersonalityFieldsEnum::SIEGE_ENGINE_7, "SiegeEngine7"},
    {AIPersonalityFieldsEnum::SIEGE_ENGINE_8, "SiegeEngine8"},
    {AIPersonalityFieldsEnum::COW_THROW_INTERVAL, "CowThrowInterval"},
    {AIPersonalityFieldsEnum::UNKNOWN_142, "Unknown142"},
    {AIPersonalityFieldsEnum::ATT_MAX_ENGINEERS, "AttMaxEngineers"},
    {AIPersonalityFieldsEnum::ATT_DIGGING_UNIT, "AttDiggingUnit"},
    {AIPersonalityFieldsEnum::ATT_DIGGING_UNIT_MAX, "AttDiggingUnitMax"},
    {AIPersonalityFieldsEnum::ATT_UNIT_2, "AttUnit2"},
    {AIPersonalityFieldsEnum::ATT_UNIT_2_MAX, "AttUnit2Max"},
    {AIPersonalityFieldsEnum::ATT_MAX_ASSASSINS, "AttMaxAssassins"},
    {AIPersonalityFieldsEnum::ATT_MAX_LADDERMEN, "AttMaxLaddermen"},
    {AIPersonalityFieldsEnum::ATT_MAX_TUNNELERS, "AttMaxTunnelers"},
    {AIPersonalityFieldsEnum::ATT_UNIT_PATROL, "AttUnitPatrol"},
    {AIPersonalityFieldsEnum::ATT_UNIT_PATROL_MAX, "AttUnitPatrolMax"},
    {AIPersonalityFieldsEnum::ATT_UNIT_PATROL_GROUPS_COUNT, "AttUnitPatrolGroupsCount"},
    {AIPersonalityFieldsEnum::ATT_UNIT_BACKUP, "AttUnitBackup"},
    {AIPersonalityFieldsEnum::ATT_UNIT_BACKUP_MAX, "AttUnitBackupMax"},
    {AIPersonalityFieldsEnum::ATT_UNIT_BACKUP_GROUPS_COUNT, "AttUnitBackupGroupsCount"},
    {AIPersonalityFieldsEnum::ATT_UNIT_ENGAGE, "AttUnitEngage"},
    {AIPersonalityFieldsEnum::ATT_UNIT_ENGAGE_MAX, "AttUnitEngageMax"},
    {AIPersonalityFieldsEnum::ATT_UNIT_SIEGE_DEF, "AttUnitSiegeDef"},
    {AIPersonalityFieldsEnum::ATT_UNIT_SIEGE_DEF_MAX, "AttUnitSiegeDefMax"},
    {AIPersonalityFieldsEnum::ATT_UNIT_SIEGE_DEF_GROUPS_COUNT, "AttUnitSiegeDefGroupsCount"},
    {AIPersonalityFieldsEnum::ATT_UNIT_MAIN_1, "AttUnitMain1"},
    {AIPersonalityFieldsEnum::ATT_UNIT_MAIN_2, "AttUnitMain2"},
    {AIPersonalityFieldsEnum::ATT_UNIT_MAIN_3, "AttUnitMain3"},
    {AIPersonalityFieldsEnum::ATT_UNIT_MAIN_4, "AttUnitMain4"},
    {AIPersonalityFieldsEnum::ATT_MAX_DEFAULT, "AttMaxDefault"},
    {AIPersonalityFieldsEnum::ATT_MAIN_GROUPS_COUNT, "AttMainGroupsCount"},
    {AIPersonalityFieldsEnum::TARGET_CHOICE, "TargetChoice"}
  })
}

#endif //AIANDPERSONALITYENUMS