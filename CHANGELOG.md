## 2025.03.31 v0.20.0 stable

* Update to Steamworks SDK 1.62
* Greenworks compiled for NW.js v0.98.0

## 2024.12.08 v0.19.0 stable

* Update to Steamworks SDK 1.61
* Greenworks compiled for NW.js v0.94.0
* New lobby and p2p APIs, #335 (thanks to @Hocti):
  - `greenworks.requestLobbyList()`
  - `greenworks.getLobbyMemberLimit(steamIDLobby: string): number`
  - `greenworks.setLobbyMemberLimit(steamIDLobby: string,limit: number): boolean`
  - `greenworks.getLobbyMemberData(steamIDLobby: string, steamIDMember: string, pchKey: string): string`
  - `greenworks.setLobbyMemberData(steamIDLobby: string, pchKey: string, pchValue: string): void`
  - `greenworks.getLobbyDataCount(steamIDLobby: string): number`
  - `greenworks.getLobbyDataByIndex(steamIDLobby: string, index:number): {key: string, value: string}`
  - `greenworks.sendLobbyChatMsg(steamIDLobby: string,data: Buffer): boolean`
  - `greenworks.getLobbyChatEntry(steamIDLobby: string,chatID: number): {steamIDUser: string, data: Buffer,chatEntryType: eChatEntryType}`
  - `greenworks.sendP2PPacket(steamId: string, sendType: eP2PSendType, data: Buffer,nChannel:number): boolean`
  - `greenworks.isP2PPacketAvailable(nChannel:number): number`
  - `greenworks.readP2PPacket(size: number,nChannel:number):{data: Buffer,steamIDRemote: string}`
  - `greenworks.acceptP2PSessionWithUser(steamId: string): void`
  - `greenworks.getP2PSessionState(steamIDUser: string): {result:boolean,connectionState:Object}`
  - `greenworks.closeP2PSessionWithUser(steamIDUser: string): boolean`
  - `greenworks.closeP2PChannelWithUser(steamIDUser: string, nChannel: number): boolean`
  - `greenworks.isBehindNAT():boolean`
* New events:
  - `lobby-match-list`
  - `lobby-chat-update`
  - `lobby-chat-msg`
  - `p2p-session-request`
  - `p2p-session-connect-fail`

## 2024.10.17 v0.18.0 stable

* Greenworks compiled for NW.js v0.92.0

## 2024.07.27 v0.17.0 stable

* Update to Steamworks SDK 1.60
* Greenworks compiled for NW.js v0.89.0
* Add `greenworks.showFloatingGamepadTextInput` API and `floating-gamepad-text-input-dismissed` event

## 2024.04.07 v0.16.0 stable

* Update to Steamworks SDK 1.59
* Greenworks copmiled for NW.js v0.86.0
* Add `greenworks.activateGameOverlayToStore` API #280

## 2023.11.04 v0.15.0 stable

* Update to Steamworks SDK 1.58
* Greenworks copmiled for NW.js v0.82.0
* Greenworks addon on mac is a universal binary (x86_64 and arm64)
* New APIs:
  * `greenworks.isSteamRunningOnSteamDeck()`
  * `greenworks.indicateAchievementProgress(achievement, current, max)`
  * `greenworks.getFriendGamePlayed(steamIDFriend)`
  * `greenworks.getLaunchCommandLine()`
  * `greenworks.getFriendPersonaName(raw_steam_id)`
  * `greenworks.setRichPresence(pchKey, pchValue)`
  * `greenworks.ClearRichPresence()`
  * `greenworks.getFriendRichPresence(steamIDFriend, pchKey)`
  * `greenworks.setPlayedWith(steamIDUserPlayedWith)`
  * `greenworks.activateGameOverlayInviteDialog(steamIDLobby)`
  * `greenworks.activateGameOverlayToUser(pchDialog, CSteamID steamID)`
  * `greenworks.createLobby(lobbyType, maxMembers)`
  * `greenworks.deleteLobbyData(steamIDLobby, pchKey)`
  * `greenworks.getLobbyByIndex(iLobby)`
  * `greenworks.getLobbyData(steamIDLobby, pchKey)`
  * `greenworks.getLobbyMemberByIndex(steamIDLobby, iMember)`
  * `greenworks.getNumLobbyMembers(steamIDLobby)`
  * `greenworks.getLobbyOwner(steamIDLobby)`
  * `greenworks.inviteUserToLobby(steamIDLobby, steamIDInvitee)`
  * `greenworks.joinLobby(steamIDLobby)`
  * `greenworks.leaveLobby(steamIDLobby)`
  * `greenworks.setLobbyData(steamIDLobby, pchKey, pchValue)`
  * `greenworks.setLobbyJoinable(steamIDLobby, bLobbyJoinable)`
  * `greenworks.setLobbyOwner(steamIDLobby, steamIDNewOwner)`
  * `greenworks.setLobbyType(steamIDLobby, eLobbyType)`
  * `greenworks.ugcGetItemState(published_file_id)`
  * `greenworks.ugcGetItemInstallInfo(published_file_id)`
  * `greenworks.getIPCountry()`
  * `greenworks.isSteamInBigPictureMode()`
  * `greenworks.getDLCDataByIndex(index)`
  * `greenworks.getAppBuildId()`
  * `greenworks.isAppInstalled(appId)`
  * `greenworks.getAppInstallDir(app_id, buffer, buffer_size)`
