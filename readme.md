# GreenTeaWorks 🍵



- fork from [greenworks.js](https://github.com/greenheartgames/greenworks)
- added types for typescript, clone from [here](https://www.npmjs.com/package/@wangdevops/greenworks)
- version number sync as nw.js (v0.92.0)
- added matching function:
  - `requestLobbyList()` (the return is useless same as `creataLobby`, use `SteamEvent.LobbyMatchList` to recieve the result)
  - `getLobbyMemberLimit(steamIDLobby: string): number`
  - `setLobbyMemberLimit(steamIDLobby: string,limit: number): boolean`
  - `getLobbyMemberData(steamIDLobby: string, steamIDMember: string, pchKey: string): string`
  - `setLobbyMemberData(steamIDLobby: string, pchKey: string, pchValue: string): void`
  - `getLobbyDataCount(steamIDLobby: string): number`
  - `getLobbyDataByIndex(steamIDLobby: string, index:number): {key: string, value: string}`
  - `sendLobbyChatMsg(steamIDLobby: string,data: Buffer): boolean`
  - `getLobbyChatEntry(steamIDLobby: string,chatID: number): {steamIDUser: string, data: Buffer,chatEntryType: eChatEntryType}`

- added p2p function:
  - `sendP2PPacket(steamId: string, sendType: eP2PSendType, data: Buffer,nChannel?:number): boolean`
  - `isP2PPacketAvailable(nChannel?:number): number`
  - `readP2PPacket(size: number,nChannel?:number):{data: Buffer,steamIDRemote: string}`
  - `acceptP2PSessionWithUser(steamId: string): void`
  - `getP2PSessionState(steamIDUser: string): {result:boolean,connectionState:🍵}`
  - `closeP2PSessionWithUser(steamIDUser: string): boolean`
  - `closeP2PChannelWithUser(steamIDUser: string, nChannel: number): boolean`
  - `isBehindNAT():boolean`
- added enum `eP2PSendType`,`eChatMemberStateChange`,`eChatEntryType`,`eChatMemberStateChange`

- added Event:
  - `SteamEvent.LobbyMatchList` callback: `(LobbiesMatching: number)` (after called `requestLobbyList`)
  - `SteamEvent.P2PSessionRequest` callback: `(steamIDRemote: string)` (after other player called `acceptP2PSessionWithUser`)
  - `SteamEvent.P2PSessionConnectFail` callback: `(steamIDRemote: string,eP2PSessionError:number)` (after connected player quit)
  - `SteamEvent.LobbyChatUpdate` callback: `(SteamIDLobby: string, SteamIDUserChanged: string, SteamIDMakingChange: string,ChatMemberStateChange:eChatMemberStateChange)` (It's not about chat actually, It's player enter/leave etc)
  - `SteamEvent.LobbyChatMsg` callback: `(steamIDLobby: string,steamIDUser: string,chatEntryType:eChatEntryType,chatID:number)`
- use case please reference [steam Matchmaking api](https://partner.steamgames.com/doc/api/ISteamMatchmaking) , [steam maatchNetworking api](https://partner.steamgames.com/doc/api/ISteamNetworking) 