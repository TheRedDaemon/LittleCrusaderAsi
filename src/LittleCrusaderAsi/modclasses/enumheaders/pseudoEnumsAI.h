#ifndef PSEUDOENUMSAI
#define PSEUDOENUMSAI

// pseudoEnums
#include "../../modcore/enumBase.h"

namespace modclasses
{
  // NOTE: all values where taken from the Unoffical Crusader Patch (https://github.com/Sh0wdown/UnofficialCrusaderPatch)
  // so the credit for this values goes to them
  // Version of the ENUMs should be from the UCP 2.14

  // since often name, value and Id are required, and not easy accessable, re-impl with new pseudo enums
  // also need to be the even heavier ones, to allow retransform

  //* CharNames, including the ingame ids *//
  inline static constexpr char aiCharacterName[]{ "AICharacterName" };
  struct AICharacterName : PseudoEnum<aiCharacterName, int32_t, true, true, true>
  {
    inline static EnumType NONE       { CreateEnum( "NULL"       , 99 ) };
    inline static EnumType RAT        { CreateEnum( "Rat"        , 1  ) };
    inline static EnumType SNAKE      { CreateEnum( "Snake"      , 2  ) };
    inline static EnumType PIG        { CreateEnum( "Pig"        , 3  ) };
    inline static EnumType WOLF       { CreateEnum( "Wolf"       , 4  ) };
    inline static EnumType SALADIN    { CreateEnum( "Saladin"    , 5  ) };
    inline static EnumType CALIPH     { CreateEnum( "Caliph"     , 6  ) };
    inline static EnumType SULTAN     { CreateEnum( "Sultan"     , 7  ) };
    inline static EnumType RICHARD    { CreateEnum( "Richard"    , 8  ) };
    inline static EnumType FREDERICK  { CreateEnum( "Frederick"  , 9  ) };
    inline static EnumType PHILLIP    { CreateEnum( "Phillip"    , 10 ) };
    inline static EnumType WAZIR      { CreateEnum( "Wazir"      , 11 ) };
    inline static EnumType EMIR       { CreateEnum( "Emir"       , 12 ) };
    inline static EnumType NIZAR      { CreateEnum( "Nizar"      , 13 ) };
    inline static EnumType SHERIFF    { CreateEnum( "Sheriff"    , 14 ) };
    inline static EnumType MARSHAL    { CreateEnum( "Marshal"    , 15 ) };
    inline static EnumType ABBOT      { CreateEnum( "Abbot"      , 16 ) };
  };


  //* Farm-Ids, used to define which are build *//
  inline static constexpr char farmBuilding[]{ "FarmBuilding" };
  struct FarmBuilding : PseudoEnum<farmBuilding, int32_t, true, true, true>
  {
    inline static EnumType NONE         { CreateEnum( "NULL"        , 0xFF ) };
    inline static EnumType NO_FARM      { CreateEnum( "None"        , 0x00 ) };
    inline static EnumType WHEAT_FARM   { CreateEnum( "WheatFarm"   , 0x1E ) };
    inline static EnumType HOP_FARM     { CreateEnum( "HopFarm"     , 0x1F ) };
    inline static EnumType APPLE_FARM   { CreateEnum( "AppleFarm"   , 0x20 ) };
    inline static EnumType DAIRY_FARM   { CreateEnum( "DairyFarm"   , 0x21 ) };
  };


  //* Resource-Ids *//
  inline static constexpr char resource[]{ "Resource" };
  struct Resource : PseudoEnum<resource, int32_t, true, true, true>
  {
    inline static EnumType NONE           { CreateEnum( "NULL"            , 0xFF ) };
    inline static EnumType NO_RESOURCE    { CreateEnum( "None"            , 0x00 ) };
    inline static EnumType WOOD           { CreateEnum( "Wood"            , 0x02 ) };
    inline static EnumType HOP            { CreateEnum( "Hop"             , 0x03 ) };
    inline static EnumType STONE          { CreateEnum( "Stone"           , 0x04 ) };
    inline static EnumType IRON           { CreateEnum( "Iron"            , 0x06 ) };
    inline static EnumType PITCH          { CreateEnum( "Pitch"           , 0x07 ) };
    inline static EnumType WHEAT          { CreateEnum( "Wheat"           , 0x09 ) };
    inline static EnumType BREAD          { CreateEnum( "Bread"           , 0x0A ) };
    inline static EnumType CHEESE         { CreateEnum( "Cheese"          , 0x0B ) };
    inline static EnumType MEAT           { CreateEnum( "Meat"            , 0x0C ) };
    inline static EnumType APPLES         { CreateEnum( "Apples"          , 0x0D ) };
    inline static EnumType BEER           { CreateEnum( "Beer"            , 0x0E ) };
    inline static EnumType FLOUR          { CreateEnum( "Flour"           , 0x10 ) };
    inline static EnumType BOWS           { CreateEnum( "Bows"            , 0x11 ) };
    inline static EnumType CROSSBOWS      { CreateEnum( "Crossbows"       , 0x12 ) };
    inline static EnumType SPEARS         { CreateEnum( "Spears"          , 0x13 ) };
    inline static EnumType PIKES          { CreateEnum( "Pikes"           , 0x14 ) };
    inline static EnumType MACES          { CreateEnum( "Maces"           , 0x15 ) };
    inline static EnumType SWORDS         { CreateEnum( "Swords"          , 0x16 ) };
    inline static EnumType LEATHERARMORS  { CreateEnum( "LeatherArmors"   , 0x17 ) };
    inline static EnumType IRONARMORS     { CreateEnum( "IronArmors"      , 0x18 ) };
  };


