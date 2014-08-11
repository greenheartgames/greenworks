greenworks
===

A node.js plugin to integrate with [Steamworks](http://www.steampowered.com/steamworks/).
The plugin was developed to enable the Steam release of Greenheart Games' [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/), a game powered by [node-webkit](https://github.com/rogerwang/node-webkit). It has since been used by [other projects](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).

API
===
For Game Dev Tycoon we've added support for several Steam related functionality, such as Workshop synchronization, Steam Cloud and Steam Achievements.
The methods we used are based on the Steamworks SDK.

**Greenworks**

* Greenworks.initAPI()
* Greenworks.getCloudQuotas()
* Greenworks.saveTextToFile(string fileName, string content, func success, func error)
* Greenworks.readTextFromFile(string fileName, func success, func error)
* Greenworks.activateAchievement(string achievementId, func success, func error)
* Greenworks.enableCloud()
* Greenworks.isCloudEnabled()
* Greenworks.isCloudEnabledForUser()
* Greenworks.getNumberOfPlayers()
* Greenworks.ugcPublish(string fileName, string title, string description, string imageFile, func successCallback, func errorCallback, func progressCallback)
* Greenworks.ugcPublishUpdate(int publishedFileId, string fileName, string title, string description, string imageFile, func successCallback, func errorCallback, func progressCallback)
* Greenworks.ugcGetItems(int type, int sort, func successCallback, func errorCallback, func progressCallback)
* Greenworks.ugcGetUserItems(int type, int sort, int filter, func successCallback, func errorCallback, func progressCallback)
* Greenworks.ugcDownloadItem(string fileName, int hFile, string targetFolder, func successCallback, func errorCallback, func progressCallback)
* Greenworks.ugcSynchronizeItems(string targetFolder, func success, func error, func progress)
* Greenworks.getCurrentGameLanguage()
* Greenworks.getCurrentUILanguage()
* Greenworks.getSteamId()
* Greenworks.ugcShowOverlay(optional workshopItemId)
* Greenworks.ugcUnsubscribe(int publishedFileId, func success, func error, func progress)
* Greenworks.getCurrentGameInstallDir()
* Greenworks.runCallbacks()

***

**Greenworks.Utils**

* Greenworks.Utils.createArchive(string zipFile, string sourceDir, string password,  int compressionLevel, func success, func error)
* Greenworks.Utils.extractArchive(string zipFile, string targetDir, string password, func success, func error)
* Greenworks.Utils.sleep(int ms)
* Greenworks.Utils.getOS(int ms)
* Greenworks.Utils.move(string sourceFolder, string targetFolder)
* Greenworks.Utils.enableConsole()
* Greenworks.Utils.disableConsole()
* Greenworks.Utils.enableWriteToLog(string targetFile)
* Greenworks.Utils.disableWriteToLog()

Please consult the [Greenworks API documentation](https://github.com/greenheartgames/greenworks/blob/master/docs/Greenworks%20API.pdf) for more information.

Dependencies
===
Before building modules you need to download Steamworks SDK ([https://partner.steamgames.com/](https://partner.steamgames.com/)).
Note that the steamworks SDK libraries and docs are only usable under license from Valve. If you fork Greenworks, don't commit any of the dependencies from the steamworks SDK to a public place.

Usage
===
We provide compiled versions of greenworks under [releases](https://github.com/greenheartgames/greenworks/releases).
To use the binaries you will have to copy them alongside the steam libraries and then load the module via require.

Test
===
Here is a simple application, which inits steam API. 
```javascript
var greenworks = require('./greenworks-osx');

if (greenworks.initAPI()) {
    console.log('Steam API initalised');
}
else {
	console.log('Error initalising Steam API');
}
```

Build
===
If you want to build greenworks yourself, you need first to add redistributable_bin/ (under SDK folder) to PATH (so steam_api.lib is linked while building).

```sh
node-gyp configure
node-gyp build
```

If everything goes fine, it will create a folder deploy/ with node module within (greenworks-X.node, where X depends on your os. E.g. for mac it will be greenworks-osx.node). Now it's possible to use the module from a node.js application. Make sure libsteam_api dynamic library is distributed along with the application.


Troubleshooting
===
Possible issues while building/running:

    * Library not loaded: @loader_path/libsteam_api.dylib (or .dll for windows)
That means you didn't copy this file to your app directory and it can't link it at runtime.

    * SteamAPI_Init() failed; unable to locate a running instance of Steam, or a local steamclient.dll
Your steam client should be running (or you can copy the steamclient dynamic library into your app directory).
    
    * SteamAPI_Init() failed; no appID found.
Steam can't determine for which steam application you are trying to initialise the API. Either launch the game from Steam, or put the file `steam_appid.txt` containing the correct appID in your app folder. steam_appid.txt should contain just your application id, nothing else.

License
===
Greenworks is published under the MIT license. See `LICENSE` file for details.

Help us improve!
===
Greenworks was developed to enable a minimum set of features for the Game Dev Tycoon release and could surely use more development. If you improve the project please consider opening a pull-request.
