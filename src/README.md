# Implementation Concepts

## Table of contents
* [Overview](#overview)
* [ModBase Class](#modbase-class)
* [Execution Order](#execution-order)
* [Something unclear?](#something-unclear)

## Overview
This text is intended to give a little overview over the ideas that went into the implementation. Instead of repeating what the main [Readme](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/master/README.md) says, it will spend more time on the details.

In general, once the *LittleCrusaderAsi* is loaded, the [DLLMain](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/master/src/LittleCrusaderAsi/modcore/dllmain.cpp) is executed, which starts a procedure that roughly follows these steps:  
*Initializing Logger* -> *Creating Mod Loader* -> *Loading Mod Config* -> *Creating Mods*  
The last step being the most extensive.

The structure of the project itself is pretty straightforward. The actual [project folder](https://github.com/TheRedDaemon/LittleCrusaderAsi/tree/master/src/LittleCrusaderAsi) contains four folders. "modcore" is filled with files that are general or supportive in nature. "modclasses" contains the actual 'mods' and the subfolder "enumheaders", true to its name, mostly header-files with enums. The content of "dependencies" are external files, currently (October 2020) the JSON parser and the logger. "misc" is for stuff with no real place. Source and header-files are not separated. This structure was upholded manually, since Visual Studio projects seem to use their own filter system.  
That being said, it is far away from being truly ordered.

Note that everything is subject of change. If it seems to be a good idea to overthrow everything here, it might be done. Also, the code it often not very clean, but extensivly commented, so that might also be worth a look. Additionally, one can check out simpler already existing mods, like the [VersionGetter](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/master/src/LittleCrusaderAsi/modclasses/versionGetter.h).

## ModBase Class
The [ModBase](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/master/src/LittleCrusaderAsi/modclasses/modBase.h) is more or less the heartpiece of the concept. Instead of creating independend structures for different functions, the idea was to have the possibility to handle all mods through one pipeline. As a result, all mods are intended to be puplic derived from this class. If properly 'registered', the [ModLoader (see also .cpp)](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/master/src/LittleCrusaderAsi/modcore/modLoader.h) will take care that the overriden functions are executed in [proper order](#execution-order).

To get a mod properly constructed, multiple steps are necessary.  
First, it needs an entry in the [ModType enum](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/master/src/LittleCrusaderAsi/modclasses/enumheaders/modtypes.h) and if it is intended to receive configuration or be created for its own and not as dependency for other mods, also in the "NLOHMANN_JSON_SERIALIZE_ENUM" structure to enable the JSON library to parse it as an enum.  
Now, one has to think if this new mod should use some features of another one. If that is the case, the mod needs to call the ModBase constructor that receives the [ModKeeper](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/master/src/LittleCrusaderAsi/modcore/modKeeper.h) in its own constructor. This object will manage the dependencies of other mods and allows accessing them if the need was registered. To achive this, getDependencies() must be overriden to return a vector of needed other mod types. The ModLoader will then make sure that they are constructed, even if they have no configuration entry (default values are important!). If everything worked, the mod will then be able to access a requested mod by calling getMod<type>(), but only starting from initialize().
The last thing now is putting an include of the mod in [modConstructors.cpp](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/master/src/LittleCrusaderAsi/modcore/modConstructors.cpp) and create an return entry in the createMod() switch statement for the ModType.

The constructors and destructors as well as the calls to getDependencies() have no guaranteed order. Therefore, the constructor is intended to only register the configuration. Calls to getMod<T>() will fail. Currently, there are three functions which are executed in relation to the dependency order: initialize(), firstThreadAttachAfterModAttachEvent() and cleanUp().
  
**initialize()** will be called after every mod is created, when the games code already resides in memory, but no code was executed so far (I presume). Note that this function needs to set the variable 'initialized' to true, or otherwise, mods that depend on this will get an empty pointer. (That should only be done if the initialization went well). As a result, getMod() should work now.

**firstThreadAttachAfterModAttachEvent()** is executed after the dllMain received the first event of a thread being attached. This is kind of a dummy function, since it was noticed that once this happens, the AIC value array was already created. Something that has not yet happened during initialize(). It is unclear if this is valueable for other structures and its reliability is questionable.

**cleanUp()** is called by the ModLoader destructor, which means when everything closes. One notable thing: Different then the other two, it runs through the mods in reverse dependency order.

## Address Resolver
TODO

## Execution Order
TODO

## Something unclear?
Do not be shy, create an issue and ask. ^-^  
Questions might help to get this write-up more understandable and if this leads to someone contributing, everyone gets something out of it.
