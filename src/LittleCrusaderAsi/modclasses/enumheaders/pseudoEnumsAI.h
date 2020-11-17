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
    inline static EnumType None       { CreateEnum( "NULL"       , 99 ) };
    inline static EnumType Rat        { CreateEnum( "Rat"        , 1  ) };
    inline static EnumType Snake      { CreateEnum( "Snake"      , 2  ) };
    inline static EnumType Pig        { CreateEnum( "Pig"        , 3  ) };
    inline static EnumType Wolf       { CreateEnum( "Wolf"       , 4  ) };
    inline static EnumType Saladin    { CreateEnum( "Saladin"    , 5  ) };
    inline static EnumType Caliph     { CreateEnum( "Caliph"     , 6  ) };
    inline static EnumType Sultan     { CreateEnum( "Sultan"     , 7  ) };
    inline static EnumType Richard    { CreateEnum( "Richard"    , 8  ) };
    inline static EnumType Frederick  { CreateEnum( "Frederick"  , 9  ) };
    inline static EnumType Phillip    { CreateEnum( "Phillip"    , 10 ) };
    inline static EnumType Wazir      { CreateEnum( "Wazir"      , 11 ) };
    inline static EnumType Emir       { CreateEnum( "Emir"       , 12 ) };
    inline static EnumType Nizar      { CreateEnum( "Nizar"      , 13 ) };
    inline static EnumType Sheriff    { CreateEnum( "Sheriff"    , 14 ) };
    inline static EnumType Marshal    { CreateEnum( "Marshal"    , 15 ) };
    inline static EnumType Abbot      { CreateEnum( "Abbot"      , 16 ) };
  };


  //* Farm-Ids, used to define which are build *//
  inline static constexpr char farmBuilding[]{ "FarmBuilding" };
  struct FarmBuilding : PseudoEnum<farmBuilding, int32_t, true, true, true>
  {
    inline static EnumType None        { CreateEnum( "NULL"        , 0xFF ) };
    inline static EnumType NoFarm      { CreateEnum( "None"        , 0x00 ) };
    inline static EnumType WheatFarm   { CreateEnum( "WheatFarm"   , 0x1E ) };
    inline static EnumType HopFarm     { CreateEnum( "HopFarm"     , 0x1F ) };
    inline static EnumType AppleFarm   { CreateEnum( "AppleFarm"   , 0x20 ) };
    inline static EnumType DairyFarm   { CreateEnum( "DairyFarm"   , 0x21 ) };
  };


  //* Resource-Ids *//
  inline static constexpr char resource[]{ "Resource" };
  struct Resource : PseudoEnum<resource, int32_t, true, true, true>
  {
    inline static EnumType None           { CreateEnum( "NULL"            , 0xFF ) };
    inline static EnumType NoResource     { CreateEnum( "None"            , 0x00 ) };
    inline static EnumType Wood           { CreateEnum( "Wood"            , 0x02 ) };
    inline static EnumType Hop            { CreateEnum( "Hop"             , 0x03 ) };
    inline static EnumType Stone          { CreateEnum( "Stone"           , 0x04 ) };
    inline static EnumType Iron           { CreateEnum( "Iron"            , 0x06 ) };
    inline static EnumType Pitch          { CreateEnum( "Pitch"           , 0x07 ) };
    inline static EnumType Wheat          { CreateEnum( "Wheat"           , 0x09 ) };
    inline static EnumType Bread          { CreateEnum( "Bread"           , 0x0A ) };
    inline static EnumType Cheese         { CreateEnum( "Cheese"          , 0x0B ) };
    inline static EnumType Meat           { CreateEnum( "Meat"            , 0x0C ) };
    inline static EnumType Apples         { CreateEnum( "Apples"          , 0x0D ) };
    inline static EnumType Beer           { CreateEnum( "Beer"            , 0x0E ) };
    inline static EnumType Flour          { CreateEnum( "Flour"           , 0x10 ) };
    inline static EnumType Bows           { CreateEnum( "Bows"            , 0x11 ) };
    inline static EnumType Crossbows      { CreateEnum( "Crossbows"       , 0x12 ) };
    inline static EnumType Spears         { CreateEnum( "Spears"          , 0x13 ) };
    inline static EnumType Pikes          { CreateEnum( "Pikes"           , 0x14 ) };
    inline static EnumType Maces          { CreateEnum( "Maces"           , 0x15 ) };
    inline static EnumType Swords         { CreateEnum( "Swords"          , 0x16 ) };
    inline static EnumType LeatherArmors  { CreateEnum( "LeatherArmors"   , 0x17 ) };
    inline static EnumType IronArmors     { CreateEnum( "IronArmors"      , 0x18 ) };
  };


  //* Siege-Engine-Ids *//
  inline static constexpr char siegeEngine[]{ "SiegeEngine" };
  struct SiegeEngine : PseudoEnum<siegeEngine, int32_t, true, true, true>
  {
    inline static EnumType None             { CreateEnum( "NULL"          , 0xFF ) };
    inline static EnumType NoSiegeEngine    { CreateEnum( "None"          , 0x00 ) };
    inline static EnumType Catapult         { CreateEnum( "Catapult"      , 0x27 ) }; 
    inline static EnumType Trebuchet        { CreateEnum( "Trebuchet"     , 0x28 ) }; 
    inline static EnumType Mangonel         { CreateEnum( "Mangonel"      , 0x29 ) }; 
    inline static EnumType Tent             { CreateEnum( "Tent"          , 0x32 ) }; 
    inline static EnumType SiegeTower       { CreateEnum( "SiegeTower"    , 0x3A ) }; 
    inline static EnumType BatteringRam     { CreateEnum( "BatteringRam"  , 0x3B ) }; 
    inline static EnumType Shield           { CreateEnum( "Shield"        , 0x3C ) }; 
    inline static EnumType TowerBallista    { CreateEnum( "TowerBallista" , 0x3D ) }; 
    inline static EnumType FireBallista     { CreateEnum( "FireBallista"  , 0x4D ) };
  };


  inline static constexpr char harassingSiegeEngine[]{ "HarassingSiegeEngine" };
  struct HarassingSiegeEngine : PseudoEnum<harassingSiegeEngine, int32_t, true, true, true>
  {
    inline static EnumType None              { CreateEnum( "NULL"          , 0xFFFF ) };
    inline static EnumType NoSiegeEngine     { CreateEnum( "None"          , 0x00   ) };
    inline static EnumType Catapult          { CreateEnum( "Catapult"      , 0xBE   ) };
    inline static EnumType FireBallista      { CreateEnum( "FireBallista"  , 0x166  ) };
  };


  //* Target IDs -> what will the ai attack *//
  inline static constexpr char targetingType[]{ "TargetingType" };
  struct TargetingType : PseudoEnum<targetingType, int32_t, true, true, true>
  {
    inline static EnumType None        { CreateEnum( "NULL"      , 99 ) };    // wrong input
    inline static EnumType Gold        { CreateEnum( "Gold"      , 0  ) };    // Highest gold
    inline static EnumType Balanced    { CreateEnum( "Balanced"  , 1  ) };    // average of weakest / closest enemy
    inline static EnumType Closest     { CreateEnum( "Closest"   , 2  ) };    // closest enemy
    inline static EnumType Any         { CreateEnum( "Any"       , 3  ) };    // Nothing specified
    inline static EnumType Player      { CreateEnum( "Player"    , 4  ) };    // player or if no players are left for the closest AI
  };


  //* Settings for weapon workshops *//
  inline static constexpr char fletcherSetting[]{ "FletcherSetting" };
  struct FletcherSetting : PseudoEnum<fletcherSetting, int32_t, true, true, true>
  {
    inline static EnumType None         { CreateEnum( "NULL"      , 0xFFFF ) }; // wrong input
    inline static EnumType Bows         { CreateEnum( "Bows"      , 0x11   ) };
    inline static EnumType Crossbows    { CreateEnum( "Crossbows" , 0x12   ) };
    inline static EnumType Both         { CreateEnum( "Both"      , -999   ) };
  };


  inline static constexpr char poleturnerSetting[]{ "PoleturnerSetting" };
  struct PoleturnerSetting : PseudoEnum<poleturnerSetting, int32_t, true, true, true>
  {
    inline static EnumType None        { CreateEnum( "NULL"     , 0xFFFF ) }; // wrong input
    inline static EnumType Spears      { CreateEnum( "Spears"   , 0x13   ) };
    inline static EnumType Pikes       { CreateEnum( "Pikes"    , 0x14   ) };
    inline static EnumType Both        { CreateEnum( "Both"     , -999   ) };
  };


  inline static constexpr char blacksmithSetting[]{ "BlacksmithSetting" };
  struct BlacksmithSetting : PseudoEnum<blacksmithSetting, int32_t, true, true, true>
  {
    inline static EnumType None       { CreateEnum( "NULL"      , 0xFFFF ) };  // wrong input
    inline static EnumType Maces      { CreateEnum( "Maces"     , 0x15   ) };
    inline static EnumType Sword      { CreateEnum( "Swords"    , 0x16   ) };
    inline static EnumType Both       { CreateEnum( "Both"      , -999   ) };
  };


  //* Unit and npc ids *//
  inline static constexpr char unit[]{ "Unit" };
  struct Unit : PseudoEnum<unit, int32_t, true, true, true>
  {
    inline static EnumType None             { CreateEnum( "NULL"            , 0xFFFF ) }; // wrong input
    inline static EnumType NoUnit           { CreateEnum( "None"            , 0x00   ) };
    inline static EnumType Tunneler         { CreateEnum( "Tunneler"        , 0x05   ) };
    inline static EnumType EuropArcher      { CreateEnum( "EuropArcher"     , 0x16   ) };
    inline static EnumType Crossbowman      { CreateEnum( "Crossbowman"     , 0x17   ) };
    inline static EnumType Spearman         { CreateEnum( "Spearman"        , 0x18   ) };
    inline static EnumType Pikeman          { CreateEnum( "Pikeman"         , 0x19   ) };
    inline static EnumType Maceman          { CreateEnum( "Maceman"         , 0x1A   ) };
    inline static EnumType Swordsman        { CreateEnum( "Swordsman"       , 0x1B   ) };
    inline static EnumType Knight           { CreateEnum( "Knight"          , 0x1C   ) };
    inline static EnumType Ladderman        { CreateEnum( "Ladderman"       , 0x1D   ) };
    inline static EnumType Engineer         { CreateEnum( "Engineer"        , 0x1E   ) };
    inline static EnumType Monk             { CreateEnum( "Monk"            , 0x25   ) };
    inline static EnumType ArabArcher       { CreateEnum( "ArabArcher"      , 0x46   ) };
    inline static EnumType Slave            { CreateEnum( "Slave"           , 0x47   ) };
    inline static EnumType Slinger          { CreateEnum( "Slinger"         , 0x48   ) };
    inline static EnumType Assassin         { CreateEnum( "Assassin"        , 0x49   ) };
    inline static EnumType HorseArcher      { CreateEnum( "HorseArcher"     , 0x4A   ) };
    inline static EnumType ArabSwordsman    { CreateEnum( "ArabSwordsman"   , 0x4B   ) };
    inline static EnumType FireThrower      { CreateEnum( "FireThrower"     , 0x4C   ) };
  };


  inline static constexpr char diggingUnit[]{ "DiggingUnit" };
  struct DiggingUnit : PseudoEnum<diggingUnit, int32_t, true, true, true>
  {
    inline static EnumType None              { CreateEnum( "NULL"          , 0xFFFF ) }; // wrong input
    inline static EnumType NoDiggingUnit     { CreateEnum( "None"          , 0x00   ) };
    inline static EnumType EuropArcher       { CreateEnum( "EuropArcher"   , 0x16   ) };
    inline static EnumType Spearman          { CreateEnum( "Spearman"      , 0x18   ) };
    inline static EnumType Pikeman           { CreateEnum( "Pikeman"       , 0x19   ) };
    inline static EnumType Maceman           { CreateEnum( "Maceman"       , 0x1A   ) };
    inline static EnumType Engineer          { CreateEnum( "Engineer"      , 0x1E   ) };
    inline static EnumType Slave             { CreateEnum( "Slave"         , 0x47   ) };
  };


  inline static constexpr char meleeUnit[]{ "MeleeUnit" };
  struct MeleeUnit : PseudoEnum<meleeUnit, int32_t, true, true, true>
  {
    inline static EnumType None              { CreateEnum( "NULL"            , 0xFFFF ) }; // wrong input
    inline static EnumType NoMeleeUnit       { CreateEnum( "None"            , 0x00   ) };
    inline static EnumType Tunneler          { CreateEnum( "Tunneler"        , 0x05   ) };
    inline static EnumType Spearman          { CreateEnum( "Spearman"        , 0x18   ) };
    inline static EnumType Pikeman           { CreateEnum( "Pikeman"         , 0x19   ) };
    inline static EnumType Maceman           { CreateEnum( "Maceman"         , 0x1A   ) };
    inline static EnumType Swordsman         { CreateEnum( "Swordsman"       , 0x1B   ) };
    inline static EnumType Knight            { CreateEnum( "Knight"          , 0x1C   ) };
  //inline static EnumType Ladderman         { CreateEnum( "Ladderman"       , 0x1D   ) };
  //inline static EnumType Engineer          { CreateEnum( "Engineer"        , 0x1E   ) };
    inline static EnumType Monk              { CreateEnum( "Monk"            , 0x25   ) };
    inline static EnumType Slave             { CreateEnum( "Slave"           , 0x47   ) };
    inline static EnumType Assassin          { CreateEnum( "Assassin"        , 0x49   ) };
    inline static EnumType ArabSwordsman     { CreateEnum( "ArabSwordsman"   , 0x4B   ) };
  };


  inline static constexpr char rangedUnit[]{ "RangedUnit" };
  struct RangedUnit : PseudoEnum<rangedUnit, int32_t, true, true, true>
  {
    inline static EnumType None              { CreateEnum( "NULL"           , 0xFFFF ) }; // wrong input
    inline static EnumType NoRangedUnit      { CreateEnum( "None"           , 0x00   ) };
    inline static EnumType EuropArcher       { CreateEnum( "EuropArcher"    , 0x16   ) };
    inline static EnumType Crossbowman       { CreateEnum( "Crossbowman"    , 0x17   ) };
    inline static EnumType ArabArcher        { CreateEnum( "ArabArcher"     , 0x46   ) };
    inline static EnumType Slinger           { CreateEnum( "Slinger"        , 0x48   ) };
    inline static EnumType HorseArcher       { CreateEnum( "HorseArcher"    , 0x4A   ) };
    inline static EnumType FireThrower       { CreateEnum( "FireThrower"    , 0x4C   ) };
  };


  inline static constexpr char npc[]{ "NPC" };
  struct NPC : PseudoEnum<npc, int32_t, true, true, true>
  {
    inline static EnumType None              { CreateEnum( "NULL"               , 0x00 ) };
    inline static EnumType Peasant           { CreateEnum( "Peasant"            , 1    ) };
    inline static EnumType BurningMan        { CreateEnum( "BurningMan"         , 2    ) };
    inline static EnumType Lumberjack        { CreateEnum( "Lumberjack"         , 3    ) };
    inline static EnumType Fletcher          { CreateEnum( "Fletcher"           , 4    ) };
    inline static EnumType Tunneler          { CreateEnum( "Tunneler"           , 5    ) };
    inline static EnumType Hunter            { CreateEnum( "Hunter"             , 6    ) };
    inline static EnumType Unknown7          { CreateEnum( "Unknown7"           , 7    ) };
    inline static EnumType Unknown8          { CreateEnum( "Unknown8"           , 8    ) };
    inline static EnumType Unknown9          { CreateEnum( "Unknown9"           , 9    ) };
    inline static EnumType PitchWorker       { CreateEnum( "PitchWorker"        , 10   ) };
    inline static EnumType Unknown11         { CreateEnum( "Unknown11"          , 11   ) };
    inline static EnumType Unknown12         { CreateEnum( "Unknown12"          , 12   ) };
    inline static EnumType Unknown13         { CreateEnum( "Unknown13"          , 13   ) };
    inline static EnumType Unknown14         { CreateEnum( "Unknown14"          , 14   ) };
    inline static EnumType Child             { CreateEnum( "Child"              , 15   ) };
    inline static EnumType Baker             { CreateEnum( "Baker"              , 16   ) };
    inline static EnumType Woman             { CreateEnum( "Woman"              , 17   ) };
    inline static EnumType Poleturner        { CreateEnum( "Poleturner"         , 18   ) };
    inline static EnumType Smith             { CreateEnum( "Smith"              , 19   ) };
    inline static EnumType Armorer           { CreateEnum( "Armorer"            , 20   ) };
    inline static EnumType Tanner            { CreateEnum( "Tanner"             , 21   ) };
    inline static EnumType EuropArcher       { CreateEnum( "EuropArcher"        , 22   ) };
    inline static EnumType Crossbowman       { CreateEnum( "CrossbowMan"        , 23   ) };
    inline static EnumType Spearman          { CreateEnum( "Spearman"           , 24   ) };
    inline static EnumType Pikeman           { CreateEnum( "Pikeman"            , 25   ) };
    inline static EnumType Maceman           { CreateEnum( "Maceman"            , 26   ) };
    inline static EnumType Swordsman         { CreateEnum( "Swordsman"          , 27   ) };
    inline static EnumType Knight            { CreateEnum( "Knight"             , 28   ) };
    inline static EnumType Ladderman         { CreateEnum( "Ladderman"          , 29   ) };
    inline static EnumType Engineer          { CreateEnum( "Engineer"           , 30   ) };
    inline static EnumType Unknown31         { CreateEnum( "Unknown31"          , 31   ) };
    inline static EnumType Unknown32         { CreateEnum( "Unknown32"          , 32   ) };
    inline static EnumType Priest            { CreateEnum( "Priest"             , 33   ) };
    inline static EnumType Unknown34         { CreateEnum( "Unknown34"          , 34   ) };
    inline static EnumType Drunkard          { CreateEnum( "Drunkard"           , 35   ) };
    inline static EnumType Innkeeper         { CreateEnum( "Innkeeper"          , 36   ) };
    inline static EnumType Monk              { CreateEnum( "Monk"               , 37   ) };
    inline static EnumType Unknown38         { CreateEnum( "Unknown38"          , 38   ) };
    inline static EnumType Catapult          { CreateEnum( "Catapult"           , 39   ) };
    inline static EnumType Trebuchet         { CreateEnum( "Trebuchet"          , 40   ) };
    inline static EnumType Mangonel          { CreateEnum( "Mangonel"           , 41   ) };
    inline static EnumType Unknown42         { CreateEnum( "Unknown42"          , 42   ) };
    inline static EnumType Unknown43         { CreateEnum( "Unknown43"          , 43   ) };
    inline static EnumType Antelope          { CreateEnum( "Antelope"           , 44   ) };
    inline static EnumType Lion              { CreateEnum( "Lion"               , 45   ) };
    inline static EnumType Rabbit            { CreateEnum( "Rabbit"             , 46   ) };
    inline static EnumType Camel             { CreateEnum( "Camel"              , 47   ) };
    inline static EnumType Unknown48         { CreateEnum( "Unknown48"          , 48   ) };
    inline static EnumType Unknown49         { CreateEnum( "Unknown49"          , 49   ) };
    inline static EnumType SiegeTent         { CreateEnum( "SiegeTent"          , 50   ) };
    inline static EnumType Unknown51         { CreateEnum( "Unknown51"          , 51   ) };
    inline static EnumType Unknown52         { CreateEnum( "Unknown52"          , 52   ) };
    inline static EnumType Fireman           { CreateEnum( "Fireman"            , 53   ) };
    inline static EnumType Ghost             { CreateEnum( "Ghost"              , 54   ) };
    inline static EnumType Lord              { CreateEnum( "Lord"               , 55   ) };
    inline static EnumType Lady              { CreateEnum( "Lady"               , 56   ) };
    inline static EnumType Jester            { CreateEnum( "Jester"             , 57   ) };
    inline static EnumType Siegetower        { CreateEnum( "Siegetower"         , 58   ) };
    inline static EnumType Ram               { CreateEnum( "Ram"                , 59   ) };
    inline static EnumType Shield            { CreateEnum( "Shield"             , 60   ) };
    inline static EnumType Ballista          { CreateEnum( "Ballista"           , 61   ) };
    inline static EnumType Chicken           { CreateEnum( "Chicken"            , 62   ) };
    inline static EnumType Unknown63         { CreateEnum( "Unknown63"          , 63   ) };
    inline static EnumType Unknown64         { CreateEnum( "Unknown64"          , 64   ) };
    inline static EnumType Juggler           { CreateEnum( "Juggler"            , 65   ) };
    inline static EnumType FireEater         { CreateEnum( "FireEater"          , 66   ) };
    inline static EnumType Dog               { CreateEnum( "Dog"                , 67   ) };
    inline static EnumType Unknown68         { CreateEnum( "Unknown68"          , 68   ) };
    inline static EnumType Unknown69         { CreateEnum( "Unknown69"          , 69   ) };
    inline static EnumType ArabArcher        { CreateEnum( "ArabArcher"         , 70   ) };
    inline static EnumType Slave             { CreateEnum( "Slave"              , 71   ) };
    inline static EnumType Slinger           { CreateEnum( "Slinger"            , 72   ) };
    inline static EnumType Assassin          { CreateEnum( "Assassin"           , 73   ) };
    inline static EnumType HorseArcher       { CreateEnum( "HorseArcher"        , 74   ) };
    inline static EnumType ArabSwordsman     { CreateEnum( "ArabSwordsman"      , 75   ) };
    inline static EnumType FireThrower       { CreateEnum( "FireThrower"        , 76   ) };
    inline static EnumType FireBallista      { CreateEnum( "FireBallista"       , 77   ) };                                                     
  };


  //* AI Personality Fields *//
  inline static constexpr char aIPersonalityFieldsEnum[]{ "AIPersonalityFieldsEnum" };
  struct AIPersonalityFieldsEnum : PseudoEnum<aIPersonalityFieldsEnum, int32_t, true, true, true>
  {
    inline static EnumType Unknown000                             { CreateEnum( "Unknown000"                      , 0   ) };
    inline static EnumType Unknown001                             { CreateEnum( "Unknown001"                      , 1   ) };
    inline static EnumType Unknown002                             { CreateEnum( "Unknown002"                      , 2   ) };
    inline static EnumType Unknown003                             { CreateEnum( "Unknown003"                      , 3   ) };
    inline static EnumType Unknown004                             { CreateEnum( "Unknown004"                      , 4   ) };
    inline static EnumType Unknown005                             { CreateEnum( "Unknown005"                      , 5   ) };
    inline static EnumType CriticalPopularity                     { CreateEnum( "CriticalPopularity"              , 6   ) };
    inline static EnumType LowestPopularity                       { CreateEnum( "LowestPopularity"                , 7   ) };
    inline static EnumType HighestPopularity                      { CreateEnum( "HighestPopularity"               , 8   ) };
    inline static EnumType TaxesMin                               { CreateEnum( "TaxesMin"                        , 9   ) };
    inline static EnumType TaxesMax                               { CreateEnum( "TaxesMax"                        , 10  ) };
    inline static EnumType Unknown011                             { CreateEnum( "Unknown011"                      , 11  ) };
    inline static EnumType Farm1                                  { CreateEnum( "Farm1"                           , 12  ) };
    inline static EnumType Farm2                                  { CreateEnum( "Farm2"                           , 13  ) };
    inline static EnumType Farm3                                  { CreateEnum( "Farm3"                           , 14  ) };
    inline static EnumType Farm4                                  { CreateEnum( "Farm4"                           , 15  ) };
    inline static EnumType Farm5                                  { CreateEnum( "Farm5"                           , 16  ) };
    inline static EnumType Farm6                                  { CreateEnum( "Farm6"                           , 17  ) };
    inline static EnumType Farm7                                  { CreateEnum( "Farm7"                           , 18  ) };
    inline static EnumType Farm8                                  { CreateEnum( "Farm8"                           , 19  ) };
    inline static EnumType PopulationPerFarm                      { CreateEnum( "PopulationPerFarm"               , 20  ) };
    inline static EnumType PopulationPerWoodcutter                { CreateEnum( "PopulationPerWoodcutter"         , 21  ) };
    inline static EnumType PopulationPerQuarry                    { CreateEnum( "PopulationPerQuarry"             , 22  ) };
    inline static EnumType PopulationPerIronmine                  { CreateEnum( "PopulationPerIronmine"           , 23  ) };
    inline static EnumType PopulationPerPitchrig                  { CreateEnum( "PopulationPerPitchrig"           , 24  ) };
    inline static EnumType MaxQuarries                            { CreateEnum( "MaxQuarries"                     , 25  ) };
    inline static EnumType MaxIronmines                           { CreateEnum( "MaxIronmines"                    , 26  ) };
    inline static EnumType MaxWoodcutters                         { CreateEnum( "MaxWoodcutters"                  , 27  ) };
    inline static EnumType MaxPitchrigs                           { CreateEnum( "MaxPitchrigs"                    , 28  ) };
    inline static EnumType MaxFarms                               { CreateEnum( "MaxFarms"                        , 29  ) };
    inline static EnumType BuildInterval                          { CreateEnum( "BuildInterval"                   , 30  ) };
    inline static EnumType ResourceRebuildDelay                   { CreateEnum( "ResourceRebuildDelay"            , 31  ) };
    inline static EnumType MaxFood                                { CreateEnum( "MaxFood"                         , 32  ) };
    inline static EnumType MinimumApples                          { CreateEnum( "MinimumApples"                   , 33  ) };
    inline static EnumType MinimumCheese                          { CreateEnum( "MinimumCheese"                   , 34  ) };
    inline static EnumType MinimumBread                           { CreateEnum( "MinimumBread"                    , 35  ) };
    inline static EnumType MinimumWheat                           { CreateEnum( "MinimumWheat"                    , 36  ) };
    inline static EnumType MinimumHop                             { CreateEnum( "MinimumHop"                      , 37  ) };
    inline static EnumType TradeAmountFood                        { CreateEnum( "TradeAmountFood"                 , 38  ) };
    inline static EnumType TradeAmountEquipment                   { CreateEnum( "TradeAmountEquipment"            , 39  ) };
    inline static EnumType Unknown040                             { CreateEnum( "Unknown040"                      , 40  ) };
    inline static EnumType MinimumGoodsRequiredAfterTrade         { CreateEnum( "MinimumGoodsRequiredAfterTrade"  , 41  ) };
    inline static EnumType DoubleRationsFoodThreshold             { CreateEnum( "DoubleRationsFoodThreshold"      , 42  ) };
    inline static EnumType MaxWood                                { CreateEnum( "MaxWood"                         , 43  ) };
    inline static EnumType MaxStone                               { CreateEnum( "MaxStone"                        , 44  ) };
    inline static EnumType MaxResourceOther                       { CreateEnum( "MaxResourceOther"                , 45  ) };
    inline static EnumType MaxEquipment                           { CreateEnum( "MaxEquipment"                    , 46  ) };
    inline static EnumType MaxBeer                                { CreateEnum( "MaxBeer"                         , 47  ) };
    inline static EnumType MaxResourceVariance                    { CreateEnum( "MaxResourceVariance"             , 48  ) };
    inline static EnumType RecruitGoldThreshold                   { CreateEnum( "RecruitGoldThreshold"            , 49  ) };
    inline static EnumType BlacksmithSetting                      { CreateEnum( "BlacksmithSetting"               , 50  ) };
    inline static EnumType FletcherSetting                        { CreateEnum( "FletcherSetting"                 , 51  ) };
    inline static EnumType PoleturnerSetting                      { CreateEnum( "PoleturnerSetting"               , 52  ) };
    inline static EnumType SellResource01                         { CreateEnum( "SellResource01"                  , 53  ) };
    inline static EnumType SellResource02                         { CreateEnum( "SellResource02"                  , 54  ) };
    inline static EnumType SellResource03                         { CreateEnum( "SellResource03"                  , 55  ) };
    inline static EnumType SellResource04                         { CreateEnum( "SellResource04"                  , 56  ) };
    inline static EnumType SellResource05                         { CreateEnum( "SellResource05"                  , 57  ) };
    inline static EnumType SellResource06                         { CreateEnum( "SellResource06"                  , 58  ) };
    inline static EnumType SellResource07                         { CreateEnum( "SellResource07"                  , 59  ) };
    inline static EnumType SellResource08                         { CreateEnum( "SellResource08"                  , 60  ) };
    inline static EnumType SellResource09                         { CreateEnum( "SellResource09"                  , 61  ) };
    inline static EnumType SellResource10                         { CreateEnum( "SellResource10"                  , 62  ) };
    inline static EnumType SellResource11                         { CreateEnum( "SellResource11"                  , 63  ) };
    inline static EnumType SellResource12                         { CreateEnum( "SellResource12"                  , 64  ) };
    inline static EnumType SellResource13                         { CreateEnum( "SellResource13"                  , 65  ) };
    inline static EnumType SellResource14                         { CreateEnum( "SellResource14"                  , 66  ) };
    inline static EnumType SellResource15                         { CreateEnum( "SellResource15"                  , 67  ) };
    inline static EnumType DefWallPatrolRallytime                 { CreateEnum( "DefWallPatrolRallyTime"          , 68  ) };
    inline static EnumType DefWallPatrolGroups                    { CreateEnum( "DefWallPatrolGroups"             , 69  ) };
    inline static EnumType DefSiegeEngineGoldThreshold            { CreateEnum( "DefSiegeEngineGoldThreshold"     , 70  ) };
    inline static EnumType DefSiegeEngineBuildDelay               { CreateEnum( "DefSiegeEngineBuildDelay"        , 71  ) };
    inline static EnumType Unknown072                             { CreateEnum( "Unknown072"                      , 72  ) };
    inline static EnumType Unknown073                             { CreateEnum( "Unknown073"                      , 73  ) };
    inline static EnumType RecruitProbDefDefault                  { CreateEnum( "RecruitProbDefDefault"           , 74  ) };
    inline static EnumType RecruitProbDefWeak                     { CreateEnum( "RecruitProbDefWeak"              , 75  ) };
    inline static EnumType RecruitProbDefStrong                   { CreateEnum( "RecruitProbDefStrong"            , 76  ) };
    inline static EnumType RecruitProbRaidDefault                 { CreateEnum( "RecruitProbRaidDefault"          , 77  ) };
    inline static EnumType RecruitProbRaidWeak                    { CreateEnum( "RecruitProbRaidWeak"             , 78  ) };
    inline static EnumType RecruitProbRaidStrong                  { CreateEnum( "RecruitProbRaidStrong"           , 79  ) };
    inline static EnumType RecruitProbAttackDefault               { CreateEnum( "RecruitProbAttackDefault"        , 80  ) };
    inline static EnumType RecruitProbAttackWeak                  { CreateEnum( "RecruitProbAttackWeak"           , 81  ) };
    inline static EnumType RecruitProbAttackStrong                { CreateEnum( "RecruitProbAttackStrong"         , 82  ) };
    inline static EnumType SortieUnitRangedMin                    { CreateEnum( "SortieUnitRangedMin"             , 83  ) };
    inline static EnumType SortieUnitRanged                       { CreateEnum( "SortieUnitRanged"                , 84  ) };
    inline static EnumType SortieUnitMeleeMin                     { CreateEnum( "SortieUnitMeleeMin"              , 85  ) };
    inline static EnumType SortieUnitMelee                        { CreateEnum( "SortieUnitMelee"                 , 86  ) };
    inline static EnumType DefDiggingUnitMax                      { CreateEnum( "DefDiggingUnitMax"               , 87  ) };
    inline static EnumType DefDiggingUnit                         { CreateEnum( "DefDiggingUnit"                  , 88  ) };
    inline static EnumType RecruitInterval                        { CreateEnum( "RecruitInterval"                 , 89  ) };
    inline static EnumType RecruitIntervalWeak                    { CreateEnum( "RecruitIntervalWeak"             , 90  ) };
    inline static EnumType RecruitIntervalStrong                  { CreateEnum( "RecruitIntervalStrong"           , 91  ) };
    inline static EnumType DefTotal                               { CreateEnum( "DefTotal"                        , 92  ) };
    inline static EnumType OuterPatrolGroupsCount                 { CreateEnum( "OuterPatrolGroupsCount"          , 93  ) };
    inline static EnumType OuterPatrolGroupsMove                  { CreateEnum( "OuterPatrolGroupsMove"           , 94  ) };
    inline static EnumType OuterPatrolRallyDelay                  { CreateEnum( "OuterPatrolRallyDelay"           , 95  ) };
    inline static EnumType DefWalls                               { CreateEnum( "DefWalls"                        , 96  ) };
    inline static EnumType DefUnit1                               { CreateEnum( "DefUnit1"                        , 97  ) };
    inline static EnumType DefUnit2                               { CreateEnum( "DefUnit2"                        , 98  ) };
    inline static EnumType DefUnit3                               { CreateEnum( "DefUnit3"                        , 99  ) };
    inline static EnumType DefUnit4                               { CreateEnum( "DefUnit4"                        , 100 ) };
    inline static EnumType DefUnit5                               { CreateEnum( "DefUnit5"                        , 101 ) };
    inline static EnumType DefUnit6                               { CreateEnum( "DefUnit6"                        , 102 ) };
    inline static EnumType DefUnit7                               { CreateEnum( "DefUnit7"                        , 103 ) };
    inline static EnumType DefUnit8                               { CreateEnum( "DefUnit8"                        , 104 ) };
    inline static EnumType RaidUnitsBase                          { CreateEnum( "RaidUnitsBase"                   , 105 ) };
    inline static EnumType RaidUnitsRandom                        { CreateEnum( "RaidUnitsRandom"                 , 106 ) };
    inline static EnumType RaidUnit1                              { CreateEnum( "RaidUnit1"                       , 107 ) };
    inline static EnumType RaidUnit2                              { CreateEnum( "RaidUnit2"                       , 108 ) };
    inline static EnumType RaidUnit3                              { CreateEnum( "RaidUnit3"                       , 109 ) };
    inline static EnumType RaidUnit4                              { CreateEnum( "RaidUnit4"                       , 110 ) };
    inline static EnumType RaidUnit5                              { CreateEnum( "RaidUnit5"                       , 111 ) };
    inline static EnumType RaidUnit6                              { CreateEnum( "RaidUnit6"                       , 112 ) };
    inline static EnumType RaidUnit7                              { CreateEnum( "RaidUnit7"                       , 113 ) };
    inline static EnumType RaidUnit8                              { CreateEnum( "RaidUnit8"                       , 114 ) };
    inline static EnumType HarassingSiegeEngine1                  { CreateEnum( "HarassingSiegeEngine1"           , 115 ) };
    inline static EnumType HarassingSiegeEngine2                  { CreateEnum( "HarassingSiegeEngine2"           , 116 ) };
    inline static EnumType HarassingSiegeEngine3                  { CreateEnum( "HarassingSiegeEngine3"           , 117 ) };
    inline static EnumType HarassingSiegeEngine4                  { CreateEnum( "HarassingSiegeEngine4"           , 118 ) };
    inline static EnumType HarassingSiegeEngine5                  { CreateEnum( "HarassingSiegeEngine5"           , 119 ) };
    inline static EnumType HarassingSiegeEngine6                  { CreateEnum( "HarassingSiegeEngine6"           , 120 ) };
    inline static EnumType HarassingSiegeEngine7                  { CreateEnum( "HarassingSiegeEngine7"           , 121 ) };
    inline static EnumType HarassingSiegeEngine8                  { CreateEnum( "HarassingSiegeEngine8"           , 122 ) };
    inline static EnumType HarassingSiegeEnginesMax               { CreateEnum( "HarassingSiegeEnginesMax"        , 123 ) };
    inline static EnumType Unknown124                             { CreateEnum( "Unknown124"                      , 124 ) };
    inline static EnumType AttForceBase                           { CreateEnum( "AttForceBase"                    , 125 ) };
    inline static EnumType AttForceRandom                         { CreateEnum( "AttForceRandom"                  , 126 ) };
    inline static EnumType AttForceSupportAllyThreshold           { CreateEnum( "AttForceSupportAllyThreshold"    , 127 ) };
    inline static EnumType AttForceRallyPercentage                { CreateEnum( "AttForceRallyPercentage"         , 128 ) };
    inline static EnumType Unknown129                             { CreateEnum( "Unknown129"                      , 129 ) };
    inline static EnumType Unknown130                             { CreateEnum( "Unknown130"                      , 130 ) };
    inline static EnumType AttUnitPatrolRecommandDelay            { CreateEnum( "AttUnitPatrolRecommandDelay"     , 131 ) };
    inline static EnumType Unknown132                             { CreateEnum( "Unknown132"                      , 132 ) };
    inline static EnumType SiegeEngine1                           { CreateEnum( "SiegeEngine1"                    , 133 ) };
    inline static EnumType SiegeEngine2                           { CreateEnum( "SiegeEngine2"                    , 134 ) };
    inline static EnumType SiegeEngine3                           { CreateEnum( "SiegeEngine3"                    , 135 ) };
    inline static EnumType SiegeEngine4                           { CreateEnum( "SiegeEngine4"                    , 136 ) };
    inline static EnumType SiegeEngine5                           { CreateEnum( "SiegeEngine5"                    , 137 ) };
    inline static EnumType SiegeEngine6                           { CreateEnum( "SiegeEngine6"                    , 138 ) };
    inline static EnumType SiegeEngine7                           { CreateEnum( "SiegeEngine7"                    , 139 ) };
    inline static EnumType SiegeEngine8                           { CreateEnum( "SiegeEngine8"                    , 140 ) };
    inline static EnumType CowThrowInterval                       { CreateEnum( "CowThrowInterval"                , 141 ) };
    inline static EnumType Unknown142                             { CreateEnum( "Unknown142"                      , 142 ) };
    inline static EnumType AttMaxEngineers                        { CreateEnum( "AttMaxEngineers"                 , 143 ) };
    inline static EnumType AttDiggingUnit                         { CreateEnum( "AttDiggingUnit"                  , 144 ) };
    inline static EnumType AttDiggingUnitMax                      { CreateEnum( "AttDiggingUnitMax"               , 145 ) };
    inline static EnumType AttUnit2                               { CreateEnum( "AttUnit2"                        , 146 ) };
    inline static EnumType AttUnit2Max                            { CreateEnum( "AttUnit2Max"                     , 147 ) };
    inline static EnumType AttMaxAssassins                        { CreateEnum( "AttMaxAssassins"                 , 148 ) };
    inline static EnumType AttMaxLaddermen                        { CreateEnum( "AttMaxLaddermen"                 , 149 ) };
    inline static EnumType AttMaxTunnelers                        { CreateEnum( "AttMaxTunnelers"                 , 150 ) };
    inline static EnumType AttUnitPatrol                          { CreateEnum( "AttUnitPatrol"                   , 151 ) };
    inline static EnumType AttUnitPatrolMax                       { CreateEnum( "AttUnitPatrolMax"                , 152 ) };
    inline static EnumType AttUnitPatrolGroupsCount               { CreateEnum( "AttUnitPatrolGroupsCount"        , 153 ) };
    inline static EnumType AttUnitBackup                          { CreateEnum( "AttUnitBackup"                   , 154 ) };
    inline static EnumType AttUnitBackupMax                       { CreateEnum( "AttUnitBackupMax"                , 155 ) };
    inline static EnumType AttUnitBackupGroupsCount               { CreateEnum( "AttUnitBackupGroupsCount"        , 156 ) };
    inline static EnumType AttUnitEngage                          { CreateEnum( "AttUnitEngage"                   , 157 ) };
    inline static EnumType AttUnitEngageMax                       { CreateEnum( "AttUnitEngageMax"                , 158 ) };
    inline static EnumType AttUnitSiegeDef                        { CreateEnum( "AttUnitSiegeDef"                 , 159 ) };
    inline static EnumType AttUnitSiegeDefMax                     { CreateEnum( "AttUnitSiegeDefMax"              , 160 ) };
    inline static EnumType AttUnitSiegeDefGroupsCount             { CreateEnum( "AttUnitSiegeDefGroupsCount"      , 161 ) };
    inline static EnumType AttUnitMain1                           { CreateEnum( "AttUnitMain1"                    , 162 ) };
    inline static EnumType AttUnitMain2                           { CreateEnum( "AttUnitMain2"                    , 163 ) };
    inline static EnumType AttUnitMain3                           { CreateEnum( "AttUnitMain3"                    , 164 ) };
    inline static EnumType AttUnitMain4                           { CreateEnum( "AttUnitMain4"                    , 165 ) };
    inline static EnumType AttMaxDefault                          { CreateEnum( "AttMaxDefault"                   , 166 ) };
    inline static EnumType AttMainGroupsCount                     { CreateEnum( "AttMainGroupsCount"              , 167 ) };
    inline static EnumType TargetChoice                           { CreateEnum( "TargetChoice"                    , 168 ) };
    inline static EnumType None                                   { CreateEnum( "NULL"                            , 999 ) };
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

  // Melee and Ranged Unit are unused currently
  // -> they share the ids though, so the question is more, would other units work
  // -> so the question is, would the game still work if I send ladderman in the main attack force?
  //   -> maybe apply that restriction later (keep digging unit though, others will not work (or?))
}

#endif //PSEUDOENUMSAI