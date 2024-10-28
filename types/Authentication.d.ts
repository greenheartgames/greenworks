import {ErrorCallback, SteamID} from './Defination';

export const EncryptedAppTicketSymmetricKeyLength: number;

/**
 * 获取要发送给希望对您进行身份验证的实体的ticket。
 *
 * ticket buffer 可以像 ticket.toString('hex') 这样在 Web API ISteamUserAuth/AuthenticateUserTicket 中被使用，这样就可以从游戏服务器安全的获得授权的Steam ID。
 * 如果票证未被使用，则需要使用 handle 来将ticket作废。
 *
 * 注意一旦创建ticket的进程终止了，那么生成出来的ticket就无效了。
 * 所以如果您想要在某些临时进程中创建ticket，请确保这些ticket在身份验证结束前保持有效！
 *
 * @param success_callback
 * ticket: ticket 的值。
 * handle: 该ticket返回的 handle。
 * @param error_callback
 */
export function getAuthSessionTicket(success_callback: (ticket: { ticket: Buffer, handle: number }) => void, error_callback?: ErrorCallback): void

/**
 * 将请求的session ticket 作废。
 *
 * @param ticket_handle ticket句柄
 */
export function cancelAuthTicket(ticket_handle: number): void

/**
 * 加密ticket可以用来从客户端获取认证的Steam Id，而无需向Steam的API服务器发送网络请求。
 *
 * 这些ticket可以通过加密应用的ticket秘钥来进行解密。
 * 在解密之后，可以通过Steamworks SDK中提供的加密应用ticket库，从ticket中查看用户的Steam ID、App ID、以及 VAC ban状态。
 *
 * @param user_data 将要加密到ticket中的任意值。将会以utf-8编码存储在ticket中。
 * @param success_callback
 * encrypted_ticket: 加密过的ticket。
 * @param error_callback
 */
export function getEncryptedAppTicket(user_data: string, success_callback: (encrypted_ticket: Buffer) => void, error_callback?: ErrorCallback): void

/**
 * 使用解密key可以堆加密过的应用ticket进行解密。
 * 如果解密成功则会返回一个 Buffer；否则会返回 Null。
 *
 * @param encrypted_ticket 加密过的ticket。
 * @param decryption_key 用于解密的秘钥。其长度应该是 greenworks.EncryptedAppTicketSymmetricKeyLength。
 */
export function decryptAppTicket(encrypted_ticket: Buffer, decryption_key: Buffer): Buffer

/**
 * 返回一个 Boolean 值，表示解密ticket是否适用于该应用。
 *
 * @param decrypted_ticket 解密过的ticket。
 * @param app_id 这个app的id。
 */
export function isTicketForApp(decrypted_ticket: Buffer, app_id: number): boolean

/**
 * 返回一个 Integer，表示该ticket的发布时间。
 *
 * @param decrypted_ticket 解密过的ticket。
 */
export function getTicketIssueTime(decrypted_ticket: Buffer): number

/**
 * 返回一个 SteamID 对象，表示该ticket的steam id。
 *
 * @param decrypted_ticket 解密过的ticket。
 */
export function getTicketSteamId(decrypted_ticket: Buffer): SteamID

/**
 * 返回一个 Integer ，表示该ticket的app id。
 *
 * @param decrypted_ticket 解密过的ticket。
 */
export function getTicketAppId(decrypted_ticket: Buffer): number
