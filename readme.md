# GreenTeaWorks

- clone from [greenworks.js](https://github.com/greenheartgames/greenworks)。
- version number sync as nw.js
- added types for typescript, clone from [here](https://www.npmjs.com/package/@wangdevops/greenworks)。
- added p2p function:
  - `acceptP2PSessionWithUser(steamId: string): void`
  - `isP2PPacketAvailable(nChannel?:number): number`
  - `sendP2PPacket(steamId: string, sendType: eP2PSendType, data: Buffer): boolean`
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

- added matching function `requestLobbyList()`
- added Event:
  - `SteamEvent.LobbyMatchList` callback: `(LobbiesMatching: number) => void` (after called `requestLobbyList`)
  - `SteamEvent.P2PSessionRequest` callback: `(steamIDRemote: string) => void` (after other player called `acceptP2PSessionWithUser`)
  - `SteamEvent.P2PSessionConnectFail` callback: `(steamIDRemote: string,eP2PSessionError:number) => void` (after connected player quit)