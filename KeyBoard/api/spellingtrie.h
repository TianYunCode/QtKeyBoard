#ifndef PINYINIME_INCLUDE_SPELLINGTRIE_H__
#define PINYINIME_INCLUDE_SPELLINGTRIE_H__

#include <stdio.h>
#include <stdlib.h>
#include "./dictdef.h"
#include <QFile>

namespace ime_pinyin {

static const unsigned short kFullSplIdStart = kHalfSpellingIdNum + 1;  ///< 完整拼音ID起始值

/// 拼音Trie树节点结构
struct SpellingNode {
    SpellingNode *first_son;  ///< 首子节点指针
    uint16 spelling_idx:11;   ///< 拼音ID索引
    uint16  num_of_son:5;     ///< 子节点数量
    char char_this_node;      ///< 当前节点字符
    unsigned char score;      ///< 节点分数
};

/**
 * 拼音Trie树类（实现拼音字符串的高效存储与检索）
 */
class SpellingTrie {
private:
    static const int kMaxYmNum = 64;                ///< 最大韵母数量
    static const size_t kValidSplCharNum = 26;      ///< 有效拼音字符数

    // 半拼音ID类型掩码
    static const uint16 kHalfIdShengmuMask = 0x01;  ///< 声母掩码
    static const uint16 kHalfIdYunmuMask = 0x02;    ///< 韵母掩码
    static const uint16 kHalfIdSzmMask = 0x04;      ///< 首字母掩码

    /// 半拼音ID到字符映射表（0位未使用）
    static const char kHalfId2Sc_[kFullSplIdStart + 1];

    static unsigned char char_flags_[];  ///< 字符标志数组
    static SpellingTrie* instance_;      ///< 单例实例指针

    char *spelling_buf_;      ///< 拼音字符串缓冲区
    uint32 spelling_size_;    ///< 最大拼音长度（含结束符和分数字节）
    uint32 spelling_num_;     ///< 完整拼音ID数量
    float score_amplifier_;   ///< 分数放大系数
    unsigned char average_score_;  ///< 平均分数

    uint8 *spl_ym_ids_;       ///< 拼音ID到韵母ID映射表
    char *ym_buf_;            ///< 韵母字符串缓冲区
    size_t ym_size_;          ///< 韵母字符串最大长度（含结束符）
    size_t ym_num_;           ///< 韵母数量

    char *splstr_queried_;     ///< 最近查询的拼音字符串（UTF-8）
    char16 *splstr16_queried_; ///< 最近查询的拼音字符串（UTF-16）

    SpellingNode* root_;       ///< Trie树根节点
    SpellingNode* dumb_node_;  ///< 非QWERTY键节点（功能键使用）
    SpellingNode* splitter_node_;  ///< 分隔符节点

    SpellingNode* level1_sons_[kValidSplCharNum];  ///< 一级子节点数组

    // 半拼音ID到完整拼音ID映射
    uint16 h2f_start_[kFullSplIdStart];  ///< 起始ID数组
    uint16 h2f_num_[kFullSplIdStart];    ///< ID数量数组

    uint16 *f2h_;  ///< 完整拼音ID到半拼音ID映射表

#ifdef ___BUILD_MODEL___
    size_t node_num_;  ///< 构建Trie树使用的节点数
#endif

    SpellingTrie();  ///< 构造函数

    /// 释放子树资源
    void free_son_trie(SpellingNode* node);

    /**
   * 构建拼音子集子树
   *
   * @param item_start 起始索引
   * @param item_end 结束索引
   * @param level 当前层级
   * @param parent 父节点
   * @return 新建子树根节点
   */
    SpellingNode* construct_spellings_subset(size_t item_start, size_t item_end,
                                             size_t level, SpellingNode *parent);

    /// 构建完整ID到半ID映射表
    bool build_f2h();

    /// 判断字符是否为声母（调用者需确保ch∈['A','Z']）
    bool is_shengmu_char(char ch) const;

    /// 判断字符是否为韵母（调用者需确保ch∈['A','Z']）
    bool is_yunmu_char(char ch) const;

#ifdef ___BUILD_MODEL___
    /// 获取拼音字符串的韵母部分（调用者需确保spl_str有效）
    const char* get_ym_str(const char *spl_str);

    /// 构建韵母信息（在拼音Trie树构建完成后调用）
    bool build_ym_info();
#endif

    // 友元类声明
    friend class SpellingParser;
    friend class SmartSplParser;
    friend class SmartSplParser2;

public:
    ~SpellingTrie();  ///< 析构函数

    /// 检查字符是否为有效拼音字符
    inline static bool is_valid_spl_char(char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    }

