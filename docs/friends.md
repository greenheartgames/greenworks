# Friends

Friends APIs give you the ability to access friends list data and general
information about users.

```javascript
var greenworks = require('./greenworks');

if (greenworks.initAPI()) {
  greenworks.on('persona-stage-change', function(steam_id, persona_change_flag) {
    if (persona_change_flag == greenworks.PersonaChange.Name)
      console.log("Change to new name: " + steam_id.getPersonaName());
  });
  // Get the number of regular friends.
  console.log(greenworks.getFriendCount(greenworks.FriendFlags.Immediate));
  var friends = greenworks.getFriends(greenworks.FriendFlags.Immediate));
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
* `Suggested`
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
* `Suggested`

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
* `Avator`
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

Returns an array of `SteamID` objects, each `SteamID` represents a friend.

### greeenworks.requestUserInformation(raw_steam_id, require_name_only)

* `raw_steam_id` String: a 64-bits steam ID (SteamID.getRawSteamID()).
* `require_name_only` Boolean: whether get name only.

Requests information about a user (persona name & avatar).  Returns true, it
means that data is being requested, and a `persona-state-changed` event will be
emitted when it's retrieved; if returns false, it means that we already have all
the details about that user, and functions can be called immediately.

If `require_name_only` is true, then the avatar of a user isn't downloaded
(it's a lot slower to download avatars and churns the local cache, so if you
don't need avatars, don't request them).
