import {ErrorCallback, FileNameAndSize} from './Defination';

/**
 * 将文本保存到文件
 *
 * @param file_name 文件名
 * @param file_content 文件内容
 * @param success_callback
 * @param error_callback
 */
export function saveTextToFile(file_name: string, file_content: string, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 从文件中读取文本
 *
 * @param file_name 文件名
 * @param success_callback
 * file_content: 代表对应 file_name 文件的文本内容。
 * @param error_callback
 */
export function readTextFromFile(file_name: string, success_callback: (file_content: string) => void, error_callback?: ErrorCallback): void

/**
 * 删除文件
 *
 * @param file_name 文件名
 * @param success_callback
 * @param error_callback
 */
export function deleteFile(file_name: string, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 将多个文件保存到Steam云服务
 *
 * @param file_path 文件在本地的路径。
 * @param success_callback
 * @param error_callback
 */
export function saveFilesToCloud(file_path: string, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 返回一个 Boolean 值，表示当前Steam账户是否开启了云服务。
 */
export function isCloudEnabledForUser(): boolean

/**
 * 返回一个 Boolean 值，表示当前应用是否支持云服务。
 *
 * 该结果与 greenworks.isCloudEnabledForUser() 无关。
 * 请记住，用户对应用特殊设定的优先级高于全局默认的设置。
 * 因此，您可能需要首先检查 isCloudEnabledForUser()。
 */
export function isCloudEnabled(): boolean

/**
 * 开启或关闭当前应用的云服务功能。
 *
 * 请记住，如果您的应用程序在最顶层关闭了它，那么应用将不会将任何内容同步Steam云（请参阅  greenworks.isCloudEnabledForUser()）。
 * @param flag
 */
export function enableCloud(flag: boolean): void

/**
 * 获取云服务配额
 *
 * @param success_callback
 * total_bytes:  配额总字节数
 * available_bytes: 配额的最大可用字节数
 * @param error_callback
 */
export function getCloudQuota(success_callback: (total_bytes: number, available_bytes: number) => void, error_callback?: ErrorCallback): void

/**
 * 获取Steam云服务中的文件数量。
 */
export function getFileCount(): number

/**
 * 根据文件索引获取文件名以及大小
 * @param index 索引
 */
export function getFileNameAndSize(index: number): FileNameAndSize