* New events:
  * `new-url-launch-parameters`
  * `rich-presence-join-requested`
  * `lobby-created`
  * `lobby-data-update`
  * `lobby-enter`
  * `lobby-invite`
  * `lobby-join-requested`
* Fix incorrect index on `canelAuthticket` API
* Fix "Error on saving file on local machine" bug, #178
* Fix `requestUserInformation` API not returning result

## 2018.11.18 v0.14.0 stable

* Greenworks complied for for NW.js v0.31.5, v0.32.4, v0.33.3 and Electron v3.0.9, v4.0.0-beta 7 with Steamworks SDK 1.42
* Fix an infinite loop in ugcGetUserItems, #203 (thanks to @Emad88)

## 2018.05.16 v0.13.0 stable

* Greenworks complied for NW.js v0.27.5, v0.28.3, v0.29.4, v0.30.4 and Electron v2.0.0 with Steamworks SDK 1.42
* Extend workshop APIs:
  *  Add `options` parameter to  `publishWorkshopFile`, `updatePublishedWorkshopFile`, `ugcGetItems`, `ugcGetUserItems`, and `ugcSynchronizeItems`
* Add APIs for enumerating files on cloud: `getFileCount` and `getFileNameAndSize`

## 2017.12.02 v0.12.0 stable

* Greenworks complied for NW.js v0.24.4, v0.25.4 & v0.26.6 with Steamworks SDK 1.41

## 2017.09.01 v0.11.0 stable

* Greenworks complied for NW.js v0.22.3 & Electron v1.8.0 Beta with Steamworks SDK 1.41
* Add `micro-txn-authorization-response event`, thanks to @MadSpyxFR

## 2017.5.28 v0.10.0 stable

* Greenworks complied for NW.js v0.22.3 & Electron 1.7.2 Beta with Steamworks SDK 1.40
* Add APIs for setting user stats:
  * `greenworks.getStatInt(name)`
  * `greenworks.getStatFloat(name)`
  * `greenworks.setStat(name, value)`
  * `greenworks.storeStats(success_callback, [error_callback])`
* Fix a potential crash of `getNickname()`
* Fix `ugcGetItems` returns an empty array in some scenarios

## 2017.3.3 v0.9.0 stable

* Greenworks complied for NW.js v0.20.3 & Electron 1.6.1 with Steamworks SDK 1.39
* Update to Steamworks SDK 1.39, thanks to @MadSpyxFR:
   * `greenworks.FriendFlags.Suggested` has been removed
* Add DLC APIs, #122:
   * `greenworks.getDLCCount()`
   * `greenworks.isDLCInstalled(dlc_app_id)`
   * `greenworks.installDLC(dlc_app_id)`
   * `greenworks.uninstallDLC(dlc_app_id)`

## 2016.12.31 v0.8.0 stable

* Greenworks complied for NW.js v0.18.8 & v0.19.4 with Steamworks SDK 1.38a
* Greenworks now requires `libsdkencryptedappticket` library. Plese copy it to
  the directory of your game.
* Update to Steamworks SDK 1.38a, thanks to @MadSpyxFR, some changes from
  Steamworks SDK:
  * SteamFriends `suggest` relationship type now is deprecated
  * `greenworks.getCloudQuota` returns a `String` representing 64 bits integer
* Add `greenworks.deleteFile` API, thanks to @dfabulich
* Add `greenworks.getAppId` API
* Add APIs for ticket decryption:
  * The ticket in `greenworks.getAuthSessionTicket` and
    `greenworks.getEncryptedAppTicket` callback is a `Buffer` type
  * `greenworks.decryptAppTicket`
  * `greenworks.isTicketForApp`
  * `greenworks.getTicketIssueTime`
  * `greenworks.getTicketSteamId`
  * `greenworks.getTicketAppId`
* Fix: potential issues causing by mismatched new/delete usage

## 2016.9.16 v0.7.0 stable

