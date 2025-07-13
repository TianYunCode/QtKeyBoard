#ifndef PINYINIME_INCLUDE_UTF16CHAR_H__
#define PINYINIME_INCLUDE_UTF16CHAR_H__

#include <stdlib.h>

namespace ime_pinyin {

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned short char16;  ///< UTF-16字符类型定义

/**
 * 从UTF-16字符串中提取令牌
 *
 * @param utf16_str 待分割字符串
 * @param token_size 返回令牌长度
 * @param utf16_str_next 返回剩余字符串指针
 * @return 令牌字符串指针（以'\0'结尾）或NULL
 */
char16* utf16_strtok(char16 *utf16_str, size_t *token_size,
                     char16 **utf16_str_next);

/**
 * UTF-16字符串转整数
 *
 * @param utf16_str UTF-16字符串
 * @return 转换后的整数值
 */
int utf16_atoi(const char16 *utf16_str);

/**
 * UTF-16字符串转浮点数
 *
 * @param utf16_str UTF-16字符串
 * @return 转换后的浮点数值
 */
float utf16_atof(const char16 *utf16_str);

/**
 * 计算UTF-16字符串长度
 *
 * @param utf16_str UTF-16字符串
 * @return 字符串长度（字符数）
 */
size_t utf16_strlen(const char16 *utf16_str);

/// UTF-16字符串比较
int utf16_strcmp(const char16 *str1, const char16 *str2);

/**
 * UTF-16字符串限定长度比较
 *
 * @param str1 字符串1
 * @param str2 字符串2
 * @param size 比较长度
 * @return 比较结果（同strncmp）
 */
int utf16_strncmp(const char16 *str1, const char16 *str2, size_t size);

/// UTF-16字符串复制
char16* utf16_strcpy(char16 *dst, const char16 *src);

/**
 * UTF-16字符串限定长度复制
 *
 * @param dst 目标缓冲区
 * @param src 源字符串
 * @param size 复制长度
 * @return 目标缓冲区指针
 */
char16* utf16_strncpy(char16 *dst, const char16 *src, size_t size);

/**
 * UTF-16字符串复制到char缓冲区
 *
 * @param dst 目标缓冲区（char类型）
 * @param src 源字符串（char16类型）
 * @return 目标缓冲区指针
 *
 * @note 假设源字符串仅包含ASCII字符
 */
char* utf16_strcpy_tochar(char *dst, const char16 *src);

#ifdef __cplusplus
}
#endif
}

#endif  // PINYINIME_INCLUDE_UTF16CHAR_H__
