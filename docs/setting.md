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

### greenworks.getSteamId()

Returns an [`SteamID`](friends.md#steamid) object represents the current Steam
user.

### greenworks.activateAchievement(achievement, success_callback, [error_callback])

* `achievement` String
* `success_callback` Function()
* `error_callback` Function(err)

The `achievement` represents the unlocked achievement in your game.

### greenworks.getAchievement(achievement, success_callback, [error_callback])

* `achievement` String: The achievement name in you game
* `success_callback` Function(is_achieved)
  * `is_achieved` Boolean: Whether the achievement is achieved.
* `error_callback` Function(err)

Gets whether the `achievement` is achieved.

### greenworks.clearAchievement(achievement, success_callback, [error_callback])

* `achievement` String - The achievement needs to be cleared
* `success_callback` Function()
* `error_callback` Function(err)

### greenworks.getAchievementNames()

Returns a `Array` represents all the achievements in the game.

### greenworks.getNumberOfAchievements()

Returns a `Integer` represents the number of all achievements in the game.

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