* Greenworks complied for NW.js v0.17.3 with Steamworks SDK 1.37
* Greenworks complied for Electron v1.4.0 with Steamworks SDK 1.37
* Add getFriendAvatar APIs, #89:
   * `avatar-image-loaded` event.
   * `greenworks.getSmallFriendAvatar(raw_steam_id)`
   * `greenworks.getMediumFriendAvatar(raw_steam_id)`
   * `greenworks.getLargeFriendAvatar(raw_steam_id)`
   * `greenworks.getImageSize(handle)`
   * `greenworks.getImageRGBA(handle)`
* Add P2P chat APIs, #91:
   * `game-connected-friend-chat-message` event
   * `greenworks.setListenForFriendsMessage(intecept_enabled)`
   * `greenworks.replyToFriendMessage(raw_steam_id, message)`
   * `greenworks.getFriendMessage(raw_steam_id, message_id, maximum_message_size)`

## 2016.8.15 v0.6.0 stable

* Greenworks complied for NW.js v0.16.1 with Steamworks SDK 1.37
* Greenworks complied for Electron v1.3.3 with Steamworks SDK 1.37
* API documents are moved from wiki to [docs](https://github.com/greenheartgames/greenworks/tree/master/docs)
* Add new friends APIs (Thanks for @marwanhilmi):
   * `persona-state-change` event
   * `greenworks.getFriendsAccount(friend_flag)`
   * `greenworks.getFriends(friend_flag)`
   * `greeenworks.requestUserInformation(raw_steam_id, require_name_only)`
* `greenworks.getSteamId()` now returns an object of `SteamID`

## 2016.07.23 v0.5.3 stable

* Greenworks complied for NW.js v0.14.7 with Steamworks SDK 1.37
* Add new APIs:
  * `greeworks.init`
  * `greenworks.restartAppIfNecessary`
  * `greenworks.isSubscribedApp`
  * `greenworks.isSteamRunning`

## 2016.02.28 v0.5.2 stable

* Greenworks complied for NW.js v0.13.0-beta7 with Steamworks SDK 1.36

## 2015.09.12 v0.5.1 stable

* Fix a crash issue when emitting `game-overlay-activated` event on Windows/Linux.

## 2015.09.07 v0.5.0 stable

* Greenworks compiled for NW.js v0.12.1 with Steamworks SDK 1.34
* Upgrade nan to v2 to support iojs v3.
* Add achievements and authentication APIs and support listening steam events:
  * greenworks.getNumberOfAchievements
  * greenworks.getAchievement
  * greenworks.clearAchievement
  * greenworks.getAchievementNames
  * greenworks.getAuthSessionTicket
  * greenworks.getEncryptedAppTicket
  * greenworks.cancelAuthTicket
  * greenworks.activateGameOverlayToWebPage
  * greenworks.on('game-overlay-activated')
  * greenworks.on('steam-servers-connected')
  * greenworks.on('steam-servers-disconnected')
  * greenworks.on('steam-servers-connect-failure')
  * greenworks.on('steam-shutdown')

## 2015.03.26 v0.4.1 stable

* Upgrade nan module to support node v0.12 as well as iojs
* Greenworks complied for NW.js(formerly node-webkit) v0.12.0 with Steamworks SDK 1.30

## 2014.10.24 v0.4.0 stable

* Greenworks complied for node-webkit v0.8.6 and v0.11.2 with Steamworks SDK 1.30
* Add activateGameOverlay and IsGameOverlayEnabled APIs
* Fix: a segment fault when zipOpenNewFileInZip4_64 fails

## 2014.10.24 v0.3.0 stable

* Greenworks complied for node-webkit v0.8.6 and v0.10.5 with Steamworks SDK 1.30
* All Greenworks APIs(cloud APIs, workshop APIs, utils APIs, Steam Info APIs) are totally rewritten with [NAN](https://github.com/rvagg/nan) module, supports node v0.10.X and node v0.11.X

## 2014.10.10 v0.3.0-alpha

* Greenworks APIs compiled for node-webkit v0.8.6 and v0.10.5
* Greenworks core methods(see below) are rewritten with `Nan` module, supports node v0.10.X and v0.11.X
  * initAPI
  * getSteamId
  * saveTextToFile
  * readTextFromFile
  * isCloudEnabled
  * isCloudEnabledForUser
  * enableCloud
  * getCloudQuota
  * activateAchievement
  * getCurrentGameLanguage
  * getCurrentUILanguage
  * getCurrentGameInstallDir
  * getNumberOfPlayers
  * fileShare
  * Utils.move

## 2014.08.11 v0.2.0

* Greenworks compiled for node-webkit v0.8.6 with Steamworks SDK 1.30
* Add Steamworks workshop APIs, such as synchronization, download, publish, update, unsubscribe or getting the Steam ID
* Add Utils APIs, like zip, unzip, write to console and/or log file

## 2013.12.02 v0.1.0

* Greenworks compiled for node-webkit v0.8.4 with Steamworks SDK 1.30
