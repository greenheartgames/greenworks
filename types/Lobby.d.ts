import {LobbyType, SteamID} from './Defination';

/**
 * 创建一个新的匹配大厅。
 *
 * @param lobbyType 此大厅的类型与可见性， 之后可通过 SetLobbyType 更改。
 * @param maxMembers 可加入此大厅的玩家最大数量。 不能超过 250 人。
 * @return 指向一个 Steam API 调用的唯一句柄。如果有函数返回其中之一，您必须使用调用结果系统记录其状态。
 */
export function createLobby(lobbyType: LobbyType, maxMembers: number): string

/**
 * 在大厅元数据中设置键/值对。 此函数可用于设置大厅名称、当前地图、游戏模式等。
 *
 * @param steamIDLobby 要设置元数据的大厅 Steam ID。可以在lobby-created回调函数中获得
 * @param pchKey 要设置数据的键。 不得长于255
 * @param pchValue 要设置的值。 不得长于8192
 * @return 返回是否设置成功
 */
export function setLobbyData(steamIDLobby: string, pchKey: string, pchValue: string): boolean

/**
 * 获取与指定大厅中的指定键相关的元数据。
 * 注意： 仅能从客户端知晓的大厅获取元数据，客户端或是从 LobbyMatchList_t 收到一个大厅列表并使用 RequestLobbyData 获取数据，或是在加入一个大厅后而知晓大厅的。
 *
 * @param steamIDLobby 要从中获得元数据的大厅的 Steam ID。
 * @param pchKey 要获取值的键。
 * @return 如果没有为此键设置值，或 steamIDLobby 无效，则返回一个空白字符串（""）。
 */
export function getLobbyData(steamIDLobby: string, pchKey: string): string

/**
 * 从大厅移除元数据键。
 * 此操作只能由大厅所有者完成。
 * 只有当键存在时，才会发送数据。 在发送数据之前会稍有延迟，因此您可以重复调用来设置您需要的所有数据，并在最后一次顺序调用之后，数据将被自动批处理和发送。
 *
 * @param steamIDLobby 待删除元数据的大厅的 Steam ID。
 * @param pchKey 待删除数据的键。
 * @return true， 表示成功删除了键/值；否则，如果 steamIDLobby 或 pchKey 无效，则为 false。
 */
export function deleteLobbyData(steamIDLobby: string, pchKey: string): boolean

/**
 * 加入一个现有大厅。
 * 可以用 RequestLobbyList 搜索并加入好友从而获取大厅的 Steam ID，也可以从邀请中获取。
 *
 * @param steamIDLobby 要加入的大厅的 Steam ID。
 * @return 指向一个 Steam API 调用的唯一句柄。如果有函数返回其中之一，您必须使用调用结果系统记录其状态。
 */
export function joinLobby(steamIDLobby: string): string

/**
 * 离开用户当前所在的大厅，这将立即在客户端生效，大厅的其他用户将得到 LobbyChatUpdate_t 回调的通知。
 *
 * @param steamIDLobby 要离开的大厅。
 */
export function leaveLobby(steamIDLobby: string): void

/**
 * 收到 RequestLobbyList 结果后，获得指定索引中大厅的 Steam ID。
 * 注意： 此函数只应在收到 LobbyMatchList_t 调用结果后调用。
 *
 * @param index 要获得其 Steam ID 的大厅索引，从 0 至 LobbyMatchList_t.m_nLobbiesMatching。
 * @return 如果所提供的索引无效或未找到大厅，返回 k_steamIDNil。
 */
export function getLobbyByIndex(index: number): SteamID

/**
 * 获得给定索引中大厅成员的 Steam ID。
 * 注意： 您在调用此函数前必需调用 GetNumLobbyMembers。
 * 注意： 当前用户必须在大厅中才能获取该大厅中其他用户的 Steam ID。
 *
 * @param steamIDLobby 这必须是与之前对 GetNumLobbyMembers 的调用中使用的相同大厅！
 * @param indexMember 在 0 与 GetNumLobbyMembers 之间的索引。
 */
