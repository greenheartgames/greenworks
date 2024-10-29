export function acceptP2PSessionWithUser(steamId: string): boolean
export function isP2PPacketAvailable(nChannel:number): number

export const enum eP2PSendType {
    Unreliable = 0,
    UnreliableNoDelay = 1,
    Reliable = 2,
    ReliableWithBuffering = 3
  }

export function sendP2PPacket(steamId: string, sendType: eP2PSendType, data: Buffer,nChannel:number): boolean

export interface P2PPacket {
    data: Buffer
    steamIDRemote: string
  }
export function readP2PPacket(size: number,nChannel:number): P2PPacket


export function isBehindNAT():boolean

export function closeP2PSessionWithUser(steamIDUser: string): boolean
export function closeP2PChannelWithUser(steamIDUser: string, nChannel: number): boolean
export function getP2PSessionState(steamIDUser: string): {
  result:boolean,
  connectionState:{
    m_bConnectionActive: number,
    m_bConnecting: number,
    m_eP2PSessionError: number,
    m_bUsingRelay: number,
    m_nBytesQueuedForSend: number,
    m_nPacketsQueuedForSend: number,
    m_nRemoteIP: number,
    m_nRemotePort: number
  }
}