## Events

`greenworks` is an EventEmitter, which is responsible for listening Steam events.

```
var greenworks = require('greenworks');

// Required to do initialized stuff before using greenworks' APIs.
greenworks.init();

function log(msg) {
  console.log(msg);
}

greenworks.on('game-overlay-activated', function(is_active) {
  if (is_active)
    log('game overlay is activated');
  else
    log('game overlay is deactivated');
});

greenworks.on('steam-servers-connected', function() { log('connected')});
greenworks.on('steam-servers-disconnected', function() { log('disconnected')});
greenworks.on('steam-server-connect-failure', function() { log('connected failure')});
greenworks.on('steam-shutdown', function() { log('shutdown')});
```

### Event: 'game-overlay-activated'

Returns:
  * `is_active` Boolean:  Indicates whether the game overlay is shown or hidden.

Emitted when game overlay activates or deactivates.

### Event: 'game-servers-connected'

Emitted when a game is connected to Steam server.

### Event: 'game-servers-disconnected'

Emitted when a game is disconnected to Steam server.

### Event: 'game-server-connect-failure'

Emitted when a game is failed to connect to Steam server.

### Event: 'steam-shutdown'

Emitted when Steam client is going to shutdown.

### Event: 'persona-state-change'

Emitted when a friends' status changes (Use with
`greenworks.requestUserInformation`).

### Event: 'avatar-image-loaded'

Emitted when a large avatar is loaded in from a previous
`getLargeFriendAvatar()` if the image wasn't already available.

### Event: 'game-connected-friend-chat-message'

Returns:
* `steam_id` String: a 64-bits steam ID.
* `message_id` Integer

Emitted when a chat message has been received from a user.

### Event: 'dlc-installed'

Returns:
* `dlc_app_id` Integer: The APPID of a DLC.

Emitted after the user gains ownership of DLC & that DLC is installed.

### Event: 'micro-txn-authorization-response'

Returns:
* `app_id` Integer: AppID for this microtransaction.
* `ord_id` String: a 64-bits OrderID provided for the microtransaction.
* `authorized` Boolean: if user authorized transaction.

Emitted after a user has responded to a microtransaction authorization request.
