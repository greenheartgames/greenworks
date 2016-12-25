## Methods

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

Returns an `Array` represents all the achievements in the game.

### greenworks.getNumberOfAchievements()

Returns an `Integer` represents the number of all achievements in the game.
