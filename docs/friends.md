# Friends

Friends APIs give you the ability to access friends list data and general
information about users.

```javascript
var greenworks = require('./greenworks');

if (greenworks.init()) {
  greenworks.on('persona-state-change',
                function(steam_id, persona_change_flag) {
    if (persona_change_flag == greenworks.PersonaChange.Name)
      console.log("Change to new name: " + steam_id.getPersonaName());
  });

  greenworks.on('game-connected-friend-chat-message',
                function(steam_id, message_id) {
    var info = greenworks.getFriendMessage(steam_id.getRawSteamID(), message_id,
                                           2048);
    if (info.chatEntryType == greenworks.ChatEntryType.ChatMsg) {
      var message = info.message;
      console.log("Receive a message from " + steam_id.getPersonaName() + ": " +
                  message);
      greenworks.replyToFriendMessage(steam_id.getRawSteamID(),
                                      "Hello, I received your message.");
    }
  });

  // Listen to messages from friends.
  greenworks.setListenForFriendsMessage(true);

  // Get the number of regular friends.
  console.log(greenworks.getFriendCount(greenworks.FriendFlags.Immediate));
  var friends = greenworks.getFriends(greenworks.FriendFlags.Immediate);
  for (var i = 0; i < friends.length; ++i) {
    console.log(friends[i].getPersonaName());
    greenworks.requestUserInformation(friends[i].getRawSteamID(), true);
  }
}
```

## Objects

### greenworks.FriendFlags

Represents Steam SDK `EFriendFlags`, for enumerating friends list, or quickly
checking a the relationship between users.

* `None`
* `Blocked`
* `FriendshipRequested`
* `Immediate`
* `ClanMember`
* `OnGameServer`
* `RequestingFriendship`
* `RequestingInfo`
* `Ignored`
* `IgnoredFriend`
* `ChatMember`
* `All`


### greenworks.FriendRelationship

Represents Steam SDK `EFriendRelationship` (set of relationships to other users).

* `None`
* `Blocked`
* `RequestRecipient`
* `Friend`
* `RequestInitiator`
* `Ignored`
* `IgnoredFriend`

### greenworks.PersonaChange

Represents Steam SDK `EPersonaChange`, which is used in `persona-state-change`
event.

It describes what the client has learned has changed recently, so on startup
you'll see a name, avatar & relationship change for every friend.

* `Name`
* `Status`
* `ComeOnline`
* `GoneOffline`
* `GamePlayed`
* `GameServer`
* `Avatar`
* `JoinedSource`
* `LeftSource`
* `RelationshipChanged`
* `NameFirstSet`
* `FacebookInfo`
* `NickName`
* `SteamLevel`

### greenworks.AccountType

Represents Steam SDK `EAccountType` (Steam account types).

* `Invalid`
* `Individual`
* `Multiseat`
* `GameServer`
* `AnonymousGameServer`
* `Pending`
* `ContentServer`
* `Clan`
* `Chat`
* `ConsoleUser`
* `AnonymousUser`

### greenworks.ChatEntryType

Represents Steam SDK `EChatEntryType` (previously was only friend-to-friend
message types).

* `Invalid`
* `ChatMsg`
* `Typing`
* `InviteGame`
* `Emote`
* `LeftConversation`
* `Entered`
* `WasKicked`
* `WasBanned`
* `Disconnected`
* `HistoricalChat`
* `LinkBlocked`

### SteamID

An object represents `CSteamID`.

The `SteamID` object has following methods:

* `SteamID.isAnonymous()`

Returns whether it is an anonymous account.

* `SteamID.isAnonymousGameServer()`

Returns whether it is  an anonymous game server account id.

* `SteamID.isAnonymousGameServerLogin()`

Returns whether it is an anonymous game server login that will be filled in.

* `SteamID.isAnonymousUser()`

Returns whether it is an anonymous user account (used to create an account or
reset a password).

* `SteamID.isChatAccount()`

Returns whether it is a chat account id.

