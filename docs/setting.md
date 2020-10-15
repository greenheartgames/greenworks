## Methods

### greenworks.initAPI()

Returns a `Boolean` whether Steam APIs were successfully initialized or not.

Note: When testing this, you need to launch and log in the Steam Client,
and create a steam_appid.txt file with your Steam APP ID
(or the steamworks example APP ID) under your app directory.

### greenworks.init()

Returns a `True` when Steam APIs were successfully initialized, otherwise throw
an error.

### greenworks.isSteamRunning()

Returns a `Boolean` whether Steam is running.

### greenworks.restartAppIfNecessary(appId)

* `appId` Integer: The APP ID of your game

If your app was not launched via Steam, this will signal Steam to launch your
app, and then cause your app to quit.

There's not a moment to lose after you call `restartAppIfNecessary()`, but if
it returns `true`, your app is being restarted.

### greenworks.getAppId()

Returns an `Integer` represents the app id of the current process.

### greenworks.getAppBuildId()

Returns an `Integer` representing the app's build id. May change at any time based on backend updates to the game.

### greenworks.getSteamId()

Returns an [`SteamID`](friends.md#steamid) object represents the current Steam
user.

### greenworks.getCurrentGameLanguage()

Returns a `String` represents the current language from Steam specifically set
for the game.

### greenworks.getCurrentUILanguage()

Returns a `String` represents the current language from Steam set in UI.

### greenworks.getCurrentGameInstallDir()

Not implement yet.

### greenworks.getAppInstallDir(app_id, buffer, buffer_size)

* `app_id` Integer: The APP ID of your game

Returns a `String` representing the absolute path to the app's installation directory.

### greenworks.getNumberOfPlayers(success_callback, [error_callback])

* `success_callback` Function(num_of_players)
  * `num_of_players` Integer: the current number of players on Steam.
* `error_callback` Function(err)

### greenworks.activateGameOverlay(option)

* `option` String: valid options are `Friends`, `Community`, `Players`,
  `Settings`, `OfficialGameGroup`, `Stats` and `Achievements`.

Activate the game overlay with the `option` dialog opens.

### greenworks.isGameOverlayEnabled()

Return `Boolean` indicates whether Steam overlay is enabled/disabled.

### greenworks.isSteamInBigPictureMode()

Return `Boolean` indicates whether Steam is in Big Picture mode. 
Will always return `false` if the application is not in Steam's `game` category.

### greenworks.activateGameOverlayToWebPage(url)

* `url` String: a full url, e.g. http://www.steamgames.com.

Open a specified url in steam game overlay.

### greenworks.isSubscribedApp(appId)

* `appId` Integer: The APP ID of your game

Returns a `Boolean` indicates whether the user has purchased that app.

### greenworks.isAppInstalled(appId)

* `appId` Integer: The APP ID of your game

Returns a `Boolean` indicating whether the app is currently installed. The app may not actually be owned by the user. 

Only works for base applications, for DLC use `isDLCInstalled` instead.

### greenworks.getImageSize(handle)

* `handle` Integer: The image handle

Returns an `object` that contains image’s width and height values.

### greenworks.getImageRGBA(handle)

* `handle` Integer: The image handle

Returns a `Buffer` that contains image data in RGBA format.

An example of saving image to `png` format:

```js
var greeenworks = require('./greenworks');
// Relies on 'jimp' module. Install it via 'npm install jimp'.
var Jimp = require('jimp');

var friends = greenworks.getFriends(greenworks.FriendFlags.Immediate);
if (friends.length > 0) {
  var handle = greenworks.getSmallFriendAvatar(friends[0].getRawSteamID());
  if (!handle) {
    console.log("The user don't set small avartar");
    return;
  }
  var image_buffer = greenworks.getImageRGBA(handle);
  var size = greenworks.getImageSize(handle);
  if (!size.height || !size.width) {
    console.log("Image corrupted. Please try again");
    return;
  }
  console.log(size);
  var image = new Jimp(size.height, size.width, function (err, image) {
    for (var i = 0; i < size.height; ++i) {
      for (var j = 0; j < size.width; ++j) {
        var idx = 4 * (i * size.height + j);
        var hex = Jimp.rgbaToInt(image_buffer[idx], image_buffer[idx+1],
            image_buffer[idx+2], image_buffer[idx+3]);
        image.setPixelColor(hex, j, i);
      }
    }
  });
  image.write("/tmp/test.png");
}
```

### greenworks.getIPCountry()

Returns the 2 digit ISO 3166-1-alpha-2 format country code which client is running in, e.g "US" or "UK".

### greenworks.getLaunchCommandLine()

Gets the command line if the game was launched via Steam URL, e.g. `steam://run/<appid>//<command line>/`. This method is preferable to launching with a command line via the operating system, which can be a security risk. In order for rich presence joins to go through this and not be placed on the OS command line, you must enable "Use launch command line" from the Installation > General page on your app.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamApps#GetLaunchCommandLine)