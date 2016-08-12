## Events

`greenworks` is an EventEmitter, which is responsible for listening Steam events.

```
var greenworks = require('greenworks');

// Required to do initialized stuff before using greenworks' APIs.
greenworks.initAPI();

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

### Event: `game-overlay-activated`

Returns:
  * `is_active` Boolean:  Indicates whether the game overlay is shown or hidden.

Emits when game overlay activates or deactivates.

### Event: 'game-servers-connected'

Emits when a game is connected to Steam server.

### Event: 'game-servers-disconnected'

Emits when a game is disconnected to Steam server.

### Event: 'game-server-connect-failure'

Emits when a game is failed to connect to Steam server.

### Event: 'steam-shutdown'

Emits when Steam client is going to shutdown.
