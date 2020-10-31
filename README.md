# LittleCrusaderAsi

## Table of contents
* [Summary](#summary)
* [Installation](#installation)
  * [Getting *LittleCrusaderAsi*](#getting-littlecrusaderasi)
  * [Getting an AsiLoader](#getting-an-asiloader)
  * [Location of files and configuration](#location-of-files-and-configuration)
* [Current Features](#current-features)
* [FAQ](#faq)
* [Credits](#credits)
* [Additional Information](#additional-information)
  * [License](#license)
  * [Dependencies](#dependencies)
  * [Other Crusader projects](#other-crusader-projects)
* [Special Thanks](#special-thanks)

## Summary
*LittleCrusaderAsi* is a modification for Stronghold Crusader 1 inspired by the awesome work on the [UnofficialCrusaderPatch][1]. The later provides a ton of features and is quite flexible when it comes to version compatibility. However, the patcher modifies the executable, which requires restarts if something needs changes.

The *LittleCrusaderAsi* tries to tackle this issue by using a very common technique when it comes to modding. The modification uses an AsiLoader (see [Installation](#installation)) to load a Dynamic Link Library (dll, just named asi in this approach) into the address space of a program. That allows to execute additional code and modify the software during runtime.

Currently, **only some simple features and the basic program structure are implemented**. One hope is that the later can be used to extend the mod in a structured (but not restricted) way. The individual features are granted a lot of freedom how to handle issues like missing addresses, so version compatibility is provided on a per feature basis.  
That being said, as long as no one with other versions then 1.41 and 1.41.1-E digs through the addresses, the mods will mostly be limited to the later. Additionally, the restriction is further defined by the mod being so far created using a German crusader version. As a result:

**Full compatibility so far is only guaranteed for Crusader versions 1.41 and 1.41.1-E and only if they are codewise identical to the german version. (A guess would be that this includes all using the latin alphabet, like the english version.)** Other "versions" of 1.41 and 1.41.1-E might even create crashes at the moment, since the current system might not know how to handle them. (There are reports of problems with the Russian version and the UnofficialCrusaderPatch, so this might be a candidate.) In all other cases (for example version 1.2), the feature should simply not work and the issue will be written to a log file.

For more information on the implementation approach and structure, please take a look at the [README](src/README.md) inside the *src*-folder. I hope it is a good read for everyone who wants to contribute. ^^

## Installation
### Getting LittleCrusaderAsi
Go [HERE](https://github.com/TheRedDaemon/LittleCrusaderAsi/releases/latest) to get the latest release.

The project can also be build from source. It is written in C++17 and the repository contains a Visual Studio 2017 project. One must make sure to use the Win32 mode and (for DEBUG mode) redefine *Properties->General->Output Directory* and *Properties->Debugging->Command*, since both are currently configured to allow debugging the dll and point therefore in the file structure of the latest who edited the project file.

### Getting an AsiLoader
*LittleCrusaderAsi* relies on a third party tool that loads it into the process. Basically only one tool was tested, but more or less two variants are possible:

* Using [Ultimate-ASI-Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases) directly. The release will contain a 'dinput8.dll' which needs to be placed in the game root directory. However, it needs to be renamed into a dll the game loads. One that works is 'ddraw.dll'. Other names might be possible, but were not tested. Now the game will load ASI files placed in game root directory or the folders 'scripts' or 'plugins', if they are created inside the game directory.

* Using [DxWrapper](https://github.com/elishacloud/dxwrapper), which uses code of the Ultimate-ASI-Loader to provide the same feature alongside a ton more, like [Dd7to9](https://github.com/elishacloud/dxwrapper/wiki/DirectDraw-to-Direct3D9-Conversion). The later allows stuff like running the game in window mode (with a few issues, like the cursor not being bound to the window). For the installation, please refer to the DxWrapper documentation. In short, it is required that the provided 'ddraw.dll' is placed in the game root folder alongside 'dxwrapper.dll' and 'dxwrapper.ini'. The asi-loading needs to be activated in the 'dxwrapper.ini'. It also supports the folders 'scripts' and 'plugins'.

Both have also additional features, but if interested, please refer to their documentations. Furthermore, loading *LittleCrusaderAsi* into the process by other means may also work, but there are no guarantees.

### Location of files and configuration
First, all now mentioned files need to be placed in the same directory. This might either be the game root, or one of the supported folders 'scripts'/'plugins' (recommended).

Currently three files are needed:

1. The 'LittleCrusaderAsi.asi' of course.
2. The 'logger.config', which is the configuration for the logger. Simply using the file [HERE](src/LittleCrusaderAsi/dependencies/logger/logger.config) is enough. For customisation please refer to the easylogging++ documentation (see [Dependencies](#dependencies)). Omitting it will result in the use of a default configuration.
3. The actual 'modConfig.json'. This file contains the mod configuration. [THIS](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/Dev/src/LittleCrusaderAsi/dependencies/jsonParser/modConfig.json.example) non valid json-file will contain examples for every possible adjustments and features.

If the asi-file is missing, it will not work. If the modConfig is missing or invalid the mod will crash with a log.
Log output is written to a file named 'crusaderAsi.log', which will be overridden on every start and generated in the same folder as the other files. In case of a noticed exception or if requested by a feature, the log will be copied and renamed on exit. Hard crashes (in case of corrupted memory for example) will likely not show up in the log.

At the moment, configuring the modification is kinda clunky. The focus for now is on functionality instead of usability. However, if anyone wants to provide a GUI for generating the configuration json, feel free. ^^

## Current Features
A list of the features currently provided by the *LittleCrusaderAsi*. The short summery will not contain in-depth details regarding the configuration. Please search in the [example json](https://github.com/TheRedDaemon/LittleCrusaderAsi/blob/Dev/src/LittleCrusaderAsi/dependencies/jsonParser/modConfig.json.example) after the feature name to get information about the configuration.

**KeyboardInterceptor**  
*Supported versions:* All (in theory)  
The keyboard interceptor has two features. First, it allows other mods to register key functions. Second, it allows to redefine keypresses. The later is directly handled through the configuration of this mod. It is therefore possible to tranform a press on the 'a'-key to a press on 'b'. However, this is only possible for single keys and is done by catching the actual key and telling the game another was pressed. As a result, combinations like 'shift+a' are carried over (for example would become 'shift+b') and text inputs are unreliable. To circumvent this issue the interceptor as a whole has a configurable on/off-key.

**BuildRangeChanger**  
*Supported versions:* V1.41.1-E | V1.41  
A very simple modification that allows to change the castle build range. Can be configured to be active at start, or can register a key function used for activation (with activated KeyboardInterceptor). Ranges are defined by map size. Key function allows toggle between vanilla- and configuration-values. The AI will build walls regardless, but no towers if they fall out of range.

**AICLoad**  
*Supported versions:* V1.41.1-E | V1.41  
AIC loader of the *LittleCrusaderAsi*. Allows to define an AIC-folder and a list of AIC-files to be loaded. The mod was created to support the file format defined by the UCP (2.14) ([Example with vanilla AIC](https://github.com/Sh0wdown/UnofficialCrusaderPatch/blob/master/UnofficialCrusaderPatch/AIC/Resources/vanilla.json)). The changes can either be loaded at the start or after requested. It is therefore possible to configure three key functions. The first simply switches between the current AIC data and the vanilla AICs. The second allows to reload the 'main'-AIC, which is the first in the AIC-list in the configuration file. The third allows to reload all AIC-files. AIC values are applied starting from the the main AIC to the second, then the third and so on. Note that this order is currently applied per value, so it is possible to carelessly create AI-amalgamations if one does only define half of the Rats values, while having another AIC with another Rat in line.

One last note. There is currently no direct way to indicate in-game that a function was executed and works. If in doubt, check the log-file created in the mod directory.

## FAQ
A place for questions some might have.

1. **Is *LittleCrusaderAsi* compatible with the UCP?**  
A test with version 0.1 of *LittleCrusaderAsi* and version 2.14 of the UCP (with not all but multiple features enabled) yielded no obvious errors. Considering that the current features of *LittleCrusaderAsi* only alter values and not processes, it should be compatible. That being said, using AIC modifications in both the UCP and *LittleCrusaderAsi* might lead to unwanted overwrites.

## Credits

* [TheRedDaemon](https://github.com/TheRedDaemon) - Maintainer and Developer
* The people of the [UCP][1] - Indirect supporters, by finding knowledge that can be taken. :-)

## Additional Information
### License
This repository is licensed using the MIT License.

### Dependencies
LittleStrongholdAsi uses third party tools/code.  
You will find them together with a copy of their licenses under [THIS](src/LittleCrusaderAsi/dependencies) folder.

Dependency | Used Version
------------ | -------------
[Easylogging++](https://github.com/amrayn/easyloggingpp) | 9.96.7
[JSON for Modern C++](https://github.com/nlohmann/json) | 3.7.3
[DDFontEngine](https://realmike.org/blog/projects/fast-bitmap-fonts-for-directdraw) | - (Old Tutorial for DirectDraw fonts)

### Other Crusader projects
There are a handful of projects on GitHub for Stronghold (Crusader) 1. So take a look. After all, every development might be helpful for other projects.  
(Note: Without any claim to comprehensiveness.)

* Was the [UnofficialCrusaderPatch][1] already mentioned? ^^
* The [Gm1KonverterCrossPlatform](https://github.com/PodeCaradox/Gm1KonverterCrossPlatform) is a tool developed for easier editing of all kinds of Crusader textures and sprites.
* While the [Sourcehold](https://github.com/sourcehold) main project seems to laying dormant at the moment (October 2020), it might still be worth a look.
  * As an interesting subproject, [sourcehold-maps](https://github.com/sourcehold/sourcehold-maps) tries do analyze how the map files are structured and is still active (October 2020). Who knows what might be possible by only messing with the map files?
* Any more suggestions? Feel free to add them (as long as they are related to Stronghold at least)!

## Special Thanks
Basically everyone involved with and thanked by the [UnofficialCrusaderPatch][1]. So pass the thanks down the thanks tree. ^^  

And especially [Firefly Studios](https://fireflyworlds.com/) for the creation of the Stronghold series. There can not be a mod without a game in the first place...

[1]:https://github.com/Sh0wdown/UnofficialCrusaderPatch
