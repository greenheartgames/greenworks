import {ChatRoomEnterResponse, ErrorCallback, Result, SteamEvent} from './Defination';

/**
 * 当游戏覆盖层激活或隐藏时触发
 *
 * @param eventKey game-overlay-activated
 * @param success_callback
 * is_active: 表示游戏覆盖层是激活还是隐藏
 * @param error_callback
 */
export function on(eventKey: SteamEvent.GameOverlayActivated, success_callback: (is_active: boolean) => void, error_callback?: ErrorCallback): void

/**
 * 当游戏链接到Steam服务器时触发
 *
 * @param eventKey game-servers-connected
 * @param success_callback
 * @param error_callback
 */
export function on(eventKey: SteamEvent.GameServersConnected, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 当游戏从Steam服务器断开连接时触发
 *
 * @param eventKey game-servers-disconnected
 * @param success_callback
 * @param error_callback
 */
export function on(eventKey: SteamEvent.GameServersDisconnected, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 当游戏连接Steam服务器失败时触发
 *
 * @param eventKey game-server-connect-failure
 * @param success_callback
 * @param error_callback
 */
export function on(eventKey: SteamEvent.GameServerConnectFailure, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 当Steam客户端即将关闭时触发
 *
 * @param eventKey steam-shutdown
 * @param success_callback
 * @param error_callback
 */
export function on(eventKey: SteamEvent.SteamShutdown, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 当一个朋友的状态改变时（与 greenworks.requestUserInformation 一起使用）触发
 *
 * @param eventKey persona-state-change
 * @param success_callback
 * @param error_callback
 */
export function on(eventKey: SteamEvent.PersonaStateChange, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 如果图像还不可用，那么在上一个使用 getLargeFriendAvatar() 加载一个头像大图时触发。
 *
 * @param eventKey avatar-image-loaded
 * @param success_callback
 * @param error_callback
 */
export function on(eventKey: SteamEvent.AvatarImageLoaded, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 当用户收到聊天消息时触发
 *
 * @param eventKey game-connected-friend-chat-message
 * @param success_callback
 * steam_id: 一个64位steam ID.
 * message_id: 消息id
 * @param error_callback
 */
export function on(eventKey: SteamEvent.GameConnectedFriendChatMessage, success_callback: (steam_id: string, message_id: number) => void, error_callback?: ErrorCallback): void

/**
 * 当用户获得DLC的所有权，或者是安装了DLC之后触发
 *
 * @param eventKey dlc-installed
 * @param success_callback
 * dlc_app_id: DLC的APPID
 * @param error_callback
 */
export function on(eventKey: SteamEvent.DlcInstalled, success_callback: (dlc_app_id: number) => void, error_callback?: ErrorCallback): void

/**
 * 当用户响应微交易授权请求后触发
 *
 * @param eventKey micro-txn-authorization-response
 * @param success_callback
 * app_id: 此次微交易的AppID。
 * ord_id: 为此次微交易提供的的64位OrderID。
 * authorized: 用户是否授权了交易。
 * @param error_callback
 */
export function on(eventKey: SteamEvent.MicroTxnAuthorizationResponse, success_callback: (app_id: number, ord_id: string, authorized: boolean) => void, error_callback?: ErrorCallback): void

/**
 * 我们请求创建大厅的结果。 此时，该大厅已有人加入且可用，并将收到 LobbyEnter_t 回调（由于本地用户正在加入他们自己的大厅）。
 *
 * @param eventKey lobby-created
 * @param success_callback
 *
 * Result可能值：
 * OK - 该大厅已成功创建。
 * Fail - 服务器已响应，但出现未知的内部错误。
 * Timeout - 消息已发送至 Steam 服务器，但尚未响应。
 * LimitExceeded - 您的游戏客户端创建了过多大厅，并受到速率限制。
 * AccessDenied - 您的游戏未设置允许创建大厅，或您的客户端无权玩该游戏。
 * NoConnection - 您的 Steam 客户端无法连接至后端。
 *
 * SteamIDLobby:所创建大厅的 Steam ID，若创建失败则为 0。
 * @param success_callback
 * @param error_callback 错误回调
 */
export function on(eventKey: SteamEvent.LobbyCreated, success_callback: (result: Result, steamIDLobby: string) => void, error_callback?: ErrorCallback): void

/**
 * 大厅元数据已变更。
 *
 * @param eventKey lobby-data-update
 * @param success_callback
 * steamIDLobby: 大厅的 Steam ID。
 * steamIDMember:  数据已更改的成员或房间自身的 Steam ID。
 * success: 如果大厅数据成功变更，则为 true，否则为 false。
 * @param error_callback 错误回调
 */
export function on(eventKey: SteamEvent.LobbyDataUpdate, success_callback: (steamIDLobby: string, steamIDMember: string, success: boolean) => void, error_callback?: ErrorCallback): void

/**
 * 在试图进入一个大厅时收到。 收到后，可以立即使用大厅元数据。
 *
 * @param eventKey lobby-enter
 * @param success_callback
 * steamIDLobby: 您已进入的大厅的 Steam ID。
 * chatPermissions: 未使用 - 始终为 0。
 * locked: 如果为 true，则仅有受邀用户可以加入。
 * chatRoomEnterResponse: 这实际上是一个 EChatRoomEnterResponse 值。 如果成功加入该大厅，将设置为 k_EChatRoomEnterResponseSuccess；否则，为 k_EChatRoomEnterResponseError。
 * @param error_callback 错误回调
 */
export function on(eventKey: SteamEvent.LobbyEnter, success_callback: (steamIDLobby: string, chatPermissions: number, locked: boolean, chatRoomEnterResponse: ChatRoomEnterResponse) => void, error_callback?: ErrorCallback): void

/**
 * 收到邀请时触发
 *
 * @param eventKey lobby-invite
 * @param success_callback
 * m_ulSteamIDUser: 发送邀请的人的Steam ID。
 * m_ulSteamIDLobby: 邀请我们的Steam ID。
 * m_ulGameID: 邀请我们的大厅的Game ID。
 * @param error_callback
 */
export function on(eventKey: SteamEvent.LobbyInvite, success_callback: (steamIDUser: string, steamIDLobby: string, gameID: string) => void, error_callback?: ErrorCallback): void

/**
 * 当用户从朋友列表或被邀请后尝试加入大厅时触发。 当连接到指定大厅的时候，游戏客户端会收到这条消息。
 *
 * @param eventKey lobby-join-requested
 * @param success_callback
 * m_steamIDLobby: 要连接到的大厅的Steam ID。
 * m_steamIDFriend: 他们是通过那个朋友加入的。如果不是直接通过朋友加入则会无效。
 * @param error_callback
 */
export function on(eventKey: SteamEvent.LobbyJoinRequested, success_callback: (steamIDLobby: string, steamIDFriend: string) => void, error_callback?: ErrorCallback): void

/**
 * 当用户尝试从好友列表中加入游戏时触发，其使用的是富消状态息。
 *
 * @param eventKey rich-presence-join-requested
 * @param success_callback
 * m_steamIdFriend: 他们是通过那个朋友加入的。如果不是直接通过朋友加入则会无效。
 * m_rgchConnect: 与 "连接" 富消状态息key关联的值。
 * @param error_callback
 */
export function on(eventKey: SteamEvent.RichPresenceJoinRequested, success_callback: (steamIdFriend: string, rgchConnect: string) => void, error_callback?: ErrorCallback): void

/**
 * 在游戏已经运行时，当用户使用命令行执行steam url或查询参数（如 steam://run/<appid>//?param1=value1;param2=value2;param3=value3; ）后触发。
 *
 * 新的参数可以使用 GetLaunchCommandLine 以及 GetLaunchQueryParam 查询。
 *
 * @param eventKey new-url-launch-parameters
 * @param success_callback
 * @param error_callback
 */
export function on(eventKey: SteamEvent.NewUrlLaunchParameters, success_callback: () => void, error_callback?: ErrorCallback): void



export function on(eventKey: SteamEvent.LobbyMatchList, success_callback: (LobbiesMatching: number) => void, error_callback?: ErrorCallback): void
export function on(eventKey: SteamEvent.P2PSessionRequest, success_callback: (steamIDRemote: string) => void, error_callback?: ErrorCallback): void
export function on(eventKey: SteamEvent.P2PSessionConnectFail, success_callback: (steamIDRemote: string,eP2PSessionError:number) => void, error_callback?: ErrorCallback): void