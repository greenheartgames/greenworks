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

### Event: 'floating-gamepad-text-input-dismissed'

Emitted when the floating keyboard invoked from ShowFloatingGamepadTextInput has been closed.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamUtils#FloatingGamepadTextInputDismissed_t)

### Event: 'lobby-match-list'

Result when requesting the lobby list(called requestLobbyList). You should iterate over the returned lobbies with GetLobbyByIndex

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyMatchList_t)

Returns:
* `LobbiesMatching` Integer: Number of lobbies that matched search criteria and we have Steam IDs for.

### Event: 'lobby-chat-update'

A lobby chat room state has changed, this is usually sent when a user has joined or left the lobby.
It's not about chat actually, It's player enter/leave etc

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatUpdate_t)

Returns:
* `SteamIDLobby` String: The Steam ID of the lobby.
* `SteamIDUserChanged` String: The user who's status in the lobby just changed - can be recipient.
* `SteamIDMakingChange` Integer: Chat member who made the change. This can be different from m_ulSteamIDUserChanged if kicking, muting, etc. For example, if one user kicks another from the lobby, this will be set to the id of the user who initiated the kick.
* `ChatMemberStateChange` eChatMemberStateChange: Bitfield of EChatMemberStateChange values.

### Event: 'lobby-chat-msg'

A chat (text or binary) message for this lobby has been received. After getting this you must use GetLobbyChatEntry to retrieve the contents of this message.

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatMsg_t)

Returns:
* `steamIDLobby` String: The Steam ID of the lobby this message was sent in.
* `steamIDUser` String: Steam ID of the user who sent this message. Note that it could have been the local user.
* `chatEntryType` Integer: Type of message received. This is actually a EChatEntryType.
* `chatID` Integer: The index of the chat entry to use with GetLobbyChatEntry, this is not valid outside of the scope of this callback and should never be stored.

### Event: 'p2p-session-request'

A user wants to communicate with us over the P2P channel via the SendP2PPacket. In response, a call to AcceptP2PSessionWithUser needs to be made, if you want to open the network channel with them.
(after other player called acceptP2PSessionWithUser)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamNetworking#P2PSessionRequest_t)

Returns:
* `steamIDRemote` String: The user who wants to start a P2P session with us.

### Event: 'p2p-session-connect-fail'

Called when packets can't get through to the specified user.
All queued packets unsent at this point will be dropped, further attempts to send will retry making the connection (but will be dropped if we fail again).
(after connected player quit)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamNetworking#P2PSessionConnectFail_t)

Returns:
* `steamIDRemote` String: User we were trying to send the packets to.
* `eP2PSessionError` Integer: Indicates the reason why we're having trouble. Actually a EP2PSessionError.
