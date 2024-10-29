# GreenTeaWorks 🍵



- clone from [greenworks.js](https://github.com/greenheartgames/greenworks)
- added types for typescript, clone from [here](https://www.npmjs.com/package/@wangdevops/greenworks)
- version number sync as nw.js
- added p2p function:
  - `acceptP2PSessionWithUser(steamId: string): void`
  - `isP2PPacketAvailable(nChannel?:number): number`
  - `sendP2PPacket(steamId: string, sendType: eP2PSendType, data: Buffer,nChannel?:number): boolean`
  - `readP2PPacket(size: number,nChannel?:number):{data: Buffer,steamIDRemote: string}`
- added enum `eP2PSendType`
  ```
  const enum eP2PSendType {
    Unreliable = 0,
    UnreliableNoDelay = 1,
    Reliable = 2,
    ReliableWithBuffering = 3
  }
  ```
- added enum `eChatMemberStateChange`
  ```
  const enum eChatMemberStateChange {
    Entered = 0x0001,
    Left = 0x0002,
    Disconnected = 0x0004,
    Kicked = 0x0008,
    Banned = 0x0010
  }
  ```

- added matching function `requestLobbyList()` (the return is useless same as `creataLobby`, use `SteamEvent.LobbyMatchList` to recieve the result)
- added Event:
  - `SteamEvent.LobbyMatchList` callback: `(LobbiesMatching: number)` (after called `requestLobbyList`)
  - `SteamEvent.P2PSessionRequest` callback: `(steamIDRemote: string)` (after other player called `acceptP2PSessionWithUser`)
  - `SteamEvent.P2PSessionConnectFail` callback: `(steamIDRemote: string,eP2PSessionError:number)` (after connected player quit)
  - `SteamEvent.LobbyChatUpdate` callback: `(SteamIDLobby: string, SteamIDUserChanged: string, SteamIDMakingChange: string,ChatMemberStateChange:eChatMemberStateChange)` (It's not about chat actually, It's player enter/leave etc)
- use case please reference [steamwork.js](https://github.com/ceifa/steamworks.js/blob/main/test/networking.js)