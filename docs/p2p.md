### added p2p function:
  - `greenworks.sendP2PPacket(steamId: string, sendType: eP2PSendType, data: Buffer,nChannel:number): boolean`
  - `greenworks.isP2PPacketAvailable(nChannel:number): number`
  - `greenworks.readP2PPacket(size: number,nChannel:number):{data: Buffer,steamIDRemote: string}`
  - `greenworks.acceptP2PSessionWithUser(steamId: string): void`
  - `greenworks.getP2PSessionState(steamIDUser: string): {result:boolean,connectionState:Object}`
  - `greenworks.closeP2PSessionWithUser(steamIDUser: string): boolean`
  - `greenworks.closeP2PChannelWithUser(steamIDUser: string, nChannel: number): boolean`
  - `greenworks.isBehindNAT():boolean`

### added enum `eP2PSendType` on types

### added event `p2p-session-request`,`p2p-session-connect-fail`

[Steam docs](https://partner.steamgames.com/doc/api/ISteamNetworking)