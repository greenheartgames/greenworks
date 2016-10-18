## Methods

### greenworks.initAPI()

Returns a `Boolean` whether Steam APIs were successfully initialized or not.

Note: When testing this, you need to launch and log in the Steam Client, and
put `steam_appid.txt` under your app directory.

### greenworks.init()

Returns a `Boolean` whether Steam APIs were successfully initialized or not.
The same as `greenworks.init()`, but this API shows better error messages.

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

### greenworks.activateGameOverlayToWebPage(url)

* `url` String: a full url, e.g. http://www.steamgames.com.

Open a specified url in steam game overlay.

### greenworks.isSubscribedApp(appId)

* `appId` Integer: The APP ID of your game

Returns a `Boolean` indicates whether the user has purchased that app.

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
  var image_buffer = greenworks.getImageRGBA(handle);
  var size = greenworks.getImageSize(handle);
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
