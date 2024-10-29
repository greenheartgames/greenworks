export function acceptP2PSessionWithUser(steamId: string): void
export function isP2PPacketAvailable(nChannel?:number): number

export const enum eP2PSendType {
    Unreliable = 0,
    UnreliableNoDelay = 1,
    Reliable = 2,
    ReliableWithBuffering = 3
  }

export function sendP2PPacket(steamId: string, sendType: eP2PSendType, data: Buffer,nChannel?:number): boolean

export interface P2PPacket {
    data: Buffer
    //size: number
    steamIDRemote: string
  }
export function readP2PPacket(size: number,nChannel?:number): P2PPacket