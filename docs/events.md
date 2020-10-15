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

### Event: 'lobby-created'

Emitted after lobby creation attempt.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyCreated_t)

Returns:
* `m_eResult` Integer: lobby creation result.
* `m_ulSteamIDLobby` String: the Steam ID of the Lobby.

### Event: 'lobby-data-update'

Emitted when lobby metadata changes.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#king#LobbyDataUpdate_t)

Returns:
* `m_ulSteamIDLobby` String: the Steam ID of the Lobby.
* `m_ulSteamIDMember` String: Steam ID of either the member whose data changed, or the room itself.
* `m_bSuccess` Boolean: whatever the lobby data was successfully changed.

### Event: 'lobby-enter'

Emitted upon attempting to enter a lobby. Lobby metadata is available to use immediately after receiving this.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#king#LobbyEnter_t)

Returns:
* `m_ulSteamIDLobby` String: the Steam ID of the Lobby.
* `m_rgfChatPermissions` Integer: unused - always 0.
* `m_bLocked` Boolean: if true, then only invited users may join.
* `m_EChatRoomEnterResponse` Integer: This is actually a [EChatRoomEnterResponse](https://partner.steamgames.com/doc/api/steam_api#EChatRoomEnterResponse) value. This will be set to k_EChatRoomEnterResponseSuccess if the lobby was successfully joined, otherwise it will be k_EChatRoomEnterResponseError.

### Event: 'lobby-invite'

Emitted on invite received.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#king#LobbyInvite_t)

Returns:
* `m_ulSteamIDUser` String: Steam ID of the person that sent the invite.
* `m_ulSteamIDLobby` String: Steam ID of the lobby we're invited to.
* `m_ulGameID` String: Game ID of the lobby we're invited to.

### Event: 'lobby-join-requested'

Emitted when the user tries to join a lobby from their friends list or from an invite. The game client should attempt to connect to specified lobby when this is received.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamFriends#GameLobbyJoinRequested_t)

Returns:
* `m_steamIDLobby` String: the Steam ID of the lobby to connect to.
* `m_steamIDFriend` String: the friend they joined through. This will be invalid if not directly via a friend.

### Event: 'rich-presence-join-requested'

Emitted when the user tries to join a game from their friends list using Rich Presence.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamFriends#GameRichPresenceJoinRequested_t)

Returns:
* `m_steamIdFriend` String: the friend they joined through. This will be invalid if not directly via a friend.
* `m_rgchConnect` String: The value associated with the "connect" Rich Presence key.

### Event: 'new-url-launch-parameters'

Posted after the user executes a steam url with command line or query parameters such as `steam://run/<appid>//?param1=value1;param2=value2;param3=value3;` while the game is already running. The new params can be queried with [GetLaunchCommandLine](https://partner.steamgames.com/doc/api/ISteamApps#GetLaunchCommandLine) and [GetLaunchQueryParam](https://partner.steamgames.com/doc/api/ISteamApps#GetLaunchQueryParam).

[Steam docs](https://partner.steamgames.com/doc/api/ISteamApps#NewUrlLaunchParameters_t)