* `SteamID.isClanAccount()`

Returns whether it is a clan account id.

* `SteamID.isConsoleUserAccount()`

Returns whether it is a faked up Steam ID for a PSN friend account.

* `SteamID.isContentServerAccount()`

Returns whether it is a content server account id.

* `SteamID.isGameServerAccount()`

Returns whether it is a game server account id (either persistent or
anonymous).

* `SteamID.isIndividualAccount()`

Returns whether it is an individual user account id.

* `SteamID.isPersistentGameServerAccount()`

Returns whether it is a persistent (not anonymous) game server account id.

* `SteamID.isLobby()`

Returns whether it is a chat account id.

* `SteamID.getAccountID()`

Returns an `Integer` represents account identifier.

* `SteamID.getRawSteamID()`

Returns a `String` represents the SteamID (converts CSteamID to its 64-bit
representation).

* `SteamID.getAccountType()`

Returns a field of `greenworks.AccountType`.

* `SteamID.isValid()`

Returns whether it is a valid account.

* `SteamID.getStaticAccountKey()`

Returns a `String` represents 64-bit static account key.

It converts the static parts of a steam ID to a 64-bit representation.
For multiseat accounts, all instances of that account will have the
same static account key, so they can be grouped together by the static
account key.

* `SteamID.getPersonaName()`

Returns a `String` represents the name.

* `SteamID.getNickname()`

Returns a `String` represents the nickname which the current user has set for
the specified player. Empty if the no nickname has been set for that player.

* `SteamID.getRelationship()`

Returns a `` represents a relationship to a user.

* `SteamID.getSteamLevel()`

Returns an `Integer` represents steam level.

## Methods

### greenworks.getFriendsAccount(friend_flag)

* `friend_flag` greenworks.FriendFlags

Returns an `Integer` represents the number of friends.

### greenworks.getFriends(friend_flag)

* `friend_flag` greenworks.FriendFlags

