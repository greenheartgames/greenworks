/**
 * 初始化 Steamworks API。
 *
 * @return
 * true 表示所有所需接口均已获得且可访问。
 * false 表示存在下列情况之一：
 *    Steam 客户端未运行。 需要有运行的 Steam 客户端才能提供各种 Steamworks 接口的实现。
 *    Steam 客户端无法判定游戏的 App ID。 如果您直接通过可执行文件或调试器运行您的应用程序，那么您的游戏目录中的可执行文件旁，必须有一个 steam_appid.txt，其中只记录了您的应用 ID，此外不含有任何其他内容。 Steam 将在当前工作目录中查找此文件。 如果您从不同的目录中运行可执行文件，您也许需要重新定位 steam_appid.txt 文件。
 *    您的应用程序运行的 OS 用户上下文，与 Steam 客户端并不相同，比如用户或管理员访问权限级别不同。
 *    确定您在当前活跃的 Steam 帐户中拥有该 App ID 的许可。 您的游戏必须显示在您的 Steam 库中。
 *    您的 AppID 未完全设置，如 发行状态：不可用，或缺失默认程序包。
 */
import {ErrorCallback, GameOverlayType, ImageSize, SteamID} from './Defination';

export function init(): boolean

/**
 * 返回是否成功初始化Steam API的布尔值。
 * 注意：测试时，您需要启动并登录Steam客户端，并在应用程序目录下创建一个带有Steam APP ID（或steamworks示例APP ID）的Steam_appid.txt文件。
 */
export function initAPI(): boolean

/**
 * 返回steam是否正在运行
 */
export function isSteamRunning(): boolean

/**
 * 如果你的应用程序不是通过Steam启动的，这将向Steam发出启动应用程序的信号，然后导致你的应用退出。
 * 调用restartAppIfNecessary（）后，您不会有任何损失，但如果它返回true，则您的应用程序正在重新启动。
 *
 * @param appId 游戏的APP ID
 */
export function restartAppIfNecessary(appId: number): void

/**
 * 返回表示当前正在运行的游戏的整数型id。
 */
export function getAppId(): number

/**
 * 返回一个整数，表示应用程序的内部版本id。可以根据游戏的后端更新随时更改。
 */
export function getAppBuildId(): number

/**
 * 返回表示当前用户的SteamID对象。
 */
export function getSteamId(): SteamID

/**
 * 返回一个字符串，该字符串表示Steam中专门为游戏设置的当前语言。
 */
export function getCurrentGameLanguage(): string

/**
 * 返回表示Steam UI中设置的当前语言的字符串。
 */
export function getCurrentUILanguage(): string

/**
 * 获取当前游戏安装目录，尚未实现
 */
export function getCurrentGameInstallDir(): 'NOT IMPLEMENTED'

/**
 * 返回表示应用程序安装目录的绝对路径的字符串。
 *
 * @param appId 游戏的APP ID
 */
export function getAppInstallDir(appId: number): string

/**
 * 获取玩家数量
 *
 * @param success_callback numOfPlayers Steam上当前游戏的玩家数量。
 * @param error_callback 错误回调
 */
export function getNumberOfPlayers(success_callback: (numOfPlayers: number) => void, error_callback?: ErrorCallback): void

/**
 * 打开选项对话框，激活游戏遮罩层。
 */
export function activateGameOverlay(overlayType: GameOverlayType): void

/**
 * 返回是否启用/禁用Steam遮罩层。
 */
export function isGameOverlayEnabled(): boolean

/**
 * 返回布尔值指示Steam是否处于大屏幕模式。如果应用程序不在Steam的游戏类别中，则始终返回false。
 */
export function isSteamInBigPictureMode(): boolean

/**
 * 在steam游戏遮罩层中打开指定的url。
 *
 * @param url 完整的url，例如。http://www.steamgames.com.
 */
export function activateGameOverlayToWebPage(url: string): void

/**
 * 返回一个布尔值，指示用户是否购买了该应用程序。
 *
 * @param appId 游戏的APP ID
 */
export function isSubscribedApp(appId: number): boolean

/**
 * 返回游戏当前是否已安装的布尔值。该游戏实际上可能不属于用户。
 * 仅适用于游戏本体，DLC使用的是 isDLCInstalled 。
 *
 * @param appId 游戏的APP ID
 */
export function isAppInstalled(appId: number): boolean

/**
 * 获取 Steam 图像句柄的大小。
 * 此函数必须在调用 GetImageRGBA 前调用，以创建大小合适的缓冲区，此缓冲区将由原始图像数据填充。
 *
 * @param image 要获取大小的图像句柄。
 */
export function getImageSize(image: number): ImageSize

/**
 * 从图像句柄中获取图像字节。
 *
 * @param handler 图片句柄
 */
export function getImageRGBA(handler: number): Buffer

/**
 * 返回客户端正在运行的 2 位 ISO 3166-1-alpha-2 格式的国家代码。
 * 如“US”或“UK”。
 *
 * 通过 IP 地址位置数据库来查找。
 */
export function getIPCountry(): string

/**
 * 如果游戏通过 Steam URL 如 steam://run/<appid>//<command line>/ 启动，获取命令行。
 * 此方法优于通过操作系统用命令行启动，后者可能存在安全隐患。
 * 为了使丰富状态加入能使用此方式，而不被置于操作系统命令行，您必须在您应用的“安装”>“通用安装”页面上启用“使用启动命令行”。
 *
 * @return 将命令行作为字符串返回至 pszCommandLine 提供的缓冲区，并返回复制入该缓冲区的字节数。
 */
export function getLaunchCommandLine(): number
