## Methods

### greenworks.getStatInt(name)

* `name` String: The name of the user stat

Returns a `Integer` represents the value of the user stat.

### greenworks.getStatFloat(name)

* `name` String: The name of the user stat

Returns a `Float` represents the value of the user stat.

### greenworks.setStat(name, value)

* `name` String: The name of the user stat
* `value` Integer or Float: The value of the user stat

Returns a `Boolean` indicates whether the method succeeds.

### greenworks.storeStats(success_callback, [error_callback])

* `success_callback` Function(game_id)
  * `game_id`: The game which these stats are for
* `error_callback` Function(err)

Stores the current user stats data on the server.