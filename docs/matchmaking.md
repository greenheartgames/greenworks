# Matchmaking

## Objects

### greenworks.ChatMemberStateChange

Represents Steam SDK [`EChatMemberStateChange`](https://partner.steamgames.com/doc/api/ISteamMatchmaking#EChatMemberStateChange), describing how a users lobby state has changed.

* `Entered`
* `Left`
* `Disconnected`
* `Kicked`
* `Banned`

### greenworks.LobbyComparison

Represents Steam SDK [`ELobbyComparison`](https://partner.steamgames.com/doc/api/ISteamMatchmaking#ELobbyComparison), describing lobby search filter options.

* `EqualToOrLessThan`
* `LessThan`
* `Equal`
* `GreaterThan`
* `EqualToOrGreaterThan`
* `NotEqual`

### greenworks.LobbyDistanceFilter

Represents Steam SDK [`ELobbyDistanceFilter`](https://partner.steamgames.com/doc/api/ISteamMatchmaking#ELobbyDistanceFilter), describing lobby search distance filters when requesting the lobby list. Lobby results are sorted from closest to farthest.

* `Close`
* `Default`
* `Far`
* `Worldwide`

### greenworks.LobbyType

Represents Steam SDK [`ELobbyType`](https://partner.steamgames.com/doc/api/ISteamMatchmaking#ELobbyType), specifies the lobby type.

* `Private`
* `FriendsOnly`
* `Public`
* `Invisible`


### greenworks.Result

Represents Steam SDK [`EResult`](https://partner.steamgames.com/doc/api/steam_api#EResult), describing steam error result codes.

<details>

<summary>107 elements</summary>

* `OK`
* `Fail`
* `NoConnection`
* `InvalidPassword`
* `LoggedInElsewhere`
* `InvalidProtocolVer`
* `InvalidParam`
* `FileNotFound`
* `Busy`
* `InvalidState`
* `InvalidName`
* `InvalidEmail`
* `DuplicateName`
* `AccessDenied`
* `Timeout`
* `Banned`
* `AccountNotFound`
* `InvalidSteamID`
* `ServiceUnavailable`
* `NotLoggedOn`
* `Pending`
* `EncryptionFailure`
* `InsufficientPrivilege`
* `LimitExceeded`
* `Revoked`
* `Expired`
* `AlreadyRedeemed`
* `DuplicateRequest`
* `AlreadyOwned`
* `IPNotFound`
* `PersistFailed`
* `LockingFailed`
* `LogonSessionReplaced`
* `ConnectFailed`
* `HandshakeFailed`
* `IOFailure`
* `RemoteDisconnect`
* `ShoppingCartNotFound`
* `Blocked`
* `Ignored`
* `NoMatch`
* `AccountDisabled`
* `ServiceReadOnly`
* `AccountNotFeatured`
* `AdministratorOK`
* `ContentVersion`
* `TryAnotherCM`
* `PasswordRequiredToKickSession`
* `AlreadyLoggedInElsewhere`
* `Suspended`
* `Cancelled`
* `DataCorruption`
* `DiskFull`
* `RemoteCallFailed`
* `PasswordUnset`
* `ExternalAccountUnlinked`
* `PSNTicketInvalid`
* `ExternalAccountAlreadyLinked`
* `RemoteFileConflict`
* `IllegalPassword`
* `SameAsPreviousValue`
* `AccountLogonDenied`
* `CannotUseOldPassword`
* `InvalidLoginAuthCode`
* `AccountLogonDeniedNoMail`
* `HardwareNotCapableOfIPT`
* `IPTInitError`
* `ParentalControlRestricted`
* `FacebookQueryError`
* `ExpiredLoginAuthCode`
* `IPLoginRestrictionFailed`
* `AccountLockedDown`
* `AccountLogonDeniedVerifiedEmailRequired`
* `NoMatchingURL`
* `BadResponse`
* `RequirePasswordReEntry`
* `ValueOutOfRange`
* `UnexpectedError`
* `Disabled`
* `InvalidCEGSubmission`
* `RestrictedDevice`
* `RegionLocked`
* `RateLimitExceeded`
* `AccountLoginDeniedNeedTwoFactor`
* `ItemDeleted`
* `AccountLoginDeniedThrottle`
* `TwoFactorCodeMismatch`
* `TwoFactorActivationCodeMismatch`
* `AccountAssociatedToMultiplePartners`
* `NotModified`
* `NoMobileDevice`
* `TimeNotSynced`
* `SmsCodeFailed`
* `AccountLimitExceeded`
* `AccountActivityLimitExceeded`
* `PhoneActivityLimitExceeded`
* `RefundToWallet`
* `EmailSendFailure`
* `NotSettled`
* `NeedCaptcha`
* `GSLTDenied`
* `GSOwnerDenied`
* `InvalidItemType`
* `IPBanned`
* `GSLTExpired`
* `InsufficientFunds`
* `TooManyPending`

</details>

## Methods

### greenworks.createLobby(lobbyType, maxMembers)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#CreateLobby)

* `lobbyType` greenworks.LobbyType
* `maxMembers` Integer: The maximum number of players that can join this lobby. This can not be above 250.

Returns an `greenworks.Result`.

### greenworks.deleteLobbyData(steamIDLobby, pchKey)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#DeleteLobbyData)

* `steamIDLobby` String: The Steam ID of the lobby to delete the metadata for.
* `pchKey` String: The key to delete the data for.

Returns an `Boolean`: true if the key/value was successfully deleted; otherwise, false if steamIDLobby or pchKey are invalid.

### greenworks.getLobbyByIndex(iLobby)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyByIndex)

* `iLobby` Integer: The index of the lobby to get the Steam ID of.

Returns an `String` represents a steamIDLobby.

### greenworks.getLobbyData(steamIDLobby, pchKey)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyData)

* `steamIDLobby` String: The Steam ID of the lobby to get the metadata from.
* `pchKey` String: The key to get the value of.

Returns an `String` represents a value of a key.

### greenworks.getLobbyMemberByIndex(steamIDLobby, iMember)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyMemberByIndex)

**NOTE**: You must call `getNumLobbyMembers` before calling this.

* `steamIDLobby` String: The Steam ID of the lobby to get the metadata from.
* `iMember` Integer: An index between 0 and `getNumLobbyMembers`.

Returns an `String` represents a Steam ID of lobby member.

### greenworks.getNumLobbyMembers(steamIDLobby)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetNumLobbyMembers)

* `steamIDLobby` String: The Steam ID of the lobby to get the number of members of.

Returns an `Integer` represents a number of members in the lobby, 0 if the current user has no data from the lobby.

### greenworks.getLobbyOwner(steamIDLobby)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyOwner)

* `steamIDLobby` String: The Steam ID of the lobby to get the owner of.

Returns an `String` represents a Steam ID of the current lobby owner.

### greenworks.inviteUserToLobby(steamIDLobby, steamIDInvitee)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#InviteUserToLobby)

* `steamIDLobby` String: The Steam ID of the lobby to invite the user to.
* `steamIDInvitee` String: The Steam ID of the person who will be invited.

Returns an `Boolean`: true if the invite was successfully sent; otherwise, false if the local user isn't in a lobby, no connection to Steam could be made, or the specified user is invalid.

### greenworks.joinLobby(steamIDLobby)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#JoinLobby)

* `steamIDLobby` String: The Steam ID of the lobby to join.

Returns an `greenworks.Result`.

### greenworks.leaveLobby(steamIDLobby)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#LeaveLobby)

* `steamIDLobby` String: The lobby to leave.

### greenworks.setLobbyData(steamIDLobby, pchKey, pchValue)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyData)

* `steamIDLobby` String: The Steam ID of the lobby to set the metadata for.
* `pchKey` String: The key to set the data for. This can not be longer than [`k_nMaxLobbyKeyLength`](https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_nMaxLobbyKeyLength).
* `pchValue` String: The value to set. This can not be longer than [`k_cubChatMetadataMax`](https://partner.steamgames.com/doc/api/ISteamFriends#k_cubChatMetadataMax).

Returns an `Boolean`: true if the data has been set successfully. false if steamIDLobby was invalid, or the key/value are too long.

### greenworks.setLobbyJoinable(steamIDLobby, bLobbyJoinable)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyJoinable)

* `steamIDLobby` String: The Steam ID of the lobby.
* `bLobbyJoinable` Boolean: Enable (true) or disable (false) allowing users to join this lobby?

Returns an `Boolean`: true upon success; otherwise, false if you're not the owner of the lobby.

### greenworks.setLobbyOwner(steamIDLobby, steamIDNewOwner)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyOwner)

* `steamIDLobby` String: The Steam ID of the lobby where the owner change will take place.
* `steamIDNewOwner` Boolean: The Steam ID of the user that will be the new owner of the lobby, they must be in the lobby.

Returns an `Boolean`: true if the owner was successfully changed otherwise false if you're not the current owner of the lobby, steamIDNewOwner is not a member in the lobby, or if no connection to Steam could be made.

### greenworks.setLobbyType(steamIDLobby, eLobbyType)

[Steam docs](https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyType)

* `steamIDLobby` String: The Steam ID of the lobby to set the type of.
* `eLobbyType` greenworks.LobbyType

Returns an `Boolean`: true upon success; otherwise, false if you're not the owner of the lobby.
