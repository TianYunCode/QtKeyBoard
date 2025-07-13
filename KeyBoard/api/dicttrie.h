#ifndef PINYINIME_INCLUDE_DICTTRIE_H__
#define PINYINIME_INCLUDE_DICTTRIE_H__

#include <stdlib.h>
#include "./atomdictbase.h"
#include "./dictdef.h"
#include "./dictlist.h"
#include "./searchutility.h"
#include <QFile>

namespace ime_pinyin {

class DictTrie : AtomDictBase {
private:
    /// 解析标记结构（节点偏移占24位，节点数占8位）
    struct ParsingMark {
        size_t node_offset:24;
        size_t node_num:8;           ///< 当前拼音ID对应的节点数：
            ///< 若为声母且在字典树第一层，值等于SpellingTrie::shm2full_num()
            ///< 若非第一层，则小于SpellingTrie::shm2full_num()
            ///< 若为完整拼音ID，始终为1
    };

    /// 里程碑结构（标记部分匹配状态以加速扩展）
    struct MileStone {
        uint16 mark_start;  ///< 标记列表起始位置
        uint16 mark_num;    ///< 标记数量
    };

    DictList* dict_list_;         ///< 词条列表对象
    const SpellingTrie *spl_trie_;///< 拼音trie树对象

    LmaNodeLE0* root_;        ///< 根节点及第一层节点
    LmaNodeGE1* nodes_ge1_;   ///< 其他层级节点

    /// 拼音ID到LmaNodeLE0节点的快速索引
    uint16 *splid_le0_index_;

    uint32 lma_node_num_le0_;  ///< LE0节点数量
    uint32 lma_node_num_ge1_;  ///< GE1节点数量

    unsigned char *lma_idx_buf_;   ///< 词条索引缓冲区（前部同音词，尾部最高分词条）
    uint32 lma_idx_buf_len_;       ///< 缓冲区总字节数
    uint32 total_lma_num_;         ///< 词典总词条数
    uint32 top_lmas_num_;          ///< 最高分词条数量

    ParsingMark *parsing_marks_;   ///< 解析标记列表
    uint16 parsing_marks_pos_;     ///< 下一个可用标记位置

    MileStone *mile_stones_;       ///< 里程碑列表
    MileStoneHandle mile_stones_pos_; ///< 下一个可用里程碑句柄

    /// 获取节点的子节点偏移量
    inline size_t get_son_offset(const LmaNodeGE1 *node);

    /// 获取节点的同音词索引偏移量
    inline size_t get_homo_idx_buf_offset(const LmaNodeGE1 *node);

    /// 通过偏移量获取词条ID
    inline LemmaIdType get_lemma_id(size_t id_offset);

    /// 释放资源
    void free_resource(bool free_dict_list);

    /// 从文件加载字典
    bool load_dict(QFile *fp);

    /// 填充LE0节点的词条到缓冲区
    size_t fill_lpi_buffer(LmaPsbItem lpi_items[], size_t max_size,
                           LmaNodeLE0 *node);

    /// 填充GE1节点的词条到缓冲区
    size_t fill_lpi_buffer(LmaPsbItem lpi_items[], size_t max_size,
                           size_t homo_buf_off, LmaNodeGE1 *node,
                           uint16 lma_len);

    /// 从0级扩展字典
    MileStoneHandle extend_dict0(MileStoneHandle from_handle,
                                 const DictExtPara *dep, LmaPsbItem *lpi_items,
                                 size_t lpi_max, size_t *lpi_num);

    /// 从1级扩展字典
    MileStoneHandle extend_dict1(MileStoneHandle from_handle,
                                 const DictExtPara *dep, LmaPsbItem *lpi_items,
                                 size_t lpi_max, size_t *lpi_num);

    /// 从2级扩展字典
    MileStoneHandle extend_dict2(MileStoneHandle from_handle,
                                 const DictExtPara *dep, LmaPsbItem *lpi_items,
                                 size_t lpi_max, size_t *lpi_num);

    /// 尝试扩展拼音ID流获取词条ID
    bool try_extend(const uint16 *splids, uint16 splid_num, LemmaIdType id_lemma);

#ifdef ___BUILD_MODEL___
    /// 保存字典到文件
    bool save_dict(FILE *fp);
#endif  // ___BUILD_MODEL___

    static const int kMaxMileStone = 100;      ///< 最大里程碑数
    static const int kMaxParsingMark = 600;    ///< 最大解析标记数
    static const MileStoneHandle kFirstValidMileStoneHandle = 1; ///< 首个有效里程碑句柄

