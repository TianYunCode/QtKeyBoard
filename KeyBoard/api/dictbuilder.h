#ifndef PINYINIME_INCLUDE_DICTBUILDER_H__
#define PINYINIME_INCLUDE_DICTBUILDER_H__

#include <stdlib.h>
#include "./utf16char.h"
#include "./dictdef.h"
#include "./dictlist.h"
#include "./spellingtable.h"
#include "./spellingtrie.h"
#include "./splparser.h"

namespace ime_pinyin {

#ifdef ___BUILD_MODEL___

#define ___DO_STATISTICS___

class DictTrie;

class DictBuilder {
private:
    // 原始词条数组缓冲区
    LemmaEntry *lemma_arr_;
    size_t lemma_num_;

    // 存储所有可能的单字符项
    // 不同项可能包含相同汉字但不同拼音ID
    SingleCharItem *scis_;
    size_t scis_num_;

    // 字典树中根节点层级为-1
    // 根节点和0级节点的词条节点
    LmaNodeLE0 *lma_nodes_le0_;

    // 层级大于0的节点词条
    LmaNodeGE1 *lma_nodes_ge1_;

    // 已使用的词条节点数
    size_t lma_nds_used_num_le0_;
    size_t lma_nds_used_num_ge1_;

    // 存储同音词ID
    LemmaIdType *homo_idx_buf_;
    // 仅含单字符的同音词数量
    size_t homo_idx_num_eq1_;
    // 含多字符的同音词数量
    size_t homo_idx_num_gt1_;

    // 最高分词条项
    LemmaEntry *top_lmas_;
    size_t top_lmas_num_;

    SpellingTable *spl_table_;
    SpellingParser *spl_parser_;

#ifdef ___DO_STATISTICS___
    size_t max_sonbuf_len_[kMaxLemmaSize];  // 子节点缓冲区最大长度
    size_t max_homobuf_len_[kMaxLemmaSize]; // 同音词缓冲区最大长度

    size_t total_son_num_[kMaxLemmaSize];          // 子节点总数
    size_t total_node_hasson_[kMaxLemmaSize];      // 含子节点的节点总数
    size_t total_sonbuf_num_[kMaxLemmaSize];       // 子节点缓冲区总数
    size_t total_sonbuf_allnoson_[kMaxLemmaSize];  // 无子节点的子缓冲区总数
    size_t total_node_in_sonbuf_allnoson_[kMaxLemmaSize]; // 无子节点缓冲区中的节点数
    size_t total_homo_num_[kMaxLemmaSize];         // 同音词总数

    size_t sonbufs_num1_;     // 仅含1个子节点的缓冲区数量
    size_t sonbufs_numgt1_;   // 含多个子节点的缓冲区数量

    size_t total_lma_node_num_;  // 词条节点总数

    void stat_init();   // 初始化统计
    void stat_print();  // 打印统计
#endif

public:
    DictBuilder();   // 构造函数
    ~DictBuilder();  // 析构函数

    /**
   * 从原始文件构建字典树
   *
   * @param fn_raw 原始词典文件名
   * @param fn_validhzs 有效汉字文件（NULL则仅包含GB2312字符）
   * @param dict_trie 目标字典树对象
   * @return 成功返回true
   */
    bool build_dict(const char* fn_raw, const char* fn_validhzs,
                    DictTrie *dict_trie);

private:
    // 将ID转换为字符缓冲区（调用者需确保参数有效）
    void id_to_charbuf(unsigned char *buf, LemmaIdType id);

    // 更新节点的子节点偏移量
    void set_son_offset(LmaNodeGE1 *node, size_t offset);

    // 更新节点同音词ID缓冲区的偏移量
    void set_homo_id_buf_offset(LmaNodeGE1 *node, size_t offset);

    // 格式化拼音字符串
    void format_spelling_str(char *spl_str);

    /**
   * 按汉字字符串排序词条并为唯一项分配ID
   * 排序目的：支持基于前缀的预测查询
   * 单字符项可能采用其他顺序（如按拼音ID）
   *
   * @return 下一个可用未分配ID
   */
    LemmaIdType sort_lemmas_by_hz();

    /**
   * 构建单字符项列表并填充词条缓冲区中的hanzi_scis_ids
   * 需在词条数组就绪后调用
   *
   * @return 唯一单字符项元素数量
   */
    size_t build_scis();

    /**
   * 使用拼音数组子集构建子树（从item_start到item_end）
   *
   * @param parent 待更新的父节点（LmaNodeLE0或LmaNodeGE1类型）
   * @param lemma_arr 词条数组
   * @param item_start 起始索引
   * @param item_end 结束索引
   * @param level 当前层级
   * @return 成功返回true
   */
    bool construct_subset(void* parent, LemmaEntry* lemma_arr,
                          size_t item_start, size_t item_end, size_t level);


    /**
   * 从文件读取有效汉字
   *
   * @param fn_validhzs 有效汉字文件名
   * @param num 返回汉字数量
   * @return 已排序的汉字缓冲区（调用者需释放内存）
   */
    char16* read_valid_hanzis(const char *fn_validhzs, size_t *num);


    /**
   * 读取原始词典
   *
   * @param fn_raw 原始词典文件名
   * @param fn_validhzs 有效汉字文件名
   * @param max_item 最大读取词条数（超限则截取）
   * @return 成功读取的词条数量
   */
    size_t read_raw_dict(const char* fn_raw, const char *fn_validhzs,
                         size_t max_item);

    // 检查字符是否在汉字列表中
    bool hz_in_hanzis_list(const char16 *hzs, size_t hzs_len, char16 hz);

    // 检查字符串所有字符是否都在汉字列表中
    bool str_in_hanzis_list(const char16 *hzs, size_t hzs_len,
                            const char16 *str, size_t str_len);

    // 获取最高分词条
    void get_top_lemmas();

    /**
   * 分配构建字典所需资源
   *
   * @param lma_num 待加载词条数量
   * @return 成功返回true
   */
    bool alloc_resource(size_t lma_num);

    // 释放资源
    void free_resource();
};
#endif  // ___BUILD_MODEL___
}

#endif  // PINYINIME_INCLUDE_DICTBUILDER_H__
