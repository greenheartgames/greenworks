import {ErrorCallback} from './Defination';

/**
 * 激活成就
 * @param achievement 成就名
 * @param success_callback 成功回调
 * @param error_callback 错误回调
 */
export function activateAchievement(achievement: string, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 为用户弹出一个带有当前成就进度的通知。
 * @param achievement 成就名
 * @param current 当前进度
 * @param max 成就解锁所需的进度
 */
export function indicateAchievementProgress(achievement: string, current: number, max: number): boolean

/**
 * 获取成就的解锁状态。
 * @param achievement 成就名
 * @param success_callback 成功回调，返回该成就是否已经解锁。
 * @param error_callback 错误回调
 */
export function getAchievement(achievement: string, success_callback: (r: boolean) => void, error_callback?: ErrorCallback): void

/**
 * 取消成就
 * @param achievement 成就名
 * @param success_callback 成功回调
 * @param error_callback 错误回调
 */
export function clearAchievement(achievement: string, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 获取所有成就名
 */
export function getAchievementNames(): string[]

/**
 * 获取成就数量
 */
export function getNumberOfAchievements(): number