    friend class DictParser;   ///< 友元类声明
    friend class DictBuilder;  ///< 友元类声明

public:
    DictTrie();   ///< 构造函数
    ~DictTrie();  ///< 析构函数

#ifdef ___BUILD_MODEL___
    /**
   * 从原始文件构建字典树
   *
   * @param fn_raw 原始词典文件
   * @param fn_validhzs 有效汉字文件（NULL则仅包含GB2312字符）
   * @return 成功返回true
   */
    bool build_dict(const char *fn_raw, const char *fn_validhzs);

    /// 保存二进制字典（同时保存SpellingTrie/DictList）
    bool save_dict(const char *filename);
#endif  // ___BUILD_MODEL___

    /// 单字符项ID转汉字
    void convert_to_hanzis(char16 *str, uint16 str_len);

    /// 汉字转单字符项ID
    void convert_to_scis_ids(char16 *str, uint16 str_len);

    /**
   * 加载二进制字典（同时加载SpellingTrie/DictList）
   *
   * @param filename 字典文件名
   * @param start_id 起始词条ID
   * @param end_id 结束词条ID
   * @return 成功返回true
   */
    bool load_dict(const char *filename, LemmaIdType start_id,
                   LemmaIdType end_id);

    /**
   * 从文件描述符加载字典
   *
   * @param sys_fd 系统文件描述符
   * @param start_offset 起始偏移
   * @param length 数据长度
   * @param start_id 起始词条ID
   * @param end_id 结束词条ID
   * @return 成功返回true
   */
    bool load_dict_fd(int sys_fd, long start_offset, long length,
                      LemmaIdType start_id, LemmaIdType end_id);

    bool close_dict() {return true;}      ///< 关闭字典（空实现）
    size_t number_of_lemmas() {return 0;} ///< 获取词条数（空实现）

    /// 重置里程碑
    void reset_milestones(uint16 from_step, MileStoneHandle from_handle);

    /// 扩展字典树
    MileStoneHandle extend_dict(MileStoneHandle from_handle,
                                const DictExtPara *dep,
                                LmaPsbItem *lpi_items,
                                size_t lpi_max, size_t *lpi_num);

    /// 获取拼音ID流对应的候选词条
    size_t get_lpis(const uint16 *splid_str, uint16 splid_str_len,
                    LmaPsbItem *lpi_items, size_t lpi_max);

    /// 获取词条ID对应的汉字字符串
    uint16 get_lemma_str(LemmaIdType id_lemma, char16 *str_buf, uint16 str_max);

    /// 获取词条ID对应的完整拼音ID
    uint16 get_lemma_splids(LemmaIdType id_lemma, uint16 *splids,
                            uint16 splids_max, bool arg_valid);

    /// 基于历史汉字进行预测
    size_t predict(const char16 *last_hzs, uint16 hzs_len,
                   NPredictItem *npre_items, size_t npre_max,
                   size_t b4_used);

    // ========== 以下为AtomDictBase要求的空实现接口 ==========
    LemmaIdType put_lemma(char16 /*lemma_str*/[], uint16 /*splids*/[],
                          uint16 /*lemma_len*/, uint16 /*count*/) {return 0;}

    LemmaIdType update_lemma(LemmaIdType /*lemma_id*/, int16 /*delta_count*/,
                             bool /*selected*/) {return 0;}

    LemmaIdType get_lemma_id(char16 /*lemma_str*/[], uint16 /*splids*/[],
                             uint16 /*lemma_len*/) {return 0;}

    LmaScoreType get_lemma_score(LemmaIdType /*lemma_id*/) {return 0;}

    LmaScoreType get_lemma_score(char16 /*lemma_str*/[], uint16 /*splids*/[],
                                 uint16 /*lemma_len*/) {return 0;}

    bool remove_lemma(LemmaIdType /*lemma_id*/) {return false;}

    size_t get_total_lemma_count() {return 0;}
    void set_total_lemma_count_of_others(size_t count);

    void flush_cache() {}  ///< 刷新缓存（空实现）

    /// 获取汉字字符串对应的词条ID
    LemmaIdType get_lemma_id(const char16 lemma_str[], uint16 lemma_len);

    /**
   * 填充最高分词条到预测缓冲区
   *
   * @param his_len 历史长度
   * @param npre_items 预测项缓冲区
   * @param npre_max 缓冲区容量
   * @param b4_used 已使用的预测项数
   * @return 新增预测项数量
   */
    size_t predict_top_lmas(size_t his_len, NPredictItem *npre_items,
                            size_t npre_max, size_t b4_used);
};
}

#endif  // PINYINIME_INCLUDE_DICTTRIE_H__
