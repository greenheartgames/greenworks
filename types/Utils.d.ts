import {ErrorCallback} from './Defination';

export declare module Utils {
  /**
   * 将source_dir移动到target_dir
   *
   * @param source_dir 源目录
   * @param target_dir 目标目录
   * @param success_callback
   * @param error_callback
   */
  export function move(source_dir: string, target_dir: string, success_callback: () => void, error_callback?: ErrorCallback): void

  /**
   * 将source_dir打包成zip压缩包
   *
   * @param zip_file_path 压缩包文件路径
   * @param source_dir 源目录
   * @param password 空字符串表示没有密码
   * @param compress_level 压缩系数0-9，数值越大压缩率越大
   * @param success_callback
   * @param error_callback
   */
  export function createArchive(zip_file_path: string, source_dir: string, password: string, compress_level: number, success_callback: () => void, error_callback?: ErrorCallback): void

  /**
   * 解压缩zip文件
   *
   * @param zip_file_path 压缩包文件路径
   * @param extract_dir 解压缩的目录
   * @param password 空字符串表示没有密码
   * @param success_callback
   * @param error_callback
   */
  export function extractArchive(zip_file_path: string, extract_dir: string, password: string, success_callback: () => void, error_callback?: ErrorCallback): void
}


