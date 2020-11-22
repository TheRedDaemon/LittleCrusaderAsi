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
    inline static EnumKeeper None       { CreateEnum( "NULL"       , 99 ) };
    inline static EnumKeeper Rat        { CreateEnum( "Rat"        , 1  ) };
    inline static EnumKeeper Snake      { CreateEnum( "Snake"      , 2  ) };
    inline static EnumKeeper Pig        { CreateEnum( "Pig"        , 3  ) };
    inline static EnumKeeper Wolf       { CreateEnum( "Wolf"       , 4  ) };
    inline static EnumKeeper Saladin    { CreateEnum( "Saladin"    , 5  ) };
    inline static EnumKeeper Caliph     { CreateEnum( "Caliph"     , 6  ) };
    inline static EnumKeeper Sultan     { CreateEnum( "Sultan"     , 7  ) };
    inline static EnumKeeper Richard    { CreateEnum( "Richard"    , 8  ) };
    inline static EnumKeeper Frederick  { CreateEnum( "Frederick"  , 9  ) };
    inline static EnumKeeper Phillip    { CreateEnum( "Phillip"    , 10 ) };
    inline static EnumKeeper Wazir      { CreateEnum( "Wazir"      , 11 ) };
    inline static EnumKeeper Emir       { CreateEnum( "Emir"       , 12 ) };
    inline static EnumKeeper Nizar      { CreateEnum( "Nizar"      , 13 ) };
    inline static EnumKeeper Sheriff    { CreateEnum( "Sheriff"    , 14 ) };
    inline static EnumKeeper Marshal    { CreateEnum( "Marshal"    , 15 ) };
    inline static EnumKeeper Abbot      { CreateEnum( "Abbot"      , 16 ) };
  };


  //* Farm-Ids, used to define which are build *//
  inline static constexpr char farmBuilding[]{ "FarmBuilding" };
  struct FarmBuilding : PseudoEnum<farmBuilding, int32_t, true, true, true>
  {
    inline static EnumKeeper None        { CreateEnum( "NULL"        , 0xFF ) };
    inline static EnumKeeper NoFarm      { CreateEnum( "None"        , 0x00 ) };
    inline static EnumKeeper WheatFarm   { CreateEnum( "WheatFarm"   , 0x1E ) };
    inline static EnumKeeper HopFarm     { CreateEnum( "HopFarm"     , 0x1F ) };
    inline static EnumKeeper AppleFarm   { CreateEnum( "AppleFarm"   , 0x20 ) };
    inline static EnumKeeper DairyFarm   { CreateEnum( "DairyFarm"   , 0x21 ) };
  };


  //* Resource-Ids *//
  inline static constexpr char resource[]{ "Resource" };
  struct Resource : PseudoEnum<resource, int32_t, true, true, true>
  {
    inline static EnumKeeper None           { CreateEnum( "NULL"            , 0xFF ) };
    inline static EnumKeeper NoResource     { CreateEnum( "None"            , 0x00 ) };
    inline static EnumKeeper Wood           { CreateEnum( "Wood"            , 0x02 ) };
    inline static EnumKeeper Hop            { CreateEnum( "Hop"             , 0x03 ) };
    inline static EnumKeeper Stone          { CreateEnum( "Stone"           , 0x04 ) };
    inline static EnumKeeper Iron           { CreateEnum( "Iron"            , 0x06 ) };
    inline static EnumKeeper Pitch          { CreateEnum( "Pitch"           , 0x07 ) };
    inline static EnumKeeper Wheat          { CreateEnum( "Wheat"           , 0x09 ) };
    inline static EnumKeeper Bread          { CreateEnum( "Bread"           , 0x0A ) };
    inline static EnumKeeper Cheese         { CreateEnum( "Cheese"          , 0x0B ) };
    inline static EnumKeeper Meat           { CreateEnum( "Meat"            , 0x0C ) };
    inline static EnumKeeper Apples         { CreateEnum( "Apples"          , 0x0D ) };
    inline static EnumKeeper Beer           { CreateEnum( "Beer"            , 0x0E ) };
    inline static EnumKeeper Flour          { CreateEnum( "Flour"           , 0x10 ) };
    inline static EnumKeeper Bows           { CreateEnum( "Bows"            , 0x11 ) };
    inline static EnumKeeper Crossbows      { CreateEnum( "Crossbows"       , 0x12 ) };
    inline static EnumKeeper Spears         { CreateEnum( "Spears"          , 0x13 ) };
    inline static EnumKeeper Pikes          { CreateEnum( "Pikes"           , 0x14 ) };
    inline static EnumKeeper Maces          { CreateEnum( "Maces"           , 0x15 ) };
    inline static EnumKeeper Swords         { CreateEnum( "Swords"          , 0x16 ) };
    inline static EnumKeeper LeatherArmors  { CreateEnum( "LeatherArmors"   , 0x17 ) };
    inline static EnumKeeper IronArmors     { CreateEnum( "IronArmors"      , 0x18 ) };
  };


  //* Siege-Engine-Ids *//
  inline static constexpr char siegeEngine[]{ "SiegeEngine" };
  struct SiegeEngine : PseudoEnum<siegeEngine, int32_t, true, true, true>
  {
    inline static EnumKeeper None             { CreateEnum( "NULL"          , 0xFF ) };
    inline static EnumKeeper NoSiegeEngine    { CreateEnum( "None"          , 0x00 ) };
    inline static EnumKeeper Catapult         { CreateEnum( "Catapult"      , 0x27 ) }; 
    inline static EnumKeeper Trebuchet        { CreateEnum( "Trebuchet"     , 0x28 ) }; 
    inline static EnumKeeper Mangonel         { CreateEnum( "Mangonel"      , 0x29 ) }; 
    inline static EnumKeeper Tent             { CreateEnum( "Tent"          , 0x32 ) }; 
    inline static EnumKeeper SiegeTower       { CreateEnum( "SiegeTower"    , 0x3A ) }; 
    inline static EnumKeeper BatteringRam     { CreateEnum( "BatteringRam"  , 0x3B ) }; 
    inline static EnumKeeper Shield           { CreateEnum( "Shield"        , 0x3C ) }; 
    inline static EnumKeeper TowerBallista    { CreateEnum( "TowerBallista" , 0x3D ) }; 
    inline static EnumKeeper FireBallista     { CreateEnum( "FireBallista"  , 0x4D ) };
  };


  inline static constexpr char harassingSiegeEngine[]{ "HarassingSiegeEngine" };
  struct HarassingSiegeEngine : PseudoEnum<harassingSiegeEngine, int32_t, true, true, true>
  {
    inline static EnumKeeper None              { CreateEnum( "NULL"          , 0xFFFF ) };
    inline static EnumKeeper NoSiegeEngine     { CreateEnum( "None"          , 0x00   ) };
    inline static EnumKeeper Catapult          { CreateEnum( "Catapult"      , 0xBE   ) };
    inline static EnumKeeper FireBallista      { CreateEnum( "FireBallista"  , 0x166  ) };
  };


  //* Target IDs -> what will the ai attack *//
  inline static constexpr char targetingType[]{ "TargetingType" };
  struct TargetingType : PseudoEnum<targetingType, int32_t, true, true, true>
  {
    inline static EnumKeeper None        { CreateEnum( "NULL"      , 99 ) };    // wrong input
    inline static EnumKeeper Gold        { CreateEnum( "Gold"      , 0  ) };    // Highest gold
    inline static EnumKeeper Balanced    { CreateEnum( "Balanced"  , 1  ) };    // average of weakest / closest enemy
    inline static EnumKeeper Closest     { CreateEnum( "Closest"   , 2  ) };    // closest enemy
    inline static EnumKeeper Any         { CreateEnum( "Any"       , 3  ) };    // Nothing specified
    inline static EnumKeeper Player      { CreateEnum( "Player"    , 4  ) };    // player or if no players are left for the closest AI
  };


  //* Settings for weapon workshops *//
  inline static constexpr char fletcherSetting[]{ "FletcherSetting" };
  struct FletcherSetting : PseudoEnum<fletcherSetting, int32_t, true, true, true>
  {
    inline static EnumKeeper None         { CreateEnum( "NULL"      , 0xFFFF ) }; // wrong input
    inline static EnumKeeper Bows         { CreateEnum( "Bows"      , 0x11   ) };
    inline static EnumKeeper Crossbows    { CreateEnum( "Crossbows" , 0x12   ) };
    inline static EnumKeeper Both         { CreateEnum( "Both"      , -999   ) };
  };


  inline static constexpr char poleturnerSetting[]{ "PoleturnerSetting" };
  struct PoleturnerSetting : PseudoEnum<poleturnerSetting, int32_t, true, true, true>
  {
    inline static EnumKeeper None        { CreateEnum( "NULL"     , 0xFFFF ) }; // wrong input
    inline static EnumKeeper Spears      { CreateEnum( "Spears"   , 0x13   ) };
    inline static EnumKeeper Pikes       { CreateEnum( "Pikes"    , 0x14   ) };
    inline static EnumKeeper Both        { CreateEnum( "Both"     , -999   ) };
  };


  inline static constexpr char blacksmithSetting[]{ "BlacksmithSetting" };
  struct BlacksmithSetting : PseudoEnum<blacksmithSetting, int32_t, true, true, true>
  {
    inline static EnumKeeper None       { CreateEnum( "NULL"      , 0xFFFF ) };  // wrong input
    inline static EnumKeeper Maces      { CreateEnum( "Maces"     , 0x15   ) };
    inline static EnumKeeper Sword      { CreateEnum( "Swords"    , 0x16   ) };
    inline static EnumKeeper Both       { CreateEnum( "Both"      , -999   ) };
  };


  //* Unit and npc ids *//
  inline static constexpr char unit[]{ "Unit" };
  struct Unit : PseudoEnum<unit, int32_t, true, true, true>
  {
    inline static EnumKeeper None             { CreateEnum( "NULL"            , 0xFFFF ) }; // wrong input
    inline static EnumKeeper NoUnit           { CreateEnum( "None"            , 0x00   ) };
    inline static EnumKeeper Tunneler         { CreateEnum( "Tunneler"        , 0x05   ) };
    inline static EnumKeeper EuropArcher      { CreateEnum( "EuropArcher"     , 0x16   ) };
    inline static EnumKeeper Crossbowman      { CreateEnum( "Crossbowman"     , 0x17   ) };
    inline static EnumKeeper Spearman         { CreateEnum( "Spearman"        , 0x18   ) };
    inline static EnumKeeper Pikeman          { CreateEnum( "Pikeman"         , 0x19   ) };
    inline static EnumKeeper Maceman          { CreateEnum( "Maceman"         , 0x1A   ) };
    inline static EnumKeeper Swordsman        { CreateEnum( "Swordsman"       , 0x1B   ) };
    inline static EnumKeeper Knight           { CreateEnum( "Knight"          , 0x1C   ) };
    inline static EnumKeeper Ladderman        { CreateEnum( "Ladderman"       , 0x1D   ) };
    inline static EnumKeeper Engineer         { CreateEnum( "Engineer"        , 0x1E   ) };
    inline static EnumKeeper Monk             { CreateEnum( "Monk"            , 0x25   ) };
    inline static EnumKeeper ArabArcher       { CreateEnum( "ArabArcher"      , 0x46   ) };
    inline static EnumKeeper Slave            { CreateEnum( "Slave"           , 0x47   ) };
    inline static EnumKeeper Slinger          { CreateEnum( "Slinger"         , 0x48   ) };
    inline static EnumKeeper Assassin         { CreateEnum( "Assassin"        , 0x49   ) };
    inline static EnumKeeper HorseArcher      { CreateEnum( "HorseArcher"     , 0x4A   ) };
    inline static EnumKeeper ArabSwordsman    { CreateEnum( "ArabSwordsman"   , 0x4B   ) };
    inline static EnumKeeper FireThrower      { CreateEnum( "FireThrower"     , 0x4C   ) };
  };


  inline static constexpr char diggingUnit[]{ "DiggingUnit" };
  struct DiggingUnit : PseudoEnum<diggingUnit, int32_t, true, true, true>
  {
    inline static EnumKeeper None              { CreateEnum( "NULL"          , 0xFFFF ) }; // wrong input
    inline static EnumKeeper NoDiggingUnit     { CreateEnum( "None"          , 0x00   ) };
    inline static EnumKeeper EuropArcher       { CreateEnum( "EuropArcher"   , 0x16   ) };
    inline static EnumKeeper Spearman          { CreateEnum( "Spearman"      , 0x18   ) };
    inline static EnumKeeper Pikeman           { CreateEnum( "Pikeman"       , 0x19   ) };
    inline static EnumKeeper Maceman           { CreateEnum( "Maceman"       , 0x1A   ) };
    inline static EnumKeeper Engineer          { CreateEnum( "Engineer"      , 0x1E   ) };
    inline static EnumKeeper Slave             { CreateEnum( "Slave"         , 0x47   ) };
  };


  inline static constexpr char meleeUnit[]{ "MeleeUnit" };
  struct MeleeUnit : PseudoEnum<meleeUnit, int32_t, true, true, true>
  {
    inline static EnumKeeper None              { CreateEnum( "NULL"            , 0xFFFF ) }; // wrong input
    inline static EnumKeeper NoMeleeUnit       { CreateEnum( "None"            , 0x00   ) };
    inline static EnumKeeper Tunneler          { CreateEnum( "Tunneler"        , 0x05   ) };
    inline static EnumKeeper Spearman          { CreateEnum( "Spearman"        , 0x18   ) };
    inline static EnumKeeper Pikeman           { CreateEnum( "Pikeman"         , 0x19   ) };
    inline static EnumKeeper Maceman           { CreateEnum( "Maceman"         , 0x1A   ) };
    inline static EnumKeeper Swordsman         { CreateEnum( "Swordsman"       , 0x1B   ) };
    inline static EnumKeeper Knight            { CreateEnum( "Knight"          , 0x1C   ) };
  //inline static EnumKeeper Ladderman         { CreateEnum( "Ladderman"       , 0x1D   ) };
  //inline static EnumKeeper Engineer          { CreateEnum( "Engineer"        , 0x1E   ) };
    inline static EnumKeeper Monk              { CreateEnum( "Monk"            , 0x25   ) };
    inline static EnumKeeper Slave             { CreateEnum( "Slave"           , 0x47   ) };
    inline static EnumKeeper Assassin          { CreateEnum( "Assassin"        , 0x49   ) };
    inline static EnumKeeper ArabSwordsman     { CreateEnum( "ArabSwordsman"   , 0x4B   ) };
  };


  inline static constexpr char rangedUnit[]{ "RangedUnit" };
  struct RangedUnit : PseudoEnum<rangedUnit, int32_t, true, true, true>
  {
    inline static EnumKeeper None              { CreateEnum( "NULL"           , 0xFFFF ) }; // wrong input
    inline static EnumKeeper NoRangedUnit      { CreateEnum( "None"           , 0x00   ) };
    inline static EnumKeeper EuropArcher       { CreateEnum( "EuropArcher"    , 0x16   ) };
    inline static EnumKeeper Crossbowman       { CreateEnum( "Crossbowman"    , 0x17   ) };
    inline static EnumKeeper ArabArcher        { CreateEnum( "ArabArcher"     , 0x46   ) };
    inline static EnumKeeper Slinger           { CreateEnum( "Slinger"        , 0x48   ) };
    inline static EnumKeeper HorseArcher       { CreateEnum( "HorseArcher"    , 0x4A   ) };
    inline static EnumKeeper FireThrower       { CreateEnum( "FireThrower"    , 0x4C   ) };
  };


  inline static constexpr char npc[]{ "NPC" };
  struct NPC : PseudoEnum<npc, int32_t, true, true, true>
  {
    inline static EnumKeeper None              { CreateEnum( "NULL"               , 0x00 ) };
    inline static EnumKeeper Peasant           { CreateEnum( "Peasant"            , 1    ) };
    inline static EnumKeeper BurningMan        { CreateEnum( "BurningMan"         , 2    ) };
    inline static EnumKeeper Lumberjack        { CreateEnum( "Lumberjack"         , 3    ) };
    inline static EnumKeeper Fletcher          { CreateEnum( "Fletcher"           , 4    ) };
    inline static EnumKeeper Tunneler          { CreateEnum( "Tunneler"           , 5    ) };
    inline static EnumKeeper Hunter            { CreateEnum( "Hunter"             , 6    ) };
    inline static EnumKeeper Unknown7          { CreateEnum( "Unknown7"           , 7    ) };
    inline static EnumKeeper Unknown8          { CreateEnum( "Unknown8"           , 8    ) };
    inline static EnumKeeper Unknown9          { CreateEnum( "Unknown9"           , 9    ) };
    inline static EnumKeeper PitchWorker       { CreateEnum( "PitchWorker"        , 10   ) };
    inline static EnumKeeper Unknown11         { CreateEnum( "Unknown11"          , 11   ) };
    inline static EnumKeeper Unknown12         { CreateEnum( "Unknown12"          , 12   ) };
    inline static EnumKeeper Unknown13         { CreateEnum( "Unknown13"          , 13   ) };
    inline static EnumKeeper Unknown14         { CreateEnum( "Unknown14"          , 14   ) };
    inline static EnumKeeper Child             { CreateEnum( "Child"              , 15   ) };
    inline static EnumKeeper Baker             { CreateEnum( "Baker"              , 16   ) };
    inline static EnumKeeper Woman             { CreateEnum( "Woman"              , 17   ) };
    inline static EnumKeeper Poleturner        { CreateEnum( "Poleturner"         , 18   ) };
    inline static EnumKeeper Smith             { CreateEnum( "Smith"              , 19   ) };
    inline static EnumKeeper Armorer           { CreateEnum( "Armorer"            , 20   ) };
    inline static EnumKeeper Tanner            { CreateEnum( "Tanner"             , 21   ) };
    inline static EnumKeeper EuropArcher       { CreateEnum( "EuropArcher"        , 22   ) };
    inline static EnumKeeper Crossbowman       { CreateEnum( "CrossbowMan"        , 23   ) };
    inline static EnumKeeper Spearman          { CreateEnum( "Spearman"           , 24   ) };
    inline static EnumKeeper Pikeman           { CreateEnum( "Pikeman"            , 25   ) };
    inline static EnumKeeper Maceman           { CreateEnum( "Maceman"            , 26   ) };
    inline static EnumKeeper Swordsman         { CreateEnum( "Swordsman"          , 27   ) };
    inline static EnumKeeper Knight            { CreateEnum( "Knight"             , 28   ) };
    inline static EnumKeeper Ladderman         { CreateEnum( "Ladderman"          , 29   ) };
    inline static EnumKeeper Engineer          { CreateEnum( "Engineer"           , 30   ) };
    inline static EnumKeeper Unknown31         { CreateEnum( "Unknown31"          , 31   ) };
    inline static EnumKeeper Unknown32         { CreateEnum( "Unknown32"          , 32   ) };
    inline static EnumKeeper Priest            { CreateEnum( "Priest"             , 33   ) };
    inline static EnumKeeper Unknown34         { CreateEnum( "Unknown34"          , 34   ) };
    inline static EnumKeeper Drunkard          { CreateEnum( "Drunkard"           , 35   ) };
    inline static EnumKeeper Innkeeper         { CreateEnum( "Innkeeper"          , 36   ) };
    inline static EnumKeeper Monk              { CreateEnum( "Monk"               , 37   ) };
    inline static EnumKeeper Unknown38         { CreateEnum( "Unknown38"          , 38   ) };
    inline static EnumKeeper Catapult          { CreateEnum( "Catapult"           , 39   ) };
    inline static EnumKeeper Trebuchet         { CreateEnum( "Trebuchet"          , 40   ) };
    inline static EnumKeeper Mangonel          { CreateEnum( "Mangonel"           , 41   ) };
    inline static EnumKeeper Unknown42         { CreateEnum( "Unknown42"          , 42   ) };
    inline static EnumKeeper Unknown43         { CreateEnum( "Unknown43"          , 43   ) };
    inline static EnumKeeper Antelope          { CreateEnum( "Antelope"           , 44   ) };
    inline static EnumKeeper Lion              { CreateEnum( "Lion"               , 45   ) };
    inline static EnumKeeper Rabbit            { CreateEnum( "Rabbit"             , 46   ) };
    inline static EnumKeeper Camel             { CreateEnum( "Camel"              , 47   ) };
    inline static EnumKeeper Unknown48         { CreateEnum( "Unknown48"          , 48   ) };
    inline static EnumKeeper Unknown49         { CreateEnum( "Unknown49"          , 49   ) };
    inline static EnumKeeper SiegeTent         { CreateEnum( "SiegeTent"          , 50   ) };
    inline static EnumKeeper Unknown51         { CreateEnum( "Unknown51"          , 51   ) };
    inline static EnumKeeper Unknown52         { CreateEnum( "Unknown52"          , 52   ) };
    inline static EnumKeeper Fireman           { CreateEnum( "Fireman"            , 53   ) };
    inline static EnumKeeper Ghost             { CreateEnum( "Ghost"              , 54   ) };
    inline static EnumKeeper Lord              { CreateEnum( "Lord"               , 55   ) };
    inline static EnumKeeper Lady              { CreateEnum( "Lady"               , 56   ) };
    inline static EnumKeeper Jester            { CreateEnum( "Jester"             , 57   ) };
    inline static EnumKeeper Siegetower        { CreateEnum( "Siegetower"         , 58   ) };
    inline static EnumKeeper Ram               { CreateEnum( "Ram"                , 59   ) };
    inline static EnumKeeper Shield            { CreateEnum( "Shield"             , 60   ) };
    inline static EnumKeeper Ballista          { CreateEnum( "Ballista"           , 61   ) };
    inline static EnumKeeper Chicken           { CreateEnum( "Chicken"            , 62   ) };
    inline static EnumKeeper Unknown63         { CreateEnum( "Unknown63"          , 63   ) };
    inline static EnumKeeper Unknown64         { CreateEnum( "Unknown64"          , 64   ) };
    inline static EnumKeeper Juggler           { CreateEnum( "Juggler"            , 65   ) };
    inline static EnumKeeper FireEater         { CreateEnum( "FireEater"          , 66   ) };
    inline static EnumKeeper Dog               { CreateEnum( "Dog"                , 67   ) };
    inline static EnumKeeper Unknown68         { CreateEnum( "Unknown68"          , 68   ) };
    inline static EnumKeeper Unknown69         { CreateEnum( "Unknown69"          , 69   ) };
    inline static EnumKeeper ArabArcher        { CreateEnum( "ArabArcher"         , 70   ) };
    inline static EnumKeeper Slave             { CreateEnum( "Slave"              , 71   ) };
    inline static EnumKeeper Slinger           { CreateEnum( "Slinger"            , 72   ) };
    inline static EnumKeeper Assassin          { CreateEnum( "Assassin"           , 73   ) };
    inline static EnumKeeper HorseArcher       { CreateEnum( "HorseArcher"        , 74   ) };
    inline static EnumKeeper ArabSwordsman     { CreateEnum( "ArabSwordsman"      , 75   ) };
    inline static EnumKeeper FireThrower       { CreateEnum( "FireThrower"        , 76   ) };
    inline static EnumKeeper FireBallista      { CreateEnum( "FireBallista"       , 77   ) };                                                     
  };


  //* AI Personality Fields *//
  inline static constexpr char aIPersonalityFieldsEnum[]{ "AIPersonalityFieldsEnum" };
  struct AIPersonalityFieldsEnum : PseudoEnum<aIPersonalityFieldsEnum, int32_t, true, true, true>
  {
    inline static EnumKeeper Unknown000                             { CreateEnum( "Unknown000"                      , 0   ) };
    inline static EnumKeeper Unknown001                             { CreateEnum( "Unknown001"                      , 1   ) };
    inline static EnumKeeper Unknown002                             { CreateEnum( "Unknown002"                      , 2   ) };
    inline static EnumKeeper Unknown003                             { CreateEnum( "Unknown003"                      , 3   ) };
    inline static EnumKeeper Unknown004                             { CreateEnum( "Unknown004"                      , 4   ) };
    inline static EnumKeeper Unknown005                             { CreateEnum( "Unknown005"                      , 5   ) };
    inline static EnumKeeper CriticalPopularity                     { CreateEnum( "CriticalPopularity"              , 6   ) };
    inline static EnumKeeper LowestPopularity                       { CreateEnum( "LowestPopularity"                , 7   ) };
    inline static EnumKeeper HighestPopularity                      { CreateEnum( "HighestPopularity"               , 8   ) };
    inline static EnumKeeper TaxesMin                               { CreateEnum( "TaxesMin"                        , 9   ) };
    inline static EnumKeeper TaxesMax                               { CreateEnum( "TaxesMax"                        , 10  ) };
    inline static EnumKeeper Unknown011                             { CreateEnum( "Unknown011"                      , 11  ) };
    inline static EnumKeeper Farm1                                  { CreateEnum( "Farm1"                           , 12  ) };
    inline static EnumKeeper Farm2                                  { CreateEnum( "Farm2"                           , 13  ) };
    inline static EnumKeeper Farm3                                  { CreateEnum( "Farm3"                           , 14  ) };
    inline static EnumKeeper Farm4                                  { CreateEnum( "Farm4"                           , 15  ) };
    inline static EnumKeeper Farm5                                  { CreateEnum( "Farm5"                           , 16  ) };
    inline static EnumKeeper Farm6                                  { CreateEnum( "Farm6"                           , 17  ) };
    inline static EnumKeeper Farm7                                  { CreateEnum( "Farm7"                           , 18  ) };
    inline static EnumKeeper Farm8                                  { CreateEnum( "Farm8"                           , 19  ) };
    inline static EnumKeeper PopulationPerFarm                      { CreateEnum( "PopulationPerFarm"               , 20  ) };
    inline static EnumKeeper PopulationPerWoodcutter                { CreateEnum( "PopulationPerWoodcutter"         , 21  ) };
    inline static EnumKeeper PopulationPerQuarry                    { CreateEnum( "PopulationPerQuarry"             , 22  ) };
    inline static EnumKeeper PopulationPerIronmine                  { CreateEnum( "PopulationPerIronmine"           , 23  ) };
    inline static EnumKeeper PopulationPerPitchrig                  { CreateEnum( "PopulationPerPitchrig"           , 24  ) };
    inline static EnumKeeper MaxQuarries                            { CreateEnum( "MaxQuarries"                     , 25  ) };
    inline static EnumKeeper MaxIronmines                           { CreateEnum( "MaxIronmines"                    , 26  ) };
    inline static EnumKeeper MaxWoodcutters                         { CreateEnum( "MaxWoodcutters"                  , 27  ) };
    inline static EnumKeeper MaxPitchrigs                           { CreateEnum( "MaxPitchrigs"                    , 28  ) };
    inline static EnumKeeper MaxFarms                               { CreateEnum( "MaxFarms"                        , 29  ) };
    inline static EnumKeeper BuildInterval                          { CreateEnum( "BuildInterval"                   , 30  ) };
    inline static EnumKeeper ResourceRebuildDelay                   { CreateEnum( "ResourceRebuildDelay"            , 31  ) };
    inline static EnumKeeper MaxFood                                { CreateEnum( "MaxFood"                         , 32  ) };
    inline static EnumKeeper MinimumApples                          { CreateEnum( "MinimumApples"                   , 33  ) };
    inline static EnumKeeper MinimumCheese                          { CreateEnum( "MinimumCheese"                   , 34  ) };
    inline static EnumKeeper MinimumBread                           { CreateEnum( "MinimumBread"                    , 35  ) };
    inline static EnumKeeper MinimumWheat                           { CreateEnum( "MinimumWheat"                    , 36  ) };
    inline static EnumKeeper MinimumHop                             { CreateEnum( "MinimumHop"                      , 37  ) };
    inline static EnumKeeper TradeAmountFood                        { CreateEnum( "TradeAmountFood"                 , 38  ) };
    inline static EnumKeeper TradeAmountEquipment                   { CreateEnum( "TradeAmountEquipment"            , 39  ) };
    inline static EnumKeeper Unknown040                             { CreateEnum( "Unknown040"                      , 40  ) };
    inline static EnumKeeper MinimumGoodsRequiredAfterTrade         { CreateEnum( "MinimumGoodsRequiredAfterTrade"  , 41  ) };
    inline static EnumKeeper DoubleRationsFoodThreshold             { CreateEnum( "DoubleRationsFoodThreshold"      , 42  ) };
    inline static EnumKeeper MaxWood                                { CreateEnum( "MaxWood"                         , 43  ) };
    inline static EnumKeeper MaxStone                               { CreateEnum( "MaxStone"                        , 44  ) };
    inline static EnumKeeper MaxResourceOther                       { CreateEnum( "MaxResourceOther"                , 45  ) };
    inline static EnumKeeper MaxEquipment                           { CreateEnum( "MaxEquipment"                    , 46  ) };
    inline static EnumKeeper MaxBeer                                { CreateEnum( "MaxBeer"                         , 47  ) };
    inline static EnumKeeper MaxResourceVariance                    { CreateEnum( "MaxResourceVariance"             , 48  ) };
    inline static EnumKeeper RecruitGoldThreshold                   { CreateEnum( "RecruitGoldThreshold"            , 49  ) };
    inline static EnumKeeper BlacksmithSetting                      { CreateEnum( "BlacksmithSetting"               , 50  ) };
    inline static EnumKeeper FletcherSetting                        { CreateEnum( "FletcherSetting"                 , 51  ) };
    inline static EnumKeeper PoleturnerSetting                      { CreateEnum( "PoleturnerSetting"               , 52  ) };
    inline static EnumKeeper SellResource01                         { CreateEnum( "SellResource01"                  , 53  ) };
    inline static EnumKeeper SellResource02                         { CreateEnum( "SellResource02"                  , 54  ) };
    inline static EnumKeeper SellResource03                         { CreateEnum( "SellResource03"                  , 55  ) };
    inline static EnumKeeper SellResource04                         { CreateEnum( "SellResource04"                  , 56  ) };
    inline static EnumKeeper SellResource05                         { CreateEnum( "SellResource05"                  , 57  ) };
    inline static EnumKeeper SellResource06                         { CreateEnum( "SellResource06"                  , 58  ) };
    inline static EnumKeeper SellResource07                         { CreateEnum( "SellResource07"                  , 59  ) };
    inline static EnumKeeper SellResource08                         { CreateEnum( "SellResource08"                  , 60  ) };
    inline static EnumKeeper SellResource09                         { CreateEnum( "SellResource09"                  , 61  ) };
    inline static EnumKeeper SellResource10                         { CreateEnum( "SellResource10"                  , 62  ) };
    inline static EnumKeeper SellResource11                         { CreateEnum( "SellResource11"                  , 63  ) };
    inline static EnumKeeper SellResource12                         { CreateEnum( "SellResource12"                  , 64  ) };
    inline static EnumKeeper SellResource13                         { CreateEnum( "SellResource13"                  , 65  ) };
    inline static EnumKeeper SellResource14                         { CreateEnum( "SellResource14"                  , 66  ) };
    inline static EnumKeeper SellResource15                         { CreateEnum( "SellResource15"                  , 67  ) };
    inline static EnumKeeper DefWallPatrolRallytime                 { CreateEnum( "DefWallPatrolRallyTime"          , 68  ) };
    inline static EnumKeeper DefWallPatrolGroups                    { CreateEnum( "DefWallPatrolGroups"             , 69  ) };
    inline static EnumKeeper DefSiegeEngineGoldThreshold            { CreateEnum( "DefSiegeEngineGoldThreshold"     , 70  ) };
    inline static EnumKeeper DefSiegeEngineBuildDelay               { CreateEnum( "DefSiegeEngineBuildDelay"        , 71  ) };
    inline static EnumKeeper Unknown072                             { CreateEnum( "Unknown072"                      , 72  ) };
    inline static EnumKeeper Unknown073                             { CreateEnum( "Unknown073"                      , 73  ) };
    inline static EnumKeeper RecruitProbDefDefault                  { CreateEnum( "RecruitProbDefDefault"           , 74  ) };
    inline static EnumKeeper RecruitProbDefWeak                     { CreateEnum( "RecruitProbDefWeak"              , 75  ) };
    inline static EnumKeeper RecruitProbDefStrong                   { CreateEnum( "RecruitProbDefStrong"            , 76  ) };
    inline static EnumKeeper RecruitProbRaidDefault                 { CreateEnum( "RecruitProbRaidDefault"          , 77  ) };
    inline static EnumKeeper RecruitProbRaidWeak                    { CreateEnum( "RecruitProbRaidWeak"             , 78  ) };
    inline static EnumKeeper RecruitProbRaidStrong                  { CreateEnum( "RecruitProbRaidStrong"           , 79  ) };
    inline static EnumKeeper RecruitProbAttackDefault               { CreateEnum( "RecruitProbAttackDefault"        , 80  ) };
    inline static EnumKeeper RecruitProbAttackWeak                  { CreateEnum( "RecruitProbAttackWeak"           , 81  ) };
    inline static EnumKeeper RecruitProbAttackStrong                { CreateEnum( "RecruitProbAttackStrong"         , 82  ) };
    inline static EnumKeeper SortieUnitRangedMin                    { CreateEnum( "SortieUnitRangedMin"             , 83  ) };
    inline static EnumKeeper SortieUnitRanged                       { CreateEnum( "SortieUnitRanged"                , 84  ) };
    inline static EnumKeeper SortieUnitMeleeMin                     { CreateEnum( "SortieUnitMeleeMin"              , 85  ) };
    inline static EnumKeeper SortieUnitMelee                        { CreateEnum( "SortieUnitMelee"                 , 86  ) };
    inline static EnumKeeper DefDiggingUnitMax                      { CreateEnum( "DefDiggingUnitMax"               , 87  ) };
    inline static EnumKeeper DefDiggingUnit                         { CreateEnum( "DefDiggingUnit"                  , 88  ) };
    inline static EnumKeeper RecruitInterval                        { CreateEnum( "RecruitInterval"                 , 89  ) };
    inline static EnumKeeper RecruitIntervalWeak                    { CreateEnum( "RecruitIntervalWeak"             , 90  ) };
    inline static EnumKeeper RecruitIntervalStrong                  { CreateEnum( "RecruitIntervalStrong"           , 91  ) };
    inline static EnumKeeper DefTotal                               { CreateEnum( "DefTotal"                        , 92  ) };
    inline static EnumKeeper OuterPatrolGroupsCount                 { CreateEnum( "OuterPatrolGroupsCount"          , 93  ) };
    inline static EnumKeeper OuterPatrolGroupsMove                  { CreateEnum( "OuterPatrolGroupsMove"           , 94  ) };
    inline static EnumKeeper OuterPatrolRallyDelay                  { CreateEnum( "OuterPatrolRallyDelay"           , 95  ) };
    inline static EnumKeeper DefWalls                               { CreateEnum( "DefWalls"                        , 96  ) };
    inline static EnumKeeper DefUnit1                               { CreateEnum( "DefUnit1"                        , 97  ) };
    inline static EnumKeeper DefUnit2                               { CreateEnum( "DefUnit2"                        , 98  ) };
    inline static EnumKeeper DefUnit3                               { CreateEnum( "DefUnit3"                        , 99  ) };
    inline static EnumKeeper DefUnit4                               { CreateEnum( "DefUnit4"                        , 100 ) };
    inline static EnumKeeper DefUnit5                               { CreateEnum( "DefUnit5"                        , 101 ) };
    inline static EnumKeeper DefUnit6                               { CreateEnum( "DefUnit6"                        , 102 ) };
    inline static EnumKeeper DefUnit7                               { CreateEnum( "DefUnit7"                        , 103 ) };
    inline static EnumKeeper DefUnit8                               { CreateEnum( "DefUnit8"                        , 104 ) };
    inline static EnumKeeper RaidUnitsBase                          { CreateEnum( "RaidUnitsBase"                   , 105 ) };
    inline static EnumKeeper RaidUnitsRandom                        { CreateEnum( "RaidUnitsRandom"                 , 106 ) };
    inline static EnumKeeper RaidUnit1                              { CreateEnum( "RaidUnit1"                       , 107 ) };
    inline static EnumKeeper RaidUnit2                              { CreateEnum( "RaidUnit2"                       , 108 ) };
    inline static EnumKeeper RaidUnit3                              { CreateEnum( "RaidUnit3"                       , 109 ) };
    inline static EnumKeeper RaidUnit4                              { CreateEnum( "RaidUnit4"                       , 110 ) };
    inline static EnumKeeper RaidUnit5                              { CreateEnum( "RaidUnit5"                       , 111 ) };
    inline static EnumKeeper RaidUnit6                              { CreateEnum( "RaidUnit6"                       , 112 ) };
    inline static EnumKeeper RaidUnit7                              { CreateEnum( "RaidUnit7"                       , 113 ) };
    inline static EnumKeeper RaidUnit8                              { CreateEnum( "RaidUnit8"                       , 114 ) };
    inline static EnumKeeper HarassingSiegeEngine1                  { CreateEnum( "HarassingSiegeEngine1"           , 115 ) };
    inline static EnumKeeper HarassingSiegeEngine2                  { CreateEnum( "HarassingSiegeEngine2"           , 116 ) };
    inline static EnumKeeper HarassingSiegeEngine3                  { CreateEnum( "HarassingSiegeEngine3"           , 117 ) };
    inline static EnumKeeper HarassingSiegeEngine4                  { CreateEnum( "HarassingSiegeEngine4"           , 118 ) };
    inline static EnumKeeper HarassingSiegeEngine5                  { CreateEnum( "HarassingSiegeEngine5"           , 119 ) };
    inline static EnumKeeper HarassingSiegeEngine6                  { CreateEnum( "HarassingSiegeEngine6"           , 120 ) };
    inline static EnumKeeper HarassingSiegeEngine7                  { CreateEnum( "HarassingSiegeEngine7"           , 121 ) };
    inline static EnumKeeper HarassingSiegeEngine8                  { CreateEnum( "HarassingSiegeEngine8"           , 122 ) };
    inline static EnumKeeper HarassingSiegeEnginesMax               { CreateEnum( "HarassingSiegeEnginesMax"        , 123 ) };
    inline static EnumKeeper Unknown124                             { CreateEnum( "Unknown124"                      , 124 ) };
    inline static EnumKeeper AttForceBase                           { CreateEnum( "AttForceBase"                    , 125 ) };
    inline static EnumKeeper AttForceRandom                         { CreateEnum( "AttForceRandom"                  , 126 ) };
    inline static EnumKeeper AttForceSupportAllyThreshold           { CreateEnum( "AttForceSupportAllyThreshold"    , 127 ) };
    inline static EnumKeeper AttForceRallyPercentage                { CreateEnum( "AttForceRallyPercentage"         , 128 ) };
    inline static EnumKeeper Unknown129                             { CreateEnum( "Unknown129"                      , 129 ) };
    inline static EnumKeeper Unknown130                             { CreateEnum( "Unknown130"                      , 130 ) };
    inline static EnumKeeper AttUnitPatrolRecommandDelay            { CreateEnum( "AttUnitPatrolRecommandDelay"     , 131 ) };
    inline static EnumKeeper Unknown132                             { CreateEnum( "Unknown132"                      , 132 ) };
    inline static EnumKeeper SiegeEngine1                           { CreateEnum( "SiegeEngine1"                    , 133 ) };
    inline static EnumKeeper SiegeEngine2                           { CreateEnum( "SiegeEngine2"                    , 134 ) };
    inline static EnumKeeper SiegeEngine3                           { CreateEnum( "SiegeEngine3"                    , 135 ) };
    inline static EnumKeeper SiegeEngine4                           { CreateEnum( "SiegeEngine4"                    , 136 ) };
    inline static EnumKeeper SiegeEngine5                           { CreateEnum( "SiegeEngine5"                    , 137 ) };
    inline static EnumKeeper SiegeEngine6                           { CreateEnum( "SiegeEngine6"                    , 138 ) };
    inline static EnumKeeper SiegeEngine7                           { CreateEnum( "SiegeEngine7"                    , 139 ) };
    inline static EnumKeeper SiegeEngine8                           { CreateEnum( "SiegeEngine8"                    , 140 ) };
    inline static EnumKeeper CowThrowInterval                       { CreateEnum( "CowThrowInterval"                , 141 ) };
    inline static EnumKeeper Unknown142                             { CreateEnum( "Unknown142"                      , 142 ) };
    inline static EnumKeeper AttMaxEngineers                        { CreateEnum( "AttMaxEngineers"                 , 143 ) };
    inline static EnumKeeper AttDiggingUnit                         { CreateEnum( "AttDiggingUnit"                  , 144 ) };
    inline static EnumKeeper AttDiggingUnitMax                      { CreateEnum( "AttDiggingUnitMax"               , 145 ) };
    inline static EnumKeeper AttUnit2                               { CreateEnum( "AttUnit2"                        , 146 ) };
    inline static EnumKeeper AttUnit2Max                            { CreateEnum( "AttUnit2Max"                     , 147 ) };
    inline static EnumKeeper AttMaxAssassins                        { CreateEnum( "AttMaxAssassins"                 , 148 ) };
    inline static EnumKeeper AttMaxLaddermen                        { CreateEnum( "AttMaxLaddermen"                 , 149 ) };
    inline static EnumKeeper AttMaxTunnelers                        { CreateEnum( "AttMaxTunnelers"                 , 150 ) };
    inline static EnumKeeper AttUnitPatrol                          { CreateEnum( "AttUnitPatrol"                   , 151 ) };
    inline static EnumKeeper AttUnitPatrolMax                       { CreateEnum( "AttUnitPatrolMax"                , 152 ) };
    inline static EnumKeeper AttUnitPatrolGroupsCount               { CreateEnum( "AttUnitPatrolGroupsCount"        , 153 ) };
    inline static EnumKeeper AttUnitBackup                          { CreateEnum( "AttUnitBackup"                   , 154 ) };
    inline static EnumKeeper AttUnitBackupMax                       { CreateEnum( "AttUnitBackupMax"                , 155 ) };
    inline static EnumKeeper AttUnitBackupGroupsCount               { CreateEnum( "AttUnitBackupGroupsCount"        , 156 ) };
    inline static EnumKeeper AttUnitEngage                          { CreateEnum( "AttUnitEngage"                   , 157 ) };
    inline static EnumKeeper AttUnitEngageMax                       { CreateEnum( "AttUnitEngageMax"                , 158 ) };
    inline static EnumKeeper AttUnitSiegeDef                        { CreateEnum( "AttUnitSiegeDef"                 , 159 ) };
    inline static EnumKeeper AttUnitSiegeDefMax                     { CreateEnum( "AttUnitSiegeDefMax"              , 160 ) };
    inline static EnumKeeper AttUnitSiegeDefGroupsCount             { CreateEnum( "AttUnitSiegeDefGroupsCount"      , 161 ) };
    inline static EnumKeeper AttUnitMain1                           { CreateEnum( "AttUnitMain1"                    , 162 ) };
    inline static EnumKeeper AttUnitMain2                           { CreateEnum( "AttUnitMain2"                    , 163 ) };
    inline static EnumKeeper AttUnitMain3                           { CreateEnum( "AttUnitMain3"                    , 164 ) };
    inline static EnumKeeper AttUnitMain4                           { CreateEnum( "AttUnitMain4"                    , 165 ) };
    inline static EnumKeeper AttMaxDefault                          { CreateEnum( "AttMaxDefault"                   , 166 ) };
    inline static EnumKeeper AttMainGroupsCount                     { CreateEnum( "AttMainGroupsCount"              , 167 ) };
    inline static EnumKeeper TargetChoice                           { CreateEnum( "TargetChoice"                    , 168 ) };
    inline static EnumKeeper None                                   { CreateEnum( "NULL"                            , 999 ) };
  };

  // using stuff:
  using AIC           = AIPersonalityFieldsEnum;
  using AICEnum       = AIPersonalityFieldsEnum::EnumKeeper;
  using AIName        = AICharacterName;
  using AINameEnum    = AICharacterName::EnumKeeper;
  using Farm          = FarmBuilding;
  using FarmEnum      = FarmBuilding::EnumKeeper;
  // using Resource   = ...;
  using ResourceEnum  = Resource::EnumKeeper;
  using SE            = SiegeEngine;
  using SEEnum        = SiegeEngine::EnumKeeper;
  using HSE           = HarassingSiegeEngine;
  using HSEEnum       = HarassingSiegeEngine::EnumKeeper;
  using Target        = TargetingType;
  using TargetEnum    = TargetingType::EnumKeeper;
  using Fletcher      = FletcherSetting;
  using FletcherEnum  = FletcherSetting::EnumKeeper;
  using Pole          = PoleturnerSetting;
  using PoleEnum      = PoleturnerSetting::EnumKeeper;
  using Smith         = BlacksmithSetting;
  using SmithEnum     = BlacksmithSetting::EnumKeeper;
  //using Unit        = ...;
  using UnitEnum      = Unit::EnumKeeper;
  using DUnit         = DiggingUnit;
  using DUnitEnum     = DiggingUnit::EnumKeeper;
  using MUnit         = MeleeUnit;
  using MUnitEnum     = MeleeUnit::EnumKeeper;
  using RUnit         = RangedUnit;
  using RUnitEnum     = RangedUnit::EnumKeeper;
  //using NPC         = ...;
  using NPCEnum       = NPC::EnumKeeper;

  // Melee and Ranged Unit are unused currently
  // -> they share the ids though, so the question is more, would other units work
  // -> so the question is, would the game still work if I send ladderman in the main attack force?
  //   -> maybe apply that restriction later (keep digging unit though, others will not work (or?))
}

#endif //PSEUDOENUMSAI