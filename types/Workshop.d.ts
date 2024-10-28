import {
  ErrorCallback,
  ItemInstallInfo,
  SteamUGCDetails,
  UGCItemState,
  UGCMatchingType,
  UGCQueryType,
  UserUGCList,
  UserUGCListSortOrder,
} from './Defination';

/**
 * 获取返回的所有创意工坊文件的详细信息
 *
 * @param options
 * app_id: 消耗此物品的应用的 App Id
 * page_num: 要接收结果的页码，第一次调用时应从1开始，每页最大50条数据
 * @param ugc_matching_type 创意工坊匹配类型
 * @param ugc_query_type 创意工坊查询类型
 * @param success_callback
 * items: 创意工坊文件详细信息列表
 * @param error_callback 错误回调
 */
export function ugcGetItems(options: { app_id: number, page_num: number }, ugc_matching_type: UGCMatchingType, ugc_query_type: UGCQueryType, success_callback: (items: SteamUGCDetails[]) => void, error_callback?: ErrorCallback): void


/**
 * 共享文件
 * @param filePath 文件路径
 * @param success_callback file_handle: 表示可以与用户和功能共享的文件句柄。
 * @param error_callback 错误回调
 */
export function fileShare(filePath: string, success_callback: (file_handle: string) => void, error_callback?: ErrorCallback): void

/**
 * 发布工坊文件
 * 已弃用 - 只与已弃用的基于 RemoteStorage 的创意工坊 API 一起使用。
 *
 * @param options
 * app_id: 将使用此工坊文件的游戏ID
 * tags: 工坊文件的标签
 * @param file_path
 * @param image_path
 * @param title
 * @param description
 * @param success_callback publish_file_handle: 发布的文件句柄
 * @param error_callback
 */
export function publishWorkshopFile(options: { app_id: number, tags: string[] }, file_path: string, image_path: string, title: string, description: string, success_callback: (publish_file_handle: string) => void, error_callback?: ErrorCallback): void

/**
 * 更新已发布的工坊文件
 * file_path/image_path/title/description这些字段是空字符串时代表不更新该字段
 *
 * @param options
 * tags: 工坊文件的标签。若要删除所有现有标记，请传递带有空字符串[“”]的数组
 * @param published_file_handle 发布的文件句柄
 * @param file_path
 * @param image_path
 * @param title
 * @param description
 * @param success_callback
 * @param error_callback
 */
export function updatePublishedWorkshopFile(options: { tags: string[] }, published_file_handle: string, file_path: string, image_path: string, title: string, description: string, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 将用户生成的内容发布到Steam工坊。
 *
 * @param file_path
 * @param title
 * @param description
 * @param image_path
 * @param success_callback published_file_handle: 发布的文件句柄
 * @param error_callback
 * @param progress_callback progress_msg: 发布过程的当前进度：在将文件保存到Steam Cloud时完成，在共享文件时完成。
 */
export function ugcPublish(file_path: string, title: string, description: string, image_path: string, success_callback: (published_file_handle: string) => void, error_callback: ErrorCallback, progress_callback: (progress_msg: string) => void): void

/**
 * 更新已发布的工坊文件。
 *
 * @param published_file_handle 发布的文件句柄
 * @param file_path
 * @param title
 * @param description
 * @param image_path
 * @param success_callback
 * @param error_callback
 * @param progress_callback progress_msg: 发布过程的当前进度：在将文件保存到Steam Cloud时完成，在共享文件时完成。
 */
export function ugcPublishUpdate(published_file_handle: string, file_path: string, title: string, description: string, image_path: string, success_callback: () => void, error_callback: ErrorCallback, progress_callback: (progress_msg: string) => void): void

/**
 * 获取所有物品
 *
 * @param options
 * app_id: 将使用此工坊文件的游戏ID
 * page_num: 要接收结果的页码，第一次调用时应从1开始，每页最大50条数据
 * @param ugc_matching_type 创意工坊匹配类型
 * @param ugc_query_type 创意工坊查询类型
 * @param success_callback
 * items: 创意工坊文件详细信息列表
 * @param error_callback
 */
export function ugcGetItems(options: { app_id: number, page_num: number }, ugc_matching_type: UGCMatchingType, ugc_query_type: UGCQueryType, success_callback: (items: SteamUGCDetails[]) => void, error_callback?: ErrorCallback): void

/**
 * 为用户获取已发布的工坊文件列表
 *
 * @param options
 * app_id: 将使用此工坊文件的游戏ID
 * page_num: 要接收结果的页码，第一次调用时应从1开始，每页最大50条数据
 * @param ugc_matching_type 创意工坊匹配类型
 * @param ugc_list_sort_order 用户列表排序
 * @param ugc_list 用户列表类型
 * @param success_callback
 * items: 创意工坊文件详细信息列表
 * @param error_callback
 */
export function ugcGetUserItems(options: { app_id: number, page_num: number }, ugc_matching_type: UGCMatchingType, ugc_list_sort_order: UserUGCListSortOrder, ugc_list: UserUGCList, success_callback: (items: SteamUGCDetails[]) => void, error_callback?: ErrorCallback): void

/**
 * 下载物品文件
 *
 * @param download_file_handle 要下载的文件句柄
 * @param download_dir 在本地计算机上保存下载文件的文件路径
 * @param success_callback
 * @param error_callback
 */
export function ugcDownloadItem(download_file_handle: string, download_dir: string, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 将用户的工坊文件（UserUGCList.Subscribed，UserMatchingType.Items）下载/同步到本地的同步目录（仅当文件的上次更新时间与Steam Cloud不同或本地不存在工坊文件时才更新）。
 *
 * @param options
 * app_id: 将使用此工坊文件的游戏ID
 * page_num: 要接收结果的页码，第一次调用时应从1开始，每页最大50条数据
 * @param sync_dir 下载同步工坊文件的目录。
 * @param success_callback
 * items: 创意工坊文件详细信息列表
 * isUpdated: 是否在此函数调用的过程中更新文件
 * @param error_callback
 */
export function ugcSynchronizeItems(options: { app_id: number, page_num: number }, sync_dir: string, success_callback: (items: SteamUGCDetails[], isUpdated: boolean) => void, error_callback?: ErrorCallback): void

/**
 * 取消订阅一个工坊文件
 *
 * @param published_file_handle 已发布的文件句柄
 * @param success_callback
 * @param error_callback
 */
export function ugcUnsubscribe(published_file_handle: string, success_callback: () => void, error_callback?: ErrorCallback): void

/**
 * 显示Steam工坊页面或指定工坊文件的Steam遮罩层。
 *
 * @param published_file_id 已发布文件的id
 */
export function ugcShowOverlay(published_file_id: string): void

/**
 * 获取指定工坊文件的 greenworks.UGCItemState。
 *
 * @param published_file_id 已发布文件的id
 */
export function ugcGetItemState(published_file_id: string): UGCItemState

/**
 * 如果工坊文件的状态包括 greenworks.UGCItemState.Installed，则获取工坊文件的安装信息。如果无法获取信息，则返回undefined ；否则返回包含以下属性的对象：
 * sizeOnDisk: 返回创意工坊物品的字节大小。
 * folder: 通过复制返回包含内容的文件夹的绝对路径。
 * timestamp: 返回创意工坊物品上次更新的时间。
 *
 * @param published_file_id 已发布文件的id
 *
 */
export function ugcGetItemInstallInfo(published_file_id: string): ItemInstallInfo