  //* Siege-Engine-Ids *//
  inline static constexpr char siegeEngine[]{ "SiegeEngine" };
  struct SiegeEngine : PseudoEnum<siegeEngine, int32_t, true, true, true>
  {
    inline static EnumType NONE             { CreateEnum( "NULL"          , 0xFF ) };
    inline static EnumType NO_SIEGE_ENGINE  { CreateEnum( "None"          , 0x00 ) };
    inline static EnumType CATAPULT         { CreateEnum( "Catapult"      , 0x27 ) }; 
    inline static EnumType TREBUCHET        { CreateEnum( "Trebuchet"     , 0x28 ) }; 
    inline static EnumType MANGONEL         { CreateEnum( "Mangonel"      , 0x29 ) }; 
    inline static EnumType TENT             { CreateEnum( "Tent"          , 0x32 ) }; 
    inline static EnumType SIEGETOWER       { CreateEnum( "SiegeTower"    , 0x3A ) }; 
    inline static EnumType BATTERINGRAM     { CreateEnum( "BatteringRam"  , 0x3B ) }; 
    inline static EnumType SHIELD           { CreateEnum( "Shield"        , 0x3C ) }; 
    inline static EnumType TOWERBALLISTA    { CreateEnum( "TowerBallista" , 0x3D ) }; 
    inline static EnumType FIREBALLISTA     { CreateEnum( "FireBallista"  , 0x4D ) };
  };


  inline static constexpr char harassingSiegeEngine[]{ "HarassingSiegeEngine" };
  struct HarassingSiegeEngine : PseudoEnum<harassingSiegeEngine, int32_t, true, true, true>
  {
    inline static EnumType NONE              { CreateEnum( "NULL"          , 0xFFFF ) };
    inline static EnumType NO_SIEGE_ENGINE   { CreateEnum( "None"          , 0x00   ) };
    inline static EnumType CATAPULT          { CreateEnum( "Catapult"      , 0xBE   ) };
    inline static EnumType FIREBALLISTA      { CreateEnum( "FireBallista"  , 0x166  ) };
  };


  //* Target IDs -> what will the ai attack *//
  inline static constexpr char targetingType[]{ "TargetingType" };
  struct TargetingType : PseudoEnum<targetingType, int32_t, true, true, true>
  {
    inline static EnumType NONE        { CreateEnum( "NULL"      , 99 ) };    // wrong input
    inline static EnumType GOLD        { CreateEnum( "Gold"      , 0  ) };    // Highest gold
    inline static EnumType BALANCED    { CreateEnum( "Balanced"  , 1  ) };    // average of weakest / closest enemy
    inline static EnumType CLOSEST     { CreateEnum( "Closest"   , 2  ) };    // closest enemy
    inline static EnumType ANY         { CreateEnum( "Any"       , 3  ) };    // Nothing specified
    inline static EnumType PLAYER      { CreateEnum( "Player"    , 4  ) };    // player or if no players are left for the closest AI
  };


  //* Settings for weapon workshops *//
  inline static constexpr char fletcherSetting[]{ "FletcherSetting" };
  struct FletcherSetting : PseudoEnum<fletcherSetting, int32_t, true, true, true>
  {
    inline static EnumType NONE         { CreateEnum( "NULL"      , 0xFFFF ) }; // wrong input
    inline static EnumType BOWS         { CreateEnum( "Bows"      , 0x11   ) };
    inline static EnumType CROSSBOWS    { CreateEnum( "Crossbows" , 0x12   ) };
    inline static EnumType BOTH         { CreateEnum( "Both"      , -999   ) };
  };


  inline static constexpr char poleturnerSetting[]{ "PoleturnerSetting" };
  struct PoleturnerSetting : PseudoEnum<poleturnerSetting, int32_t, true, true, true>
  {
    inline static EnumType NONE        { CreateEnum( "NULL"     , 0xFFFF ) }; // wrong input
    inline static EnumType SPEARS      { CreateEnum( "Spears"   , 0x13   ) };
    inline static EnumType PIKES       { CreateEnum( "Pikes"    , 0x14   ) };
    inline static EnumType BOTH        { CreateEnum( "Both"     , -999   ) };
  };


  inline static constexpr char blacksmithSetting[]{ "BlacksmithSetting" };
  struct BlacksmithSetting : PseudoEnum<blacksmithSetting, int32_t, true, true, true>
  {
    inline static EnumType NONE       { CreateEnum( "NULL"      , 0xFFFF ) };  // wrong input
    inline static EnumType MACES      { CreateEnum( "Maces"     , 0x15   ) };
    inline static EnumType SWORD      { CreateEnum( "Swords"    , 0x16   ) };
    inline static EnumType BOTH       { CreateEnum( "Both"      , -999   ) };
  };


  //* Unit and npc ids *//
  inline static constexpr char unit[]{ "Unit" };
  struct Unit : PseudoEnum<unit, int32_t, true, true, true>
  {
    inline static EnumType NONE             { CreateEnum( "NULL"            , 0xFFFF ) }; // wrong input
    inline static EnumType NO_UNIT          { CreateEnum( "None"            , 0x00   ) };
    inline static EnumType TUNNELER         { CreateEnum( "Tunneler"        , 0x05   ) };
    inline static EnumType EUROPARCHER      { CreateEnum( "EuropArcher"     , 0x16   ) };
    inline static EnumType CROSSBOWMAN      { CreateEnum( "Crossbowman"     , 0x17   ) };
    inline static EnumType SPEARMAN         { CreateEnum( "Spearman"        , 0x18   ) };
    inline static EnumType PIKEMAN          { CreateEnum( "Pikeman"         , 0x19   ) };
    inline static EnumType MACEMAN          { CreateEnum( "Maceman"         , 0x1A   ) };
    inline static EnumType SWORDSMAN        { CreateEnum( "Swordsman"       , 0x1B   ) };
    inline static EnumType KNIGHT           { CreateEnum( "Knight"          , 0x1C   ) };
    inline static EnumType LADDERMAN        { CreateEnum( "Ladderman"       , 0x1D   ) };
    inline static EnumType ENGINEER         { CreateEnum( "Engineer"        , 0x1E   ) };
    inline static EnumType MONK             { CreateEnum( "Monk"            , 0x25   ) };
    inline static EnumType ARABARCHER       { CreateEnum( "ArabArcher"      , 0x46   ) };
    inline static EnumType SLAVE            { CreateEnum( "Slave"           , 0x47   ) };
    inline static EnumType SLINGER          { CreateEnum( "Slinger"         , 0x48   ) };
    inline static EnumType ASSASSIN         { CreateEnum( "Assassin"        , 0x49   ) };
    inline static EnumType HORSEARCHER      { CreateEnum( "HorseArcher"     , 0x4A   ) };
    inline static EnumType ARABSWORDSMAN    { CreateEnum( "ArabSwordsman"   , 0x4B   ) };
    inline static EnumType FIRETHROWER      { CreateEnum( "FireThrower"     , 0x4C   ) };
  };


