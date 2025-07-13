#ifndef PINYINIME_INCLUDE_UTF16READER_H__
#define PINYINIME_INCLUDE_UTF16READER_H__

#include <stdio.h>
#include "./utf16char.h"

namespace ime_pinyin {

/**
 * UTF-16文件读取器类
 * 提供带缓冲的UTF-16文本行读取功能
 */
class Utf16Reader {
private:
    FILE *fp_;               ///< 文件指针
    char16 *buffer_;         ///< 读缓冲区
    size_t buffer_total_len_; ///< 缓冲区总长度
    size_t buffer_next_pos_;  ///< 缓冲区下一个读取位置
    size_t buffer_valid_len_; ///< 缓冲区有效数据长度（始终小于buffer_total_len_-buffer_next_pos_）

public:
    Utf16Reader();   ///< 构造函数
    ~Utf16Reader();  ///< 析构函数

    /**
   * 打开UTF-16编码文件
   *
   * @param filename 文件名
   * @param buffer_len 缓冲区长度（加速后续读取）
   * @return 成功打开返回true
   */
    bool open(const char* filename, size_t buffer_len);

    /**
   * 读取一行文本
   *
   * @param read_buf 读取缓冲区
   * @param max_len 缓冲区最大容量
   * @return 读取的文本行（失败返回NULL）
   */
    char16* readline(char16* read_buf, size_t max_len);

    /// 关闭文件
    bool close();
};
}

#endif  // PINYINIME_INCLUDE_UTF16READER_H__
