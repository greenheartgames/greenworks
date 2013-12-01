greenworks
===

A node.js plugin to integrate with [Steamworks](http://www.steampowered.com/steamworks/).

The plugin was developed the enable the Steam release of Greenheart Games' [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/), a game powered by [node-webkit](https://github.com/rogerwang/node-webkit).

For Game Dev Tycoon we've built 5 methods, based on Steam API:
- initAPI() - this one is used when the app starts. This method uses SteamAPI_Init and ISteamUserStats::RequestCurrentStats methods;
- getCloudQuota() - is used to get steam cloud usage. Uses methods from ISteamRemoteStorage;
- saveTextToFile() - saves some textual data locally, but it also gets synced with steam cloud in background;
- readTextFromFile() - gets some data, previously saved using saveTextToFile(). Both readTextFromFile and saveTextToFile are using ISteamRemoteStorage;
- activateAchievement() - sets a user achievement, using ISteamUserStats.

Dependencies
===
Before building modules you need to download Steamworks SDK ([https://partner.steamgames.com/](https://partner.steamgames.com/)).

Build
===
In order to build modules, you need first to add redistributable_bin/ (under SDK folder) to PATH (so steam_api.lib is linked while building).

```sh
node-gyp configure
node-gyp build
```

If everything goes fine, it will create a folder deploy/ with node module within (gdt-steam-X.node, where X depends on your os. E.g. for mac it will be gdt-steam-osx.node). Now it's possible to use the module from a node.js application. Make sure libsteam_api dynamic library is distributed along with the application.

Test
===
Here is a simple application, which inits steam API. 
```javascript
var steam = require('./greenworks-steam-osx');

if (steam.initAPI()) {
    console.log('Steam API initalised');
}
else {
	console.log('Error initalising Steam API');
}
```

Troubleshooting
===
Possible issues while building/running:

    * Library not loaded: @loader_path/libsteam_api.dylib (or .dll for windows)
That means you didn't copy this file to your app directory and it can't link it at runtime.

    * SteamAPI_Init() failed; unable to locate a running instance of Steam, or a local steamclient.dll
Your steam client should be running (or you can copy the steamclient dynamic library into your app directory).
    
    * SteamAPI_Init() failed; no appID found.
Steam can't determine for which steam application you are trying to initialise the API. Either launch the game from Steam, or put the file `steam_appid.txt` containing the correct appID in your app folder. steam_appid.txt should contain just your application id, nothing else.