  inline static constexpr char diggingUnit[]{ "DiggingUnit" };
  struct DiggingUnit : PseudoEnum<diggingUnit, int32_t, true, true, true>
  {
    inline static EnumType NONE              { CreateEnum( "NULL"          , 0xFFFF ) }; // wrong input
    inline static EnumType NO_DIGGING_UNIT   { CreateEnum( "None"          , 0x00   ) };
    inline static EnumType EUROPARCHER       { CreateEnum( "EuropArcher"   , 0x16   ) };
    inline static EnumType SPEARMAN          { CreateEnum( "Spearman"      , 0x18   ) };
    inline static EnumType PIKEMAN           { CreateEnum( "Pikeman"       , 0x19   ) };
    inline static EnumType MACEMAN           { CreateEnum( "Maceman"       , 0x1A   ) };
    inline static EnumType ENGINEER          { CreateEnum( "Engineer"      , 0x1E   ) };
    inline static EnumType SLAVE             { CreateEnum( "Slave"         , 0x47   ) };
  };


  inline static constexpr char meleeUnit[]{ "MeleeUnit" };
  struct MeleeUnit : PseudoEnum<meleeUnit, int32_t, true, true, true>
  {
    inline static EnumType NONE              { CreateEnum( "NULL"            , 0xFFFF ) }; // wrong input
    inline static EnumType NO_MELEE_UNIT     { CreateEnum( "None"            , 0x00   ) };
    inline static EnumType TUNNELER          { CreateEnum( "Tunneler"        , 0x05   ) };
    inline static EnumType SPEARMAN          { CreateEnum( "Spearman"        , 0x18   ) };
    inline static EnumType PIKEMAN           { CreateEnum( "Pikeman"         , 0x19   ) };
    inline static EnumType MACEMAN           { CreateEnum( "Maceman"         , 0x1A   ) };
    inline static EnumType SWORDSMAN         { CreateEnum( "Swordsman"       , 0x1B   ) };
    inline static EnumType KNIGHT            { CreateEnum( "Knight"          , 0x1C   ) };
  //inline static EnumType LADDERMAN         { CreateEnum( "Ladderman"       , 0x1D   ) };
  //inline static EnumType ENGINEER          { CreateEnum( "Engineer"        , 0x1E   ) };
    inline static EnumType MONK              { CreateEnum( "Monk"            , 0x25   ) };
    inline static EnumType SLAVE             { CreateEnum( "Slave"           , 0x47   ) };
    inline static EnumType ASSASSIN          { CreateEnum( "Assassin"        , 0x49   ) };
    inline static EnumType ARABSWORDSMAN     { CreateEnum( "ArabSwordsman"   , 0x4B   ) };
  };


  inline static constexpr char rangedUnit[]{ "RangedUnit" };
  struct RangedUnit : PseudoEnum<rangedUnit, int32_t, true, true, true>
  {
    inline static EnumType NONE              { CreateEnum( "NULL"           , 0xFFFF ) }; // wrong input
    inline static EnumType NO_RANGED_UNIT    { CreateEnum( "None"           , 0x00   ) };
    inline static EnumType EUROPARCHER       { CreateEnum( "EuropArcher"    , 0x16   ) };
    inline static EnumType CROSSBOWMAN       { CreateEnum( "Crossbowman"    , 0x17   ) };
    inline static EnumType ARABARCHER        { CreateEnum( "ArabArcher"     , 0x46   ) };
    inline static EnumType SLINGER           { CreateEnum( "Slinger"        , 0x48   ) };
    inline static EnumType HORSEARCHER       { CreateEnum( "HorseArcher"    , 0x4A   ) };
    inline static EnumType FIRETHROWER       { CreateEnum( "FireThrower"    , 0x4C   ) };
  };


