import {DLCData} from './Defination';

/**
 * 获取当前应用的 DLC 数量
 */
export function getDLCCount(): number

/**
 * 按索引返回 DLC 元数据
 * @param index DLC的索引
 */
export function getDLCDataByIndex(index: number): DLCData

/**
 * 检查用户是否拥有特定 DLC 且该 DLC 已安装
 * @param dlcAppId 要检查的 DLC 的 AppID
 */
export function isDLCInstalled(dlcAppId: number): boolean

/**
 * 允许您安装可选的 DLC
 * @param dlcAppId 您要安装的 DLC 的 AppID
 */
export function installDLC(dlcAppId: number): void

/**
 * 允许您卸载可选的 DLC
 * @param dlcAppId 您要卸载的 DLC 的 AppID
 */
export function uninstallDLC(dlcAppId: number): void