export function getLobbyMemberByIndex(steamIDLobby: string, indexMember: number): SteamID

/**
 * 返回当前大厅所有者。
 * 注意： 您必须是大厅成员才能访问
 * 始终只有一位大厅所有者，如果当前所有者离开，该大厅中的另一名玩家将自动成为所有者。 在一个大厅的所有者刚离开时，其他玩家便有机会（但较少见）加入该大厅，进入大厅后，其自身便成为该大厅的所有者。
 *
 * @param steamIDLobby 要获得其所有者的大厅的 Steam ID。
 */
export function getLobbyOwner(steamIDLobby: string): SteamID

/**
 * 获得一个大厅中的用户数。
 * 注意： 当前用户必须在大厅中才能获取该大厅中其他用户的 Steam ID。
 * 用于循环访问，调用此函数后，可使用 GetLobbyMemberByIndex 来获得大厅中每个成员的 Steam ID。 通过 ISteamFriends 接口可自动收到大厅其他成员的个人信息（姓名、头像等）。
 *
 * @param steamIDLobby 要获得其成员数量的大厅的 Steam ID。
 * @return 大厅中成员的数量，如果当前用户没有来自大厅的数据，则为 0。
 */
export function getNumLobbyMembers(steamIDLobby: string): number

/**
 * 邀请其他用户进入大厅。
 * 指定用户点击了加入链接后，如果该用户在游戏中，将发送 GameLobbyJoinRequested_t 回调 ，如果游戏尚未运行，则游戏将自动通过命令行参数 +connect_lobby <64-bit lobby Steam ID> 启动。
 *
 * @param steamIDLobby 要邀请用户进入的大厅的 Steam ID。
 * @param steamIDInvitee 将被邀请的用户的 Steam ID。
 * @return true， 表示成功发送邀请；否则，如果本地用户不在大厅中，或无法连接至 Steam，或指定用户无效，则为 false。
 * 注意： 此调用不检查其他用户是否被成功邀请。
 */
export function inviteUserToLobby(steamIDLobby: string, steamIDInvitee: string): boolean

/**
 * 设置一个大厅是否对其他玩家开放。 始终默认为启用新的大厅。
 * 如果禁止加入，那么没有玩家可以加入，即便他们是好友或已受邀请。
 * 禁止加入的大厅将不会从大厅搜索中返回。
 *
 * @param steamIDLobby 大厅的 Steam ID。
 * @param lobbyJoinable 是启用（true）还是禁用（false）“允许用户加入此大厅”？
 */
export function setLobbyJoinable(steamIDLobby: string, lobbyJoinable: boolean): boolean

/**
 * 变更大厅所有者。
 * 这只能由大厅所有者设置。 这将针对大厅中的所有用户触发 LobbyDataUpdate_t，每位用户应更新其本地状态以反映新的所有者。 通常在所有者名称旁边显示一个皇冠图标。
 * 触发 LobbyDataUpdate_t 回调。
 *
 * @param steamIDLobby 要发生所有者变更的大厅的 Steam ID。
 * @param steamIDNewOwner 要成为大厅新所有者的用户的 Steam ID，该用户必须在该大厅中。
 * @return true， 表示成功变更所有者。 false， 表示您不是大厅目前的所有者，或 steamIDNewOwner 不是大厅成员，或无法连接至 Steam。
 */
export function setLobbyOwner(steamIDLobby: string, steamIDNewOwner: string): boolean

/**
 * 刷新大厅类型。
 *
 * 当使用 CreateLobby 创建大厅时也会进行此设置。
 * 它只可由大厅所有者设置。
 *
 * @param steamIDLobby 要设置类型的大厅的 Steam ID。
 * @param lobbyType 要设置的新大厅类型。
 * @return true， 表示成功；否则，如果您不是大厅所有者，则返回 false。
 */
export function setLobbyType(steamIDLobby: string, lobbyType: LobbyType): boolean

export function requestLobbyList(): string