  inline static constexpr char npc[]{ "NPC" };
  struct NPC : PseudoEnum<npc, int32_t, true, true, true>
  {
    inline static EnumType NONE              { CreateEnum( "NULL"               , 0x00 ) };
    inline static EnumType PEASANT           { CreateEnum( "Peasant"            , 1    ) };
    inline static EnumType BURNINGMAN        { CreateEnum( "BurningMan"         , 2    ) };
    inline static EnumType LUMBERJACK        { CreateEnum( "Lumberjack"         , 3    ) };
    inline static EnumType FLETCHER          { CreateEnum( "Fletcher"           , 4    ) };
    inline static EnumType TUNNELER          { CreateEnum( "Tunneler"           , 5    ) };
    inline static EnumType HUNTER            { CreateEnum( "Hunter"             , 6    ) };
    inline static EnumType UNKNOWN_7         { CreateEnum( "Unknown7"           , 7    ) };
    inline static EnumType UNKNOWN_8         { CreateEnum( "Unknown8"           , 8    ) };
    inline static EnumType UNKNOWN_9         { CreateEnum( "Unknown9"           , 9    ) };
    inline static EnumType PITCHWORKER       { CreateEnum( "PitchWorker"        , 10   ) };
    inline static EnumType UNKNOWN_11        { CreateEnum( "Unknown11"          , 11   ) };
    inline static EnumType UNKNOWN_12        { CreateEnum( "Unknown12"          , 12   ) };
    inline static EnumType UNKNOWN_13        { CreateEnum( "Unknown13"          , 13   ) };
    inline static EnumType UNKNOWN_14        { CreateEnum( "Unknown14"          , 14   ) };
    inline static EnumType CHILD             { CreateEnum( "Child"              , 15   ) };
    inline static EnumType BAKER             { CreateEnum( "Baker"              , 16   ) };
    inline static EnumType WOMAN             { CreateEnum( "Woman"              , 17   ) };
    inline static EnumType POLETURNER        { CreateEnum( "Poleturner"         , 18   ) };
    inline static EnumType SMITH             { CreateEnum( "Smith"              , 19   ) };
    inline static EnumType ARMORER           { CreateEnum( "Armorer"            , 20   ) };
    inline static EnumType TANNER            { CreateEnum( "Tanner"             , 21   ) };
    inline static EnumType EUROPARCHER       { CreateEnum( "EuropArcher"        , 22   ) };
    inline static EnumType CROSSBOWMAN       { CreateEnum( "CrossbowMan"        , 23   ) };
    inline static EnumType SPEARMAN          { CreateEnum( "Spearman"           , 24   ) };
    inline static EnumType PIKEMAN           { CreateEnum( "Pikeman"            , 25   ) };
    inline static EnumType MACEMAN           { CreateEnum( "Maceman"            , 26   ) };
    inline static EnumType SWORDSMAN         { CreateEnum( "Swordsman"          , 27   ) };
    inline static EnumType KNIGHT            { CreateEnum( "Knight"             , 28   ) };
    inline static EnumType LADDERMAN         { CreateEnum( "Ladderman"          , 29   ) };
    inline static EnumType ENGINEER          { CreateEnum( "Engineer"           , 30   ) };
    inline static EnumType UNKNOWN_31        { CreateEnum( "Unknown31"          , 31   ) };
    inline static EnumType UNKNOWN_32        { CreateEnum( "Unknown32"          , 32   ) };
    inline static EnumType PRIEST            { CreateEnum( "Priest"             , 33   ) };
    inline static EnumType UNKNOWN_34        { CreateEnum( "Unknown34"          , 34   ) };
    inline static EnumType DRUNKARD          { CreateEnum( "Drunkard"           , 35   ) };
    inline static EnumType INNKEEPER         { CreateEnum( "Innkeeper"          , 36   ) };
    inline static EnumType MONK              { CreateEnum( "Monk"               , 37   ) };
    inline static EnumType UNKNOWN_38        { CreateEnum( "Unknown38"          , 38   ) };
    inline static EnumType CATAPULT          { CreateEnum( "Catapult"           , 39   ) };
    inline static EnumType TREBUCHET         { CreateEnum( "Trebuchet"          , 40   ) };
    inline static EnumType MANGONEL          { CreateEnum( "Mangonel"           , 41   ) };
    inline static EnumType UNKNOWN_42        { CreateEnum( "Unknown42"          , 42   ) };
    inline static EnumType UNKNOWN_43        { CreateEnum( "Unknown43"          , 43   ) };
    inline static EnumType ANTELOPE          { CreateEnum( "Antelope"           , 44   ) };
    inline static EnumType LION              { CreateEnum( "Lion"               , 45   ) };
    inline static EnumType RABBIT            { CreateEnum( "Rabbit"             , 46   ) };
    inline static EnumType CAMEL             { CreateEnum( "Camel"              , 47   ) };
    inline static EnumType UNKNOWN_48        { CreateEnum( "Unknown48"          , 48   ) };
    inline static EnumType UNKNOWN_49        { CreateEnum( "Unknown49"          , 49   ) };
    inline static EnumType SIEGETENT         { CreateEnum( "SiegeTent"          , 50   ) };
    inline static EnumType UNKNOWN_51        { CreateEnum( "Unknown51"          , 51   ) };
    inline static EnumType UNKNOWN_52        { CreateEnum( "Unknown52"          , 52   ) };
    inline static EnumType FIREMAN           { CreateEnum( "Fireman"            , 53   ) };
    inline static EnumType GHOST             { CreateEnum( "Ghost"              , 54   ) };
    inline static EnumType LORD              { CreateEnum( "Lord"               , 55   ) };
    inline static EnumType LADY              { CreateEnum( "Lady"               , 56   ) };
    inline static EnumType JESTER            { CreateEnum( "Jester"             , 57   ) };
    inline static EnumType SIEGETOWER        { CreateEnum( "Siegetower"         , 58   ) };
    inline static EnumType RAM               { CreateEnum( "Ram"                , 59   ) };
    inline static EnumType SHIELD            { CreateEnum( "Shield"             , 60   ) };
    inline static EnumType BALLISTA          { CreateEnum( "Ballista"           , 61   ) };
    inline static EnumType CHICKEN           { CreateEnum( "Chicken"            , 62   ) };
    inline static EnumType UNKNOWN_63        { CreateEnum( "Unknown63"          , 63   ) };
    inline static EnumType UNKNOWN_64        { CreateEnum( "Unknown64"          , 64   ) };
    inline static EnumType JUGGLER           { CreateEnum( "Juggler"            , 65   ) };
    inline static EnumType FIREEATER         { CreateEnum( "FireEater"          , 66   ) };
    inline static EnumType DOG               { CreateEnum( "Dog"                , 67   ) };
    inline static EnumType UNKNOWN_68        { CreateEnum( "Unknown68"          , 68   ) };
    inline static EnumType UNKNOWN_69        { CreateEnum( "Unknown69"          , 69   ) };
    inline static EnumType ARABARCHER        { CreateEnum( "ArabArcher"         , 70   ) };
    inline static EnumType SLAVE             { CreateEnum( "Slave"              , 71   ) };
    inline static EnumType SLINGER           { CreateEnum( "Slinger"            , 72   ) };
    inline static EnumType ASSASSIN          { CreateEnum( "Assassin"           , 73   ) };
    inline static EnumType HORSEARCHER       { CreateEnum( "HorseArcher"        , 74   ) };
    inline static EnumType ARABSWORDSMAN     { CreateEnum( "ArabSwordsman"      , 75   ) };
    inline static EnumType FIRETHROWER       { CreateEnum( "FireThrower"        , 76   ) };
    inline static EnumType FIREBALLISTA      { CreateEnum( "FireBallista"       , 77   ) };                                                     
  };


