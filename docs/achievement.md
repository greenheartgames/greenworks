## Methods

### greenworks.activateAchievement(achievement, success_callback, [error_callback])

* `achievement` String
* `success_callback` Function()
* `error_callback` Function(err)

The `achievement` represents the unlocked achievement in your game.

### greenworks.indicateAchievementProgress(achievement, current, max)

Shows the user a pop-up notification with the current progress of an achievement.
Calling this function will NOT set the progress or unlock the achievement, use [SetStat](stats.md#greenworkssetstatname-value).

* `achievement` String: API name of the achievement.
* `current` Number: The current progress.
* `max` Number: The progress required to unlock the achievement.

Returns `true` upon success, otherwise `false`.

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

Returns an `Array` represents all the achievements in the game.

### greenworks.getNumberOfAchievements()

Returns an `Integer` represents the number of all achievements in the game.