    /// 检查两个字符是否相同（忽略大小写）
    inline static bool is_same_spl_char(char ch1, char ch2) {
        return ch1 == ch2 || ch1 - ch2 == 'a' - 'A' || ch2 - ch1 == 'a' - 'A';
    }

    /**
   * 从排序后的拼音数组构建Trie树
   *
   * @param spelling_arr 拼音数组
   * @param item_size 每项大小
   * @param item_num 项数量
   * @param score_amplifier 分数放大系数
   * @param average_score 平均分数
   * @return 成功返回true
   */
    bool construct(const char* spelling_arr, size_t item_size, size_t item_num,
                   float score_amplifier, unsigned char average_score);

    /**
   * 检查并更新拼音ID有效性
   *
   * @param splid 待检查的拼音ID
   * @return 有效返回true（可能更新splid为完整ID）
   *
   * @note 例如'A'作为半ID(1)输入，但它是单字符韵母，将被更新为完整ID
   */
    bool if_valid_id_update(uint16 *splid) const;

    /// 检查是否为半拼音ID
    bool is_half_id(uint16 splid) const;

    /// 检查是否为完整拼音ID
    bool is_full_id(uint16 splid) const;

    /// 检查是否为单字符韵母半ID（如'A','E','O'）
    bool is_half_id_yunmu(uint16 splid) const;

    /**
   * 检查字符是否为首字母字符
   *
   * @param ch 待检查字符（需∈['A','Z']）
   * @return 是首字母字符返回true
   *
   * @note 拼音中仅i/u/v不是首字母字符
   */
    bool is_szm_char(char ch) const;

    /**
   * 检查字符在首字母模式下是否启用
   *
   * @param ch 待检查字符（需∈['A','Z']）
   * @return 启用返回true
   */
    bool szm_is_enabled(char ch) const;

    /// 在首字母模式下启用/禁用声母
    void szm_enable_shm(bool enable);

    /// 在首字母模式下启用/禁用韵母
    void szm_enable_ym(bool enable);

    /**
   * 检查字符在首字母模式下是否启用
   *
   * @param ch 待检查字符（需∈['A','Z']）
   * @return 启用返回true
   */
    bool is_szm_enabled(char ch) const;

    /**
   * 获取半拼音ID对应的完整ID数量
   *
   * @param half_id 半拼音ID
   * @return 对应的完整ID数量
   */
    uint16 half2full_num(uint16 half_id) const;

    /**
   * 获取半拼音ID对应的完整ID列表
   *
   * @param half_id 半拼音ID
   * @param spl_id_start 返回的起始完整ID
   * @return 完整ID数量
   */
    uint16 half_to_full(uint16 half_id, uint16 *spl_id_start) const;

    /**
   * 获取完整拼音ID对应的半拼音ID
   *
   * @param full_id 完整拼音ID
   * @return 对应的半拼音ID（失败返回0）
   *
   * @note 非高效方法，不频繁使用
   */
    uint16 full_to_half(uint16 full_id) const;

    /**
   * 检查半拼音ID与完整拼音ID是否兼容
   *
   * @param half_id 半拼音ID
   * @param full_id 完整拼音ID
   * @return 兼容返回true
   *
   * @note 通常当half_id == full_to_half(full_id)时为兼容
   */
    bool half_full_compatible(uint16 half_id, uint16 full_id) const;

    /// 获取只读实例指针
    static const SpellingTrie* get_cpinstance();

    /// 获取单例实例引用
    static SpellingTrie& get_instance();

    /// 保存Trie树到文件
    bool save_spl_trie(FILE *fp);

    /// 从文件加载Trie树
    bool load_spl_trie(QFile *fp);

    /// 获取拼音数量
    size_t get_spelling_num();

    /**
   * 获取韵母字符串对应的韵母ID
   *
   * @param ym_str 韵母字符串
   * @return 韵母ID（无效返回0）
   */
    uint8 get_ym_id(const char* ym_str);

    /// 获取拼音ID对应的拼音字符串（UTF-8）
    const char* get_spelling_str(uint16 splid);

    /// 获取拼音ID对应的拼音字符串（UTF-16）
    const char16* get_spelling_str16(uint16 splid);

    /**
   * 获取拼音字符串到缓冲区
   *
   * @param splid 拼音ID
   * @param splstr16 输出缓冲区
   * @param splstr16_len 缓冲区长度
   * @return 字符串长度（含结束符）
   */
    size_t get_spelling_str16(uint16 splid, char16 *splstr16,
                              size_t splstr16_len);
};
}

#endif  // PINYINIME_INCLUDE_SPELLINGTRIE_H__