  //* AI Personality Fields *//
  inline static constexpr char aIPersonalityFieldsEnum[]{ "AIPersonalityFieldsEnum" };
  struct AIPersonalityFieldsEnum : PseudoEnum<aIPersonalityFieldsEnum, int32_t, true, true, true>
  {
    inline static EnumType UNKNOWN_000                            { CreateEnum( "Unknown000"                      , 0   ) };
    inline static EnumType UNKNOWN_001                            { CreateEnum( "Unknown001"                      , 1   ) };
    inline static EnumType UNKNOWN_002                            { CreateEnum( "Unknown002"                      , 2   ) };
    inline static EnumType UNKNOWN_003                            { CreateEnum( "Unknown003"                      , 3   ) };
    inline static EnumType UNKNOWN_004                            { CreateEnum( "Unknown004"                      , 4   ) };
    inline static EnumType UNKNOWN_005                            { CreateEnum( "Unknown005"                      , 5   ) };
    inline static EnumType CRITICAL_POPULARITY                    { CreateEnum( "CriticalPopularity"              , 6   ) };
    inline static EnumType LOWEST_POPULARITY                      { CreateEnum( "LowestPopularity"                , 7   ) };
    inline static EnumType HIGHEST_POPULARITY                     { CreateEnum( "HighestPopularity"               , 8   ) };
    inline static EnumType TAXES_MIN                              { CreateEnum( "TaxesMin"                        , 9   ) };
    inline static EnumType TAXES_MAX                              { CreateEnum( "TaxesMax"                        , 10  ) };
    inline static EnumType UNKNOWN_011                            { CreateEnum( "Unknown011"                      , 11  ) };
    inline static EnumType FARM_1                                 { CreateEnum( "Farm1"                           , 12  ) };
    inline static EnumType FARM_2                                 { CreateEnum( "Farm2"                           , 13  ) };
    inline static EnumType FARM_3                                 { CreateEnum( "Farm3"                           , 14  ) };
    inline static EnumType FARM_4                                 { CreateEnum( "Farm4"                           , 15  ) };
    inline static EnumType FARM_5                                 { CreateEnum( "Farm5"                           , 16  ) };
    inline static EnumType FARM_6                                 { CreateEnum( "Farm6"                           , 17  ) };
    inline static EnumType FARM_7                                 { CreateEnum( "Farm7"                           , 18  ) };
    inline static EnumType FARM_8                                 { CreateEnum( "Farm8"                           , 19  ) };
    inline static EnumType POPULATION_PER_FARM                    { CreateEnum( "PopulationPerFarm"               , 20  ) };
    inline static EnumType POPULATION_PER_WOODCUTTER              { CreateEnum( "PopulationPerWoodcutter"         , 21  ) };
    inline static EnumType POPULATION_PER_QUARRY                  { CreateEnum( "PopulationPerQuarry"             , 22  ) };
    inline static EnumType POPULATION_PER_IRONMINE                { CreateEnum( "PopulationPerIronmine"           , 23  ) };
    inline static EnumType POPULATION_PER_PITCHRIG                { CreateEnum( "PopulationPerPitchrig"           , 24  ) };
    inline static EnumType MAX_QUARRIES                           { CreateEnum( "MaxQuarries"                     , 25  ) };
    inline static EnumType MAX_IRONMINES                          { CreateEnum( "MaxIronmines"                    , 26  ) };
    inline static EnumType MAX_WOODCUTTERS                        { CreateEnum( "MaxWoodcutters"                  , 27  ) };
    inline static EnumType MAX_PITCHRIGS                          { CreateEnum( "MaxPitchrigs"                    , 28  ) };
    inline static EnumType MAX_FARMS                              { CreateEnum( "MaxFarms"                        , 29  ) };
    inline static EnumType BUILD_INTERVAL                         { CreateEnum( "BuildInterval"                   , 30  ) };
    inline static EnumType RESOURCE_REBUILD_DELAY                 { CreateEnum( "ResourceRebuildDelay"            , 31  ) };
    inline static EnumType MAX_FOOD                               { CreateEnum( "MaxFood"                         , 32  ) };
    inline static EnumType MINIMUM_APPLES                         { CreateEnum( "MinimumApples"                   , 33  ) };
    inline static EnumType MINIMUM_CHEESE                         { CreateEnum( "MinimumCheese"                   , 34  ) };
    inline static EnumType MINIMUM_BREAD                          { CreateEnum( "MinimumBread"                    , 35  ) };
    inline static EnumType MINIMUM_WHEAT                          { CreateEnum( "MinimumWheat"                    , 36  ) };
    inline static EnumType MINIMUM_HOP                            { CreateEnum( "MinimumHop"                      , 37  ) };
    inline static EnumType TRADE_AMOUNT_FOOD                      { CreateEnum( "TradeAmountFood"                 , 38  ) };
    inline static EnumType TRADE_AMOUNT_EQUIPMENT                 { CreateEnum( "TradeAmountEquipment"            , 39  ) };
    inline static EnumType UNKNOWN_040                            { CreateEnum( "Unknown040"                      , 40  ) };
    inline static EnumType MINIMUM_GOODS_REQUIRED_AFTER_TRADE     { CreateEnum( "MinimumGoodsRequiredAfterTrade"  , 41  ) };
    inline static EnumType DOUBLE_RATIONS_FOOD_THRESHOLD          { CreateEnum( "DoubleRationsFoodThreshold"      , 42  ) };
    inline static EnumType MAX_WOOD                               { CreateEnum( "MaxWood"                         , 43  ) };
    inline static EnumType MAX_STONE                              { CreateEnum( "MaxStone"                        , 44  ) };
    inline static EnumType MAX_RESOURCE_OTHER                     { CreateEnum( "MaxResourceOther"                , 45  ) };
    inline static EnumType MAX_EQUIPMENT                          { CreateEnum( "MaxEquipment"                    , 46  ) };
    inline static EnumType MAX_BEER                               { CreateEnum( "MaxBeer"                         , 47  ) };
    inline static EnumType MAX_RESOURCE_VARIANCE                  { CreateEnum( "MaxResourceVariance"             , 48  ) };
    inline static EnumType RECRUIT_GOLD_THRESHOLD                 { CreateEnum( "RecruitGoldThreshold"            , 49  ) };
    inline static EnumType BLACKSMITH_SETTING                     { CreateEnum( "BlacksmithSetting"               , 50  ) };
    inline static EnumType FLETCHER_SETTING                       { CreateEnum( "FletcherSetting"                 , 51  ) };
    inline static EnumType POLETURNER_SETTING                     { CreateEnum( "PoleturnerSetting"               , 52  ) };
    inline static EnumType SELL_RESOURCE_01                       { CreateEnum( "SellResource01"                  , 53  ) };
    inline static EnumType SELL_RESOURCE_02                       { CreateEnum( "SellResource02"                  , 54  ) };
    inline static EnumType SELL_RESOURCE_03                       { CreateEnum( "SellResource03"                  , 55  ) };
    inline static EnumType SELL_RESOURCE_04                       { CreateEnum( "SellResource04"                  , 56  ) };
    inline static EnumType SELL_RESOURCE_05                       { CreateEnum( "SellResource05"                  , 57  ) };
    inline static EnumType SELL_RESOURCE_06                       { CreateEnum( "SellResource06"                  , 58  ) };
    inline static EnumType SELL_RESOURCE_07                       { CreateEnum( "SellResource07"                  , 59  ) };
    inline static EnumType SELL_RESOURCE_08                       { CreateEnum( "SellResource08"                  , 60  ) };
    inline static EnumType SELL_RESOURCE_09                       { CreateEnum( "SellResource09"                  , 61  ) };
    inline static EnumType SELL_RESOURCE_10                       { CreateEnum( "SellResource10"                  , 62  ) };
    inline static EnumType SELL_RESOURCE_11                       { CreateEnum( "SellResource11"                  , 63  ) };
    inline static EnumType SELL_RESOURCE_12                       { CreateEnum( "SellResource12"                  , 64  ) };
    inline static EnumType SELL_RESOURCE_13                       { CreateEnum( "SellResource13"                  , 65  ) };
    inline static EnumType SELL_RESOURCE_14                       { CreateEnum( "SellResource14"                  , 66  ) };
    inline static EnumType SELL_RESOURCE_15                       { CreateEnum( "SellResource15"                  , 67  ) };
    inline static EnumType DEF_WALLPATROL_RALLYTIME               { CreateEnum( "DefWallPatrolRallyTime"          , 68  ) };
    inline static EnumType DEF_WALLPATROL_GROUPS                  { CreateEnum( "DefWallPatrolGroups"             , 69  ) };
    inline static EnumType DEF_SIEGEENGINE_GOLD_THRESHOLD         { CreateEnum( "DefSiegeEngineGoldThreshold"     , 70  ) };
    inline static EnumType DEF_SIEGEENGINE_BUILD_DELAY            { CreateEnum( "DefSiegeEngineBuildDelay"        , 71  ) };
    inline static EnumType UNKNOWN_072                            { CreateEnum( "Unknown072"                      , 72  ) };
    inline static EnumType UNKNOWN_073                            { CreateEnum( "Unknown073"                      , 73  ) };
    inline static EnumType RECRUIT_PROB_DEF_DEFAULT               { CreateEnum( "RecruitProbDefDefault"           , 74  ) };
    inline static EnumType RECRUIT_PROB_DEF_WEAK                  { CreateEnum( "RecruitProbDefWeak"              , 75  ) };
    inline static EnumType RECRUIT_PROB_DEF_STRONG                { CreateEnum( "RecruitProbDefStrong"            , 76  ) };
    inline static EnumType RECRUIT_PROB_RAID_DEFAULT              { CreateEnum( "RecruitProbRaidDefault"          , 77  ) };
    inline static EnumType RECRUIT_PROB_RAID_WEAK                 { CreateEnum( "RecruitProbRaidWeak"             , 78  ) };
    inline static EnumType RECRUIT_PROB_RAID_STRONG               { CreateEnum( "RecruitProbRaidStrong"           , 79  ) };
    inline static EnumType RECRUIT_PROB_ATTACK_DEFAULT            { CreateEnum( "RecruitProbAttackDefault"        , 80  ) };
    inline static EnumType RECRUIT_PROB_ATTACK_WEAK               { CreateEnum( "RecruitProbAttackWeak"           , 81  ) };
    inline static EnumType RECRUIT_PROB_ATTACK_STRONG             { CreateEnum( "RecruitProbAttackStrong"         , 82  ) };
    inline static EnumType SORTIE_UNIT_RANGED_MIN                 { CreateEnum( "SortieUnitRangedMin"             , 83  ) };
    inline static EnumType SORTIE_UNIT_RANGED                     { CreateEnum( "SortieUnitRanged"                , 84  ) };
    inline static EnumType SORTIE_UNIT_MELEE_MIN                  { CreateEnum( "SortieUnitMeleeMin"              , 85  ) };
    inline static EnumType SORTIE_UNIT_MELEE                      { CreateEnum( "SortieUnitMelee"                 , 86  ) };
    inline static EnumType DEF_DIGGING_UNIT_MAX                   { CreateEnum( "DefDiggingUnitMax"               , 87  ) };
    inline static EnumType DEF_DIGGING_UNIT                       { CreateEnum( "DefDiggingUnit"                  , 88  ) };
    inline static EnumType RECRUIT_INTERVAL                       { CreateEnum( "RecruitInterval"                 , 89  ) };
    inline static EnumType RECRUIT_INTERVAL_WEAK                  { CreateEnum( "RecruitIntervalWeak"             , 90  ) };
    inline static EnumType RECRUIT_INTERVAL_STRONG                { CreateEnum( "RecruitIntervalStrong"           , 91  ) };
    inline static EnumType DEF_TOTAL                              { CreateEnum( "DefTotal"                        , 92  ) };
    inline static EnumType OUTER_PATROL_GROUPS_COUNT              { CreateEnum( "OuterPatrolGroupsCount"          , 93  ) };
    inline static EnumType OUTER_PATROL_GROUPS_MOVE               { CreateEnum( "OuterPatrolGroupsMove"           , 94  ) };
    inline static EnumType OUTER_PATROL_RALLY_DELAY               { CreateEnum( "OuterPatrolRallyDelay"           , 95  ) };
    inline static EnumType DEF_WALLS                              { CreateEnum( "DefWalls"                        , 96  ) };
    inline static EnumType DEF_UNIT_1                             { CreateEnum( "DefUnit1"                        , 97  ) };
    inline static EnumType DEF_UNIT_2                             { CreateEnum( "DefUnit2"                        , 98  ) };
    inline static EnumType DEF_UNIT_3                             { CreateEnum( "DefUnit3"                        , 99  ) };
    inline static EnumType DEF_UNIT_4                             { CreateEnum( "DefUnit4"                        , 100 ) };
    inline static EnumType DEF_UNIT_5                             { CreateEnum( "DefUnit5"                        , 101 ) };
    inline static EnumType DEF_UNIT_6                             { CreateEnum( "DefUnit6"                        , 102 ) };
    inline static EnumType DEF_UNIT_7                             { CreateEnum( "DefUnit7"                        , 103 ) };
    inline static EnumType DEF_UNIT_8                             { CreateEnum( "DefUnit8"                        , 104 ) };
    inline static EnumType RAID_UNITS_BASE                        { CreateEnum( "RaidUnitsBase"                   , 105 ) };
    inline static EnumType RAID_UNITS_RANDOM                      { CreateEnum( "RaidUnitsRandom"                 , 106 ) };
    inline static EnumType RAID_UNIT_1                            { CreateEnum( "RaidUnit1"                       , 107 ) };
    inline static EnumType RAID_UNIT_2                            { CreateEnum( "RaidUnit2"                       , 108 ) };
    inline static EnumType RAID_UNIT_3                            { CreateEnum( "RaidUnit3"                       , 109 ) };
    inline static EnumType RAID_UNIT_4                            { CreateEnum( "RaidUnit4"                       , 110 ) };
    inline static EnumType RAID_UNIT_5                            { CreateEnum( "RaidUnit5"                       , 111 ) };
    inline static EnumType RAID_UNIT_6                            { CreateEnum( "RaidUnit6"                       , 112 ) };
    inline static EnumType RAID_UNIT_7                            { CreateEnum( "RaidUnit7"                       , 113 ) };
    inline static EnumType RAID_UNIT_8                            { CreateEnum( "RaidUnit8"                       , 114 ) };
    inline static EnumType HARASSING_SIEGE_ENGINE_1               { CreateEnum( "HarassingSiegeEngine1"           , 115 ) };
    inline static EnumType HARASSING_SIEGE_ENGINE_2               { CreateEnum( "HarassingSiegeEngine2"           , 116 ) };
    inline static EnumType HARASSING_SIEGE_ENGINE_3               { CreateEnum( "HarassingSiegeEngine3"           , 117 ) };
    inline static EnumType HARASSING_SIEGE_ENGINE_4               { CreateEnum( "HarassingSiegeEngine4"           , 118 ) };
    inline static EnumType HARASSING_SIEGE_ENGINE_5               { CreateEnum( "HarassingSiegeEngine5"           , 119 ) };
    inline static EnumType HARASSING_SIEGE_ENGINE_6               { CreateEnum( "HarassingSiegeEngine6"           , 120 ) };
    inline static EnumType HARASSING_SIEGE_ENGINE_7               { CreateEnum( "HarassingSiegeEngine7"           , 121 ) };
    inline static EnumType HARASSING_SIEGE_ENGINE_8               { CreateEnum( "HarassingSiegeEngine8"           , 122 ) };
    inline static EnumType HARASSING_SIEGE_ENGINES_MAX            { CreateEnum( "HarassingSiegeEnginesMax"        , 123 ) };
    inline static EnumType UNKNOWN_124                            { CreateEnum( "Unknown124"                      , 124 ) };
    inline static EnumType ATT_FORCE_BASE                         { CreateEnum( "AttForceBase"                    , 125 ) };
    inline static EnumType ATT_FORCE_RANDOM                       { CreateEnum( "AttForceRandom"                  , 126 ) };
    inline static EnumType ATT_FORCE_SUPPORT_ALLY_THRESHOLD       { CreateEnum( "AttForceSupportAllyThreshold"    , 127 ) };
    inline static EnumType ATT_FORCE_RALLY_PERCENTAGE             { CreateEnum( "AttForceRallyPercentage"         , 128 ) };
    inline static EnumType UNKNOWN_129                            { CreateEnum( "Unknown129"                      , 129 ) };
    inline static EnumType UNKNOWN_130                            { CreateEnum( "Unknown130"                      , 130 ) };
    inline static EnumType ATT_UNIT_PATROL_RECOMMAND_DELAY        { CreateEnum( "AttUnitPatrolRecommandDelay"     , 131 ) };
    inline static EnumType UNKNOWN_132                            { CreateEnum( "Unknown132"                      , 132 ) };
    inline static EnumType SIEGE_ENGINE_1                         { CreateEnum( "SiegeEngine1"                    , 133 ) };
    inline static EnumType SIEGE_ENGINE_2                         { CreateEnum( "SiegeEngine2"                    , 134 ) };
    inline static EnumType SIEGE_ENGINE_3                         { CreateEnum( "SiegeEngine3"                    , 135 ) };
    inline static EnumType SIEGE_ENGINE_4                         { CreateEnum( "SiegeEngine4"                    , 136 ) };
    inline static EnumType SIEGE_ENGINE_5                         { CreateEnum( "SiegeEngine5"                    , 137 ) };
    inline static EnumType SIEGE_ENGINE_6                         { CreateEnum( "SiegeEngine6"                    , 138 ) };
    inline static EnumType SIEGE_ENGINE_7                         { CreateEnum( "SiegeEngine7"                    , 139 ) };
    inline static EnumType SIEGE_ENGINE_8                         { CreateEnum( "SiegeEngine8"                    , 140 ) };
    inline static EnumType COW_THROW_INTERVAL                     { CreateEnum( "CowThrowInterval"                , 141 ) };
    inline static EnumType UNKNOWN_142                            { CreateEnum( "Unknown142"                      , 142 ) };
    inline static EnumType ATT_MAX_ENGINEERS                      { CreateEnum( "AttMaxEngineers"                 , 143 ) };
    inline static EnumType ATT_DIGGING_UNIT                       { CreateEnum( "AttDiggingUnit"                  , 144 ) };
    inline static EnumType ATT_DIGGING_UNIT_MAX                   { CreateEnum( "AttDiggingUnitMax"               , 145 ) };
    inline static EnumType ATT_UNIT_2                             { CreateEnum( "AttUnit2"                        , 146 ) };
    inline static EnumType ATT_UNIT_2_MAX                         { CreateEnum( "AttUnit2Max"                     , 147 ) };
    inline static EnumType ATT_MAX_ASSASSINS                      { CreateEnum( "AttMaxAssassins"                 , 148 ) };
    inline static EnumType ATT_MAX_LADDERMEN                      { CreateEnum( "AttMaxLaddermen"                 , 149 ) };
    inline static EnumType ATT_MAX_TUNNELERS                      { CreateEnum( "AttMaxTunnelers"                 , 150 ) };
    inline static EnumType ATT_UNIT_PATROL                        { CreateEnum( "AttUnitPatrol"                   , 151 ) };
    inline static EnumType ATT_UNIT_PATROL_MAX                    { CreateEnum( "AttUnitPatrolMax"                , 152 ) };
    inline static EnumType ATT_UNIT_PATROL_GROUPS_COUNT           { CreateEnum( "AttUnitPatrolGroupsCount"        , 153 ) };
    inline static EnumType ATT_UNIT_BACKUP                        { CreateEnum( "AttUnitBackup"                   , 154 ) };
    inline static EnumType ATT_UNIT_BACKUP_MAX                    { CreateEnum( "AttUnitBackupMax"                , 155 ) };
    inline static EnumType ATT_UNIT_BACKUP_GROUPS_COUNT           { CreateEnum( "AttUnitBackupGroupsCount"        , 156 ) };
    inline static EnumType ATT_UNIT_ENGAGE                        { CreateEnum( "AttUnitEngage"                   , 157 ) };
    inline static EnumType ATT_UNIT_ENGAGE_MAX                    { CreateEnum( "AttUnitEngageMax"                , 158 ) };
    inline static EnumType ATT_UNIT_SIEGE_DEF                     { CreateEnum( "AttUnitSiegeDef"                 , 159 ) };
    inline static EnumType ATT_UNIT_SIEGE_DEF_MAX                 { CreateEnum( "AttUnitSiegeDefMax"              , 160 ) };
    inline static EnumType ATT_UNIT_SIEGE_DEF_GROUPS_COUNT        { CreateEnum( "AttUnitSiegeDefGroupsCount"      , 161 ) };
    inline static EnumType ATT_UNIT_MAIN_1                        { CreateEnum( "AttUnitMain1"                    , 162 ) };
    inline static EnumType ATT_UNIT_MAIN_2                        { CreateEnum( "AttUnitMain2"                    , 163 ) };
    inline static EnumType ATT_UNIT_MAIN_3                        { CreateEnum( "AttUnitMain3"                    , 164 ) };
    inline static EnumType ATT_UNIT_MAIN_4                        { CreateEnum( "AttUnitMain4"                    , 165 ) };
    inline static EnumType ATT_MAX_DEFAULT                        { CreateEnum( "AttMaxDefault"                   , 166 ) };
    inline static EnumType ATT_MAIN_GROUPS_COUNT                  { CreateEnum( "AttMainGroupsCount"              , 167 ) };
    inline static EnumType TARGET_CHOICE                          { CreateEnum( "TargetChoice"                    , 168 ) };
    inline static EnumType NONE                                   { CreateEnum( "NULL"                            , 999 ) };
  };

