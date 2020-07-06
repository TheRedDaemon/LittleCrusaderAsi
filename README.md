# LittleCrusaderAsi
## Summary
LittleCrusaderAsi is a modification for Stronghold Crusader 1 inspired by the awesome work on the [UnofficialCrusaderPatch][1]. The later provides a ton of features and is quite flexible when it comes to version compatibility. However, the patcher modifies the executable, which requires restarts if something needs changes.

The LittleCrusaderAsi tries to tackle this issue by using a very common technique when it comes to modding. The modification uses an AsiLoader (see [Installation](##installation)) to load a Dynamic Link Libray (dll, just named asi in this approach) into the address space of a program. That allows to execute additional code and modify the software during runtime.

One hope for this project is to one day provide most if not all features of the UnofficialCrusaderPatch, while also allowing for more dynamic changes, like reloading of KI parameters at runtime to make modifications easier.

Currently, **only the basic program structure is implemented**, which I hope can be used to extend the mod in a structured (but not restricted) way. The individual features are granted a lot of freedom how to handle issues like missing addresses, so version compatibility is provided on a per feature basis.  
That being said, as long as no one with other versions then 1.41 and 1.41.1-E digs through the addresses, the mods will mostly be limited to the later. Additionally, the restriction is further defined by the mod being so far created using a German crusader version. As a result:

**Full compatibility so far is only guaranteed for Crusader versions 1.41 and 1.41.1-E and only if they are code wise identical to the German version.** Other "versions" of 1.41 and 1.41.1-E might even create crashes at the moment, since the current system does not know how to handle them. (There are reports of problems with the Russian version and the UnofficialCrusaderPatch, so this might be candidate.) In all other cases (for example version 1.2), the feature should simply not work and write this issue to a log file.

For more information on the implementation approach and structure, please take a look at this Readme(add it) inside the *src*-folder. I hope it is a good read for everyone who wants to contribute. ^^

## Installation
### Getting LittleCrusaderAsi
As of now (Juli 2020), no release build is provided.

## Current Features

## Additional Information
### Dependencies
LittleStrongholdAsi uses third party tools/code.  
You will find them together with a copy of their licenses under [THIS](https://github.com/TheRedDaemon/LittleCrusaderAsi/tree/Dev/src/LittleCrusaderAsi/dependencies) folder.

Dependency | Used Version
------------ | -------------
[Easylogging++](https://github.com/amrayn/easyloggingpp) | 9.96.7
[JSON for Modern C++](https://github.com/nlohmann/json) | 3.7.3

### Special Thanks
Basically everyone involved with and thanked by the [UnofficialCrusaderPatch][1]. So pass the thanks down the thanks tree. ^^  

And especially [Firefly Studios](https://fireflyworlds.com/) for the creation of the Stronghold series. Have never seen a mod without a game in the first place...

[1]:https://github.com/Sh0wdown/UnofficialCrusaderPatch