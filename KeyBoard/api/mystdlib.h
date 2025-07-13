#ifndef PINYINIME_INCLUDE_MYSTDLIB_H__
#define PINYINIME_INCLUDE_MYSTDLIB_H__

#include <stdlib.h>

namespace ime_pinyin {

/**
 * 自定义快速排序实现
 *
 * @param p 待排序数组起始地址
 * @param n 数组元素个数
 * @param es 每个元素的大小（字节）
 * @param cmp 比较函数指针
 */
void myqsort(void *p, size_t n, size_t es,
             int (*cmp)(const void *, const void *));

/**
 * 自定义二分查找实现
 *
 * @param key 查找关键字
 * @param base 已排序数组基地址
 * @param nmemb 数组元素个数
 * @param size 每个元素的大小（字节）
 * @param compar 比较函数指针
 * @return 找到的元素指针，未找到返回NULL
 */
void *mybsearch(const void *key, const void *base,
                size_t nmemb, size_t size,
                int (*compar)(const void *, const void *));
}

#endif  // PINYINIME_INCLUDE_MYSTDLIB_H__