  // using stuff:
  using AIC           = AIPersonalityFieldsEnum;
  using AICEnum       = AIPersonalityFieldsEnum::EnumType;
  using AIName        = AICharacterName;
  using AINameEnum    = AICharacterName::EnumType;
  using Farm          = FarmBuilding;
  using FarmEnum      = FarmBuilding::EnumType;
  // using Resource   = ...;
  using ResourceEnum  = Resource::EnumType;
  using SE            = SiegeEngine;
  using SEEnum        = SiegeEngine::EnumType;
  using HSE           = HarassingSiegeEngine;
  using HSEEnum       = HarassingSiegeEngine::EnumType;
  using Target        = TargetingType;
  using TargetEnum    = TargetingType::EnumType;
  using Fletcher      = FletcherSetting;
  using FletcherEnum  = FletcherSetting::EnumType;
  using Pole          = PoleturnerSetting;
  using PoleEnum      = PoleturnerSetting::EnumType;
  using Smith         = BlacksmithSetting;
  using SmithEnum     = BlacksmithSetting::EnumType;
  //using Unit        = ...;
  using UnitEnum      = Unit::EnumType;
  using DUnit         = DiggingUnit;
  using DUnitEnum     = DiggingUnit::EnumType;
  using MUnit         = MeleeUnit;
  using MUnitEnum     = MeleeUnit::EnumType;
  using RUnit         = RangedUnit;
  using RUnitEnum     = RangedUnit::EnumType;
  //using NPC         = ...;
  using NPCEnum       = NPC::EnumType;
}

#endif //PSEUDOENUMSAI