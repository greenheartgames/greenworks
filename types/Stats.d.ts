import {ErrorCallback} from './Defination';

/**
 * 为当前用户设置/更新给定统计的值。
 * todo 暂时不知道name规则，NumGames、NumWins、NumLosses这几个可以用，但是咱也不知道为啥。。。
 * @param name 统计的“API 名称”
 * @param value 统计的新值
 */
export function setStat(name: string, value: number): boolean

/**
 * 获取当前用户的当前统计值。
 * @param name 统计的“API 名称”
 */
export function getStatInt(name: string): number

/**
 * 获取当前用户的当前统计值。返回float值
 * @param name 统计的“API 名称”
 */
export function getStatFloat(name: string): number

/**
 * 将变动的统计与成就数据发送至服务器进行持久保存。
 * @param success_callback 成功回调，返回当前游戏的ID，比如480
 * @param error_callback 错误回调
 */
export function storeStats(success_callback: (gameId: any) => void, error_callback?: ErrorCallback): void

/**
 * 重置当前用户的统计状态
 * @param isResetAchievement 是否重置成就
 */
export function resetAllStats(isResetAchievement: boolean): boolean