Returns an array of [`SteamID`](friends.md#steamid) objects, each `SteamID`
represents a friend.

### greenworks.requestUserInformation(raw_steam_id, require_name_only)

* `raw_steam_id` String: a 64-bits steam ID (`SteamID.getRawSteamID()`).
* `require_name_only` Boolean: whether get name only.

Requests information about a user (persona name & avatar). Returns true, it
means that data is being requested, and a `persona-state-changed` event will be
emitted when it's retrieved; if returns false, it means that we already have all
the details about that user, and functions can be called immediately.

If `require_name_only` is true, then the avatar of a user isn't downloaded
(it's a lot slower to download avatars and churns the local cache, so if you
don't need avatars, don't request them).

### greenworks.getSmallFriendAvatar(raw_steam_id)

* `raw_steam_id` String: a 64-bits steam ID (`SteamID.getRawSteamID()`).

Gets the small (32x32) avatar. Returns an integer handle which is used in
`getImageRGBA()`; returns 0 if none set.

### greenworks.getMediumFriendAvatar(raw_steam_id)

* `raw_steam_id` String: a 64-bits steam ID (`SteamID.getRawSteamID()`).

Gets the medium (64*64) avatar. Returns an integer handle which is used in
`getImageRGBA()`; returns 0 if none set.

### greenworks.getLargeFriendAvatar(raw_steam_id)

* `raw_steam_id` String: a 64-bits steam ID (`SteamID.getRawSteamID()`).

Gets the large (128*128) avatar. Returns an integer handle which is used in
`getImageRGBA()`; returns 0 if none set; returns -1 if this image has yet to be
loaded, in this case you should wait for `avatar-image-loaded` event.

### greenworks.setListenForFriendsMessage(intecept_enabled)

* `intercept_enabled` Boolean

Listen for friends message event (`game-connected-friend-chat-message`).
Return a `Boolean` indicates whether the listener is set successfully.

### greenworks.replyToFriendMessage(raw_steam_id, message)

* `raw_steam_id` String: a 64-bits steam ID.
* `message` String: a message being sent to a friend.

Send a message to a friend. Returns a `Boolean` indicates whether the message
is sent successfully.

### greenworks.getFriendMessage(raw_steam_id, message_id, maximum_message_size)

* `raw_steam_id` String: a 64-bits steam ID.
* `message_id` Integer
* `maximum_message_size` Integer

Returns a `String` represents a message from a friend.

### greenworks.getFriendPersonaName(raw_steam_id)

* `raw_steam_id` String: a 64-bits steam ID.

Returns a `String` represents a specified user's persona (display) name.

### greenworks.setRichPresence(pchKey, pchValue)

* `pchKey` String: the rich presence 'key' to set. This can not be longer than specified in [k_cchMaxRichPresenceKeyLength](https://partner.steamgames.com/doc/api/ISteamFriends#k_cchMaxRichPresenceKeyLength).
* `pchValue` String: the rich presence 'value' to associate with `pchKey`. This can not be longer than specified in [k_cchMaxRichPresenceValueLength](https://partner.steamgames.com/doc/api/ISteamFriends#k_cchMaxRichPresenceValueLength).
If this is set to an empty string ("") or NULL then the key is removed if it's set.

Returns a `Boolean`:

`true` if the rich presence was set successfully.

`false` under the following conditions:
* `pchKey` was longer than `k_cchMaxRichPresenceKeyLength` or had a length of 0.
* `pchValue` was longer than `k_cchMaxRichPresenceValueLength`.
* The user has reached the maximum amount of rich presence keys as defined by [k_cchMaxRichPresenceKeys](https://partner.steamgames.com/doc/api/ISteamFriends#k_cchMaxRichPresenceKeys).

### greenworks.ClearRichPresence() 

Clears all of the current user's Rich Presence key/values.

### greenworks.getFriendRichPresence(steamIDFriend, pchKey)

* `steamIDFriend` String: a 64-bits steam ID.
* `pchKey` String: the Rich Presence key to request.

Returns `String` representing Rich Presence value from a specified friend.

### greenworks.setPlayedWith(steamIDUserPlayedWith)

Mark a target user as 'played with'.

* `steamIDUserPlayedWith` String: a 64-bits steam ID of other user that we have played with.

### greenworks.getFriendGamePlayed(steamIDFriend)

* `steamIDFriend` String: a 64-bit Steam ID.

Returns an object containing [FriendGameInfo_t](https://partner.steamgames.com/doc/api/ISteamFriends#FriendGameInfo_t) values if the user is a friend and in a game, otherwise returns `undefined`.

### greenworks.activateGameOverlayInviteDialog(steamIDLobby)

Activates the Steam Overlay to open the invite dialog. Invitations sent from this dialog will be for the provided lobby.

* `steamIDLobby` String: the Steam ID of the lobby that selected users will be invited to.

### greenworks.activateGameOverlayToUser(pchDialog, CSteamID steamID)

Activates Steam Overlay to a specific dialog.

* `pchDialog` String: the dialog to open.
* `steamID` String: the Steam ID of the context to open this dialog to.

Valid pchDialog options are:
* "steamid" - Opens the overlay web browser to the specified user or groups profile.
* "chat" - Opens a chat window to the specified user, or joins the group chat.
* "jointrade" - Opens a window to a Steam Trading session that was started with the [ISteamEconomy/StartTrade](https://partner.steamgames.com/doc/webapi/ISteamEconomy#StartTrade) Web API.
* "stats" - Opens the overlay web browser to the specified user's stats.
* "achievements" - Opens the overlay web browser to the specified user's achievements.
* "friendadd" - Opens the overlay in minimal mode prompting the user to add the target user as a friend.
* "friendremove" - Opens the overlay in minimal mode prompting the user to remove the target friend.
* "friendrequestaccept" - Opens the overlay in minimal mode prompting the user to accept an incoming friend invite.
* "friendrequestignore" - Opens the overlay in minimal mode prompting the user to ignore an incoming friend invite.
