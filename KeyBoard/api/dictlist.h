#ifndef PINYINIME_INCLUDE_DICTLIST_H__
#define PINYINIME_INCLUDE_DICTLIST_H__

#include <stdlib.h>
#include <stdio.h>
#include "./dictdef.h"
#include "./searchutility.h"
#include "./spellingtrie.h"
#include "./utf16char.h"

namespace ime_pinyin {

class DictList {
private:
    bool initialized_;  ///< 初始化标志

    const SpellingTrie *spl_trie_;  ///< 拼音trie树引用

    // 单字符项数量（首项为空，因ID0无效）
    uint32 scis_num_;
    char16 *scis_hz_;        ///< 单字符项汉字数组
    SpellingId *scis_splid_; ///< 单字符项拼音ID数组

    char16 *buf_;  ///< 存储词条列表的内存块

    // 按词条长度索引的起始位置（以char16计数）
    // 索引i对应长度为(i+1)的词条
    uint32 start_pos_[kMaxLemmaSize + 1];
    uint32 start_id_[kMaxLemmaSize + 1];  ///< 按长度索引的起始ID

    /// 不同长度词条的比较函数数组
    int (*cmp_func_[kMaxLemmaSize])(const void *, const void *);

    /**
   * 分配资源
   *
   * @param buf_size 词条缓冲区大小
   * @param scim_num 单字符项数量
   * @return 成功返回true
   */
    bool alloc_resource(size_t buf_size, size_t scim_num);

    /// 释放资源
    void free_resource();

#ifdef ___BUILD_MODEL___
    /**
   * 计算所需内存（含start_pos[]缓冲区）
   *
   * @param lemma_arr 词条数组
   * @param lemma_num 词条数量
   * @return 所需内存字节数
   */
    size_t calculate_size(const LemmaEntry *lemma_arr, size_t lemma_num);

    /**
   * 填充单字符项数据
   *
   * @param scis 单字符项数组
   * @param scis_num 单字符项数量
   */
    void fill_scis(const SingleCharItem *scis, size_t scis_num);

    /**
   * 填充词条列表（需在calculate_size()后调用）
   *
   * @param lemma_arr 词条数组
   * @param lemma_num 词条数量
   */
    void fill_list(const LemmaEntry *lemma_arr, size_t lemma_num);

    /**
   * 查找以指定汉字开头的双字词位置
   *
   * @param hz_char 首汉字
   * @return 缓冲区中对应位置指针
   */
    char16* find_pos2_startedbyhz(char16 hz_char);
#endif

    /**
   * 查找以指定汉字开头的词条起始位置
   *
   * @param last_hzs 汉字前缀数组
   * @param word_Len 词条长度
   * @param cmp_func 比较函数
   * @return 缓冲区中对应位置指针
   */
    char16* find_pos_startedbyhzs(const char16 last_hzs[],
                                  size_t word_Len,
                                  int (*cmp_func)(const void *, const void *));

public:
    DictList();   ///< 构造函数
    ~DictList();  ///< 析构函数

    /**
   * 保存词条列表到文件
   *
   * @param fp 文件指针
   * @return 成功返回true
   */
    bool save_list(FILE *fp);

    /**
   * 从文件加载词条列表
   *
   * @param fp 文件指针
   * @return 成功返回true
   */
    bool load_list(QFile *fp);

#ifdef ___BUILD_MODEL___
    /**
   * 从词条数组初始化列表
   *
   * @param scis 单字符项数组
   * @param scis_num 单字符项数量
   * @param lemma_arr 词条数组（需按汉字排序且ID从1开始）
   * @param lemma_num 词条数量
   * @return 成功返回true
   */
    bool init_list(const SingleCharItem *scis, size_t scis_num,
                   const LemmaEntry *lemma_arr, size_t lemma_num);
#endif

    /**
   * 获取词条ID对应的汉字字符串
   *
   * @param id_hz 汉字索引的词条ID
   * @param str_buf 输出缓冲区
   * @param str_max 缓冲区最大容量
   * @return 实际写入字符数
   */
    uint16 get_lemma_str(LemmaIdType id_hz, char16 *str_buf, uint16 str_max);

    /**
   * 将单字符项ID转换为实际汉字
   *
   * @param str 单字符项ID数组
   * @param str_len 数组长度
   */
    void convert_to_hanzis(char16 *str, uint16 str_len);

    /**
   * 将汉字转换为单字符项ID
   *
   * @param str 汉字数组
   * @param str_len 数组长度
   */
    void convert_to_scis_ids(char16 *str, uint16 str_len);

    /**
   * 基于历史汉字进行预测
   *
   * @param last_hzs 历史汉字数组（长度≤kMaxPredictSize）
   * @param hzs_len 历史汉字数量
   * @param npre_items 预测结果缓冲区
   * @param npre_max 缓冲区容量
   * @param b4_used 已使用的预测项数
   * @return 新增预测项数量
   */
    size_t predict(const char16 last_hzs[], uint16 hzs_len,
                   NPredictItem *npre_items, size_t npre_max,
                   size_t b4_used);

    /**
   * 获取汉字对应的拼音ID
   *
   * @param hanzi 目标汉字
   * @param half_splid 半拼音ID（无效则忽略）
   * @param splids 返回的拼音ID数组
   * @param max_splids 数组容量
   * @return 实际返回拼音ID数量
   */
    uint16 get_splids_for_hanzi(char16 hanzi, uint16 half_splid,
                                uint16 *splids, uint16 max_splids);

    /**
   * 获取词条的汉字索引ID
   *
   * @param str 汉字字符串
   * @param str_len 字符串长度
   * @return 词条ID（未找到返回0）
   */
    LemmaIdType get_lemma_id(const char16 *str, uint16 str_len);
};
}

#endif  // PINYINIME_INCLUDE_DICTLIST_H__
