import { Dialog, FriendFlags, FriendGameInfo, SteamID } from './Defination';

/**
 * 获取客户端知道并符合特定条件的用户人数。 （例如好友、已屏蔽、同一服务器上的用户等等）
 * @param friendFlag 好友标识位
 */
export function getFriendCount(friendFlag: FriendFlags): number


/**
 * 返回SteamID对象数组，每个SteamID表示一个朋友
 * @param friendFlag 好友标识位
 */
export function getFriends(friendFlag: FriendFlags): SteamID[]

/**
 * 请求指定用户的昵称及头像（可选）
 *
 * 注意： 下载头像相当缓慢，并且会改动本地缓存，所以如果不需要头像，请勿发出请求
 *
 * @return 返回一个bool值，
 * true 表示已请求数据，收到数据时将会发布 PersonaStateChange_t 回调。
 * false 表示已经有了该用户的详细数据，需要这些信息的函数可立即使用。
 *
 * @param steamIdUser 64位形式的SteamId，从SteamID.getRawSteamID()获得
 * @param requireNameOnly 只返回昵称（true）吗？ 还是同时返回名称和头像（false）？
 */
export function requestUserInformation(steamIdUser: string, requireNameOnly: boolean): boolean

/**
 * 为指定用户获取中型（32*32 像素）头像的integer型句柄。
 * 如果未对用户设置头像，返回 0。
 * @param steamIdFriend 64位形式的SteamId，从SteamID.getRawSteamID()获得
 */
export function getSmallFriendAvatar(steamIdFriend: string): number

/**
 * 为指定用户获取中型（64*64 像素）头像的integer型句柄。
 * 如果未对用户设置头像，返回 0。
 * @param steamIdFriend 64位形式的SteamId，从SteamID.getRawSteamID()获得
 */
export function getMediumFriendAvatar(steamIdFriend: string): number

/**
 * 为指定用户获取大型（128*128 像素）头像的integer型句柄。
 * 如果未对用户设置头像，返回 0。
 * 如果图像数据尚未载入并请求下载，返回 -1。 在此情况下，请等待 AvatarImageLoaded_t 回调，然后再次调用。
 * @param steamIdFriend 64位形式的SteamId，从SteamID.getRawSteamID()获得
 */
export function getLargeFriendAvatar(steamIdFriend: string): number

/**
 * 激活 Steam 界面，打开指定的对话框
 * @param dialog 要打开的对话框
 * @param steamId 要将此对话框打开至的上下文的 Steam ID
 */
export function activateGameOverlayToUser(dialog: Dialog, steamId: string): void

/**
 * 激活 Steam 界面，打开邀请对话框。 进入该大厅的邀请将从此窗口发出。
 * @param steamIdLobby 选定玩家将受邀进入的大厅的 Steam ID。
 */
export function activateGameOverlayInviteDialog(steamIdLobby: string): void

/**
 * 检查指定的好友是否在游戏中，若是则获取游戏的相关信息
 *
 * @param steamIDFriend 另一位用户的 Steam ID
 * @return 如果用户在游戏中，则返回详细数据。否则返回undefined
 */
export function getFriendGamePlayed(steamIDFriend: string): FriendGameInfo

/**
 * 清除当前用户所有的丰富状态（Rich Presence）键/值。
 */
export function clearRichPresence(): void

/**
 * 获取指定用户的昵称（显示名称）。这只有在另一名用户为当前用户的好友、与本地用户在同一个游戏服务器、聊天室、大厅或 Steam 小组时才能获知。
 * 注意： 第一次进入大厅、聊天室或游戏服务器时，当前用户无法自动获知其他用户的名称。该信息将通过 PersonaStateChange_t 回调异步到达。
 * 要获取当前用户的昵称，请使用 GetPersonaName。
 * 如果提供的 Steam ID 无效，或不为调用者所知，则返回空白字符串（""）或“[unknown]”。
 * @param steamIdFriend 另一位用户的 Steam ID。
 */
export function getFriendPersonaName(steamIdFriend: string): string

/**
 * 获取 Steam 好友消息中的数据。
 *
 * 此函数应只有在响应 GameConnectedFriendChatMsg_t 回调时才能调用。
 * @param rawSteamId 发出该消息的好友的 Steam ID。
 * @param messageId 消息的索引。 应为 GameConnectedFriendChatMsg_t 的 m_iMessageID 字段。
 * @param maximumMessageSize 缓冲区接收的最大消息大小
 */
export function getFriendMessage(rawSteamId: string, messageId: number, maximumMessageSize: number): string

/**
 * 获取指定好友的丰富状态值。
 * @param steamIDFriend 要获取其 Rich Presence 值的好友。
 * @param pchKey 要请求的 Rich Presence 键。
 */
export function getFriendRichPresence(steamIDFriend: string, pchKey: string): string

/**
 * 将目标用户标记为“一起玩过游戏的”。
 * @param steamIDUserPlayedWith  一起玩过游戏的另一位用户。
 */
export function setPlayedWith(steamIDUserPlayedWith: string): void

/**
 * 侦听 Steam 好友的聊天消息。
 * 您可将聊天嵌入到游戏中显示， 例如Dota2中的聊天系统。
 * 启用后，每当用户收到了聊天消息，您会收到 GameConnectedFriendChatMsg_t 回调。
 * 您可使用 GetFriendMessage 从回调中获取实际的消息数据。
 * 您可使用 ReplyToFriendMessage 来发送消息。
 * @param interceptEnabled 启用（true）或关闭（(false）拦截好友消息？
 * @return 触发一个 GameConnectedFriendChatMsg_t 回调。 始终返回 true。
 */
export function setListenForFriendsMessage(interceptEnabled: boolean): boolean

/**
 * 设置当前用户的丰富状态键/值，该键/值会自动分享给玩同一游戏的所有好友。
 * @param pchKey 要设置的丰富状态“键”。 不可比 k_cchMaxRichPresenceKeyLength 中指定的长。
 * @param pchValue 要与 pchKey 关联的丰富状态“值”。 不可比 k_cchMaxRichPresenceValueLength 中规定的长。 若设为空字符串（""）或 NULL，便会移除已设置的键。
 * @return  如果丰富状态设置成功，返回 true。 在下列情况时，返回 false： * pchKey 超过了 k_cchMaxRichPresenceKeyLength 中规定的大小，或者长度为 0。 * pchValue 超过了 k_cchMaxRichPresenceValueLength 中规定的大小。 * 用户已经达到了 k_cchMaxRichPresenceKeys 指定的丰富状态键的最大数量。
 */
export function setRichPresence(pchKey: string, pchValue: string): boolean

/**
 * 给 Steam 好友发送一则消息。
 * @param rawSteamId 要发送消息至的好友的 Steam ID。
 * @param message 待发送的 UTF-8 格式消息。
 * @return  如果消息发送成功，返回 true。 如果当前用户受到速率限制或被禁止聊天，则返回 false。
 */
export function replyToFriendMessage(rawSteamId: string, message: string): boolean
