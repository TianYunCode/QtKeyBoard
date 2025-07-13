#ifndef PINYINIME_INCLUDE_SPELLINGTABLE_H__
#define PINYINIME_INCLUDE_SPELLINGTABLE_H__

#include <stdlib.h>
#include "./dictdef.h"

namespace ime_pinyin {

#ifdef ___BUILD_MODEL___

const size_t kMaxSpellingSize = kMaxPinyinSize;  ///< 最大拼音尺寸

/// 原始拼音结构（字符串+频率）
typedef struct {
    char str[kMaxSpellingSize + 1];  ///< 拼音字符串
    double freq;                     ///< 出现频率
} RawSpelling, *PRawSpelling;

/**
 * 拼音表类（用于存储拼音字符串）
 *
 * 输入的拼音字符串长度必须≤初始化设置的spelling_size_
 * 超长字符串将被截断保留前spelling_size_字符
 */
class SpellingTable {
private:
    static const size_t kNotSupportNum = 3;  ///< 不支持的拼音数量
    static const char kNotSupportList[kNotSupportNum][kMaxSpellingSize + 1];  ///< 不支持拼音列表

    bool need_score_;          ///< 是否需要计算分数
    size_t spelling_max_num_;  ///< 最大拼音存储数
    RawSpelling *raw_spellings_; ///< 原始拼音数组
    char *spelling_buf_;       ///< 拼音缓冲区（含分数）
    size_t spelling_size_;     ///< 拼音字符串尺寸
    double total_freq_;        ///< 总频率
    size_t spelling_num_;      ///< 当前拼音数量
    double score_amplifier_;   ///< 分数放大系数
    unsigned char average_score_;  ///< 平均分数
    bool frozen_;              ///< 冻结标志（冻结后禁止添加/查询）

    size_t get_hash_pos(const char* spelling_str);  ///< 获取哈希位置
    size_t hash_pos_next(size_t hash_pos);          ///< 获取下一个哈希位置
    void free_resource();                          ///< 释放资源

public:
    SpellingTable();   ///< 构造函数
    ~SpellingTable();  ///< 析构函数

    /**
   * 初始化拼音表
   *
   * @param pure_spl_size 纯拼音最大长度（如"zhuang"为6）
   * @param spl_max_num 最大存储拼音数
   * @param need_score 是否需要计算分数
   * @return 成功返回true
   */
    bool init_table(size_t pure_spl_size, size_t spl_max_num, bool need_score);

    /**
   * 添加拼音到表中
   *
   * @param spelling_str 拼音字符串
   * @param spl_count 出现频率
   * @return 成功返回true（冻结后始终返回false）
   *
   * @note 已存在拼音将累计频率
   */
    bool put_spelling(const char* spelling_str, double spl_count);

    /// 检查拼音是否在表中（冻结后始终返回false）
    bool contain(const char* spelling_str);

    /**
   * 排序拼音字符串并存入缓冲区
   *
   * @param item_size 返回每个拼音项的大小
   * @param spl_num 返回拼音总数
   * @return 排序后的拼音缓冲区指针
   *
   * @note 排序后禁止调用put_spelling()/contain()
   * @note 若需计算分数，每个拼音项末尾追加1字节分数（分数越低概率越高）
   */
    const char* arrange(size_t *item_size, size_t *spl_num);

    /// 获取分数放大系数
    float get_score_amplifier();

    /// 获取平均分数
    unsigned char get_average_score();
};
#endif  // ___BUILD_MODEL___
}

#endif  // PINYINIME_INCLUDE_SPELLINGTABLE_H__
