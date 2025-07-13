#ifndef PINYINIME_ANDPY_INCLUDE_MATRIXSEARCH_H__
#define PINYINIME_ANDPY_INCLUDE_MATRIXSEARCH_H__

#include <stdlib.h>
#include "./atomdictbase.h"
#include "./dicttrie.h"
#include "./searchutility.h"
#include "./spellingtrie.h"
#include "./splparser.h"

namespace ime_pinyin {

static const size_t kMaxRowNum = kMaxSearchSteps;  ///< 最大行数

/// 字典匹配信息结构
typedef struct {
    MileStoneHandle dict_handles[2];  ///< 系统/用户词典的里程碑句柄
    PoolPosType dmi_fr;               ///< 来源DMI节点（-1表示根节点）
    uint16 spl_id;                    ///< 当前拼音ID（半ID时为该声母的首节点）
    unsigned char dict_level:7;       ///< 节点层级（根节点为0）
    unsigned char c_phrase:1;         ///< 是否为组词短语
    unsigned char splid_end_split:1;  ///< 拼音ID是否以分隔符结束
    unsigned char splstr_len:7;       ///< 整个词的拼音字符串长度
    unsigned char all_full_id:1;      ///< 是否全为完整拼音ID（用于键盘映射模式）
} DictMatchInfo, *PDictMatchInfo;

/// 矩阵节点结构
typedef struct MatrixNode {
    LemmaIdType id;     ///< 词条ID
    float score;        ///< 分数
    MatrixNode *from;   ///< 前驱节点
    PoolPosType dmi_fr; ///< 来源DMI节点
    uint16 step;        ///< 所属步骤
} MatrixNode, *PMatrixNode;

/// 矩阵行结构
typedef struct {
    PoolPosType mtrx_nd_pos;   ///< 矩阵节点在池中的位置
    PoolPosType dmi_pos;       ///< DMI节点在池中的位置
    uint16 mtrx_nd_num;        ///< 矩阵节点数量
    uint16 dmi_num:15;         ///< DMI节点数量
    uint16 dmi_has_full_id:1;  ///< 是否存在完整拼音ID的DMI节点（决定是否扩展子串）
    MatrixNode *mtrx_nd_fixed; ///< 用户选择的矩阵节点
} MatrixRow, *PMatrixRow;

/// 组词短语结构
typedef struct {
    uint16 spl_ids[kMaxRowNum];      ///< 拼音ID数组
    uint16 spl_start[kMaxRowNum];    ///< 拼音起始位置
    char16 chn_str[kMaxRowNum];      ///< 中文字符串
    uint16 sublma_start[kMaxRowNum]; ///< 子词条起始位置（按汉字计数）
    size_t sublma_num;               ///< 子词条数量
    uint16 length;                   ///< 总长度（按汉字计数）
} ComposingPhrase, *TComposingPhrase;

/**
 * 矩阵搜索核心类（实现拼音输入法Viterbi解码）
 */
class MatrixSearch {
private:
    // ===================== 配置常量 =====================
    static const bool kPredictLimitGt1 = false;         ///< 是否限制长度>1的预测结果数量
    static const bool kPreferLongHistoryPredict = true; ///< 是否优先长历史预测
    static const bool kOnlyUserDictPredict = false;     ///< 是否仅使用用户词典预测（调试用）
    static const size_t kMaxLmaPsbItems = 1450;         ///< 最大候选词条缓存数
    static const size_t kMaxNodeARow = 5;               ///< 每步最大节点数
    static const size_t kMaxSentenceLength = 16;        ///< 最大句子长度（汉字）
    static const size_t kMtrxNdPoolSize = 200;          ///< 矩阵节点池大小
    static const size_t kDmiPoolSize = 800;             ///< DMI节点池大小

    bool inited_;                   ///< 初始化标志
    const SpellingTrie *spl_trie_;  ///< 拼音Trie树引用
    bool xi_an_enabled_;            ///< "xi'an"类分割开关（默认关闭）
    DictTrie* dict_trie_;           ///< 系统词典
    AtomDictBase* user_dict_;       ///< 用户词典
    SpellingParser* spl_parser_;    ///< 拼音解析器
    size_t max_sps_len_;            ///< 最大拼音字符串长度
    size_t max_hzs_len_;            ///< 最大中文字符串长度

    // ===================== 拼音相关 =====================
    char pys_[kMaxRowNum];          ///< 拼音字符串缓冲区
    size_t pys_decoded_len_;        ///< 已解码拼音长度

    // ===================== 资源池 =====================
    size_t *share_buf_;             ///< 共享缓冲区
    MatrixNode *mtrx_nd_pool_;      ///< 矩阵节点池
    PoolPosType mtrx_nd_pool_used_; ///< 矩阵节点池使用量
    DictMatchInfo *dmi_pool_;       ///< DMI节点池
    PoolPosType dmi_pool_used_;     ///< DMI节点池使用量
    MatrixRow *matrix_;             ///< 矩阵（首行为起始行）

    // ===================== 工作缓冲区 =====================
    DictExtPara *dep_;              ///< 词典扩展参数
    NPredictItem *npre_items_;      ///< 预测项缓冲区
    size_t npre_items_len_;         ///< 预测项缓冲区长度

    // ===================== 解码状态 =====================
    size_t lma_id_num_;             ///< 句子候选词条数
    uint16 lma_start_[kMaxRowNum];  ///< 词条起始位置（按拼音ID计数）
    LemmaIdType lma_id_[kMaxRowNum];///< 句子候选词条ID
    size_t fixed_lmas_;             ///< 固定词条数
    uint8 fixed_lmas_no1_[kMaxRowNum]; ///< 固定词条是否为首选标志

    // ===================== 组词短语 =====================
    ComposingPhrase c_phrase_;      ///< 组词短语
    bool dmi_c_phrase_;             ///< 当前是否匹配组词短语

    // ===================== 拼音切分 =====================
    size_t spl_id_num_;             ///< 拼音ID数量
    uint16 spl_start_[kMaxRowNum];  ///< 拼音起始位置
    uint16 spl_id_[kMaxRowNum];     ///< 拼音ID数组
    size_t fixed_hzs_;              ///< 固定汉字位置

    // ===================== 候选管理 =====================
    LmaPsbItem lpi_items_[kMaxLmaPsbItems]; ///< 候选词条缓存
    size_t lpi_total_;               ///< 候选词条总数

    /// 重置所有指针为NULL（仅限构造函数和free_resource调用）
    void reset_pointers_to_null();

    /// 分配资源
    bool alloc_resource();

    /// 释放资源
    void free_resource();

    /// 完全重置搜索空间
    bool reset_search0();

    /**
   * 从指定位置重置搜索空间
   *
   * @param ch_pos 重置起始位置
   * @param clear_fixed_this_step 是否清除本步固定状态
   * @param clear_dmi_this_step 是否清除本步DMI节点
   * @param clear_mtrx_this_step 是否清除本步矩阵节点
   * @return 成功返回true
   */
    bool reset_search(size_t ch_pos, bool clear_fixed_this_step,
                      bool clear_dmi_this_step, bool clear_mtrx_this_step);

    /// 删除拼音字符串中的指定部分
    void del_in_pys(size_t start, size_t len);

    /// 合并固定词条为组词短语
    void merge_fixed_lmas(size_t del_spl_pos);

    /// 获取拼音起始位置和ID（结果存入spl_id_num_, spl_start_, spl_id_）
    void get_spl_start_id();

    /**
   * 获取匹配拼音ID流的候选词条
   *
   * @param splid_str 拼音ID流
   * @param splid_str_len 拼音ID长度
   * @param lma_buf 输出缓冲区
   * @param max_lma_buf 缓冲区容量
   * @param pfullsent 全句候选（非空时排除相同词条）
   * @param sort_by_psb 是否按概率排序
   * @return 候选词条数量
   */
    size_t get_lpis(const uint16* splid_str, size_t splid_str_len,
                    LmaPsbItem* lma_buf, size_t max_lma_buf,
                    const char16 *pfullsent, bool sort_by_psb);

    /// 获取词条ID对应的汉字字符串
    uint16 get_lemma_str(LemmaIdType id_lemma, char16 *str_buf, uint16 str_max);

    /// 获取词条ID对应的拼音ID
    uint16 get_lemma_splids(LemmaIdType id_lemma, uint16 *splids,
                            uint16 splids_max, bool arg_valid);

    /**
   * 扩展DMI节点
   *
   * @param dep 词典扩展参数
   * @param dmi_s 源DMI节点
   * @return 新DMI节点数量（0或1）
   *
   * @note 会更新lpi_items_和lpi_total_
   */
    size_t extend_dmi(DictExtPara *dep, DictMatchInfo *dmi_s);

    /// 扩展组词短语的DMI节点
    size_t extend_dmi_c(DictExtPara *dep, DictMatchInfo *dmi_s);

    /**
   * 扩展矩阵节点
   *
   * @param mtrx_nd 矩阵节点
   * @param lpi_items 候选词条数组
   * @param lpi_num 候选词条数量
   * @param dmi_fr 来源DMI位置
   * @param res_row 目标行号
   * @return 固定返回0
   */
    size_t extend_mtrx_nd(MatrixNode *mtrx_nd, LmaPsbItem lpi_items[],
                          size_t lpi_num, PoolPosType dmi_fr, size_t res_row);

    /// 在指定位置匹配DMI节点
    PoolPosType match_dmi(size_t step_to, uint16 spl_ids[], uint16 spl_id_num);

    /// 添加字符到拼音缓冲区
    bool add_char(char ch);
    bool prepare_add_char(char ch);
    bool add_char_qwerty();  ///< QWERTY键盘模式添加字符

    /// 准备候选列表（从最后一个固定汉字位置开始）
    void prepare_candidates();

    /// 检查指定位置是否为分隔符
    bool is_split_at(uint16 pos);

    /// 填充DMI结构
    void fill_dmi(DictMatchInfo *dmi, MileStoneHandle *handles,
                  PoolPosType dmi_fr,
                  uint16 spl_id, uint16 node_num, unsigned char dict_level,
                  bool splid_end_split, unsigned char splstr_len,
                  unsigned char all_full_id);

    /// 内部预测实现
    size_t inner_predict(const char16 fixed_scis_ids[], uint16 scis_num,
                         char16 predict_buf[][kMaxPredictSize + 1],
                         size_t buf_len);

    /// 尝试添加首选候选到用户词典
    bool try_add_cand0_to_userdict();

    /**
   * 添加用户词条到用户词典
   *
   * @param lma_from 起始词条索引
   * @param lma_num 合并词条数量
   * @param score 词条分数
   * @return 成功返回true
   */
    bool add_lma_to_userdict(uint16 lma_from, uint16 lma_num, float score);

    /// 更新词典频率
    void update_dict_freq();

    /// 调试输出DMI信息
    void debug_print_dmi(PoolPosType dmi_pos, uint16 nest_level);

public:
    MatrixSearch();   ///< 构造函数
    ~MatrixSearch();  ///< 析构函数

    /// 初始化系统词典和用户词典
    bool init(const char *fn_sys_dict, const char *fn_usr_dict);

    /// 通过文件描述符初始化
    bool init_fd(int sys_fd, long start_offset, long length,
                 const char *fn_usr_dict);

    /// 初始化用户词典
    void init_user_dictionary(const char *fn_usr_dict);

    /// 检查用户词典是否启用
    bool is_user_dictionary_enabled() const;

    /// 设置最大长度限制
    void set_max_lens(size_t max_sps_len, size_t max_hzs_len);

    /// 关闭引擎
    void close();

    /// 刷新缓存
    void flush_cache();

    /// 设置"西安"类分割开关
    void set_xi_an_switch(bool xi_an_enabled);

    /// 获取"西安"类分割开关状态
    bool get_xi_an_switch();

    /// 完全重置搜索空间（等价于reset_search(0)）
    bool reset_search();

    /**
   * 搜索拼音字符串
   *
   * @param py 拼音字符串
   * @param py_len 拼音长度
   * @return 成功解析的位置
   */
    size_t search(const char *py, size_t py_len);

    /**
   * 删除拼音并重新搜索
   *
   * @param pos 删除位置
   * @param is_pos_in_splid 是否按拼音ID索引
   * @param clear_fixed_this_step 是否清除本步固定状态
   * @return 新拼音字符串长度
   */
    size_t delsearch(size_t pos, bool is_pos_in_splid,
                     bool clear_fixed_this_step);

    /// 获取候选数量
    size_t get_candidate_num();

    /// 获取引擎存储的拼音字符串
    const char* get_pystr(size_t *decoded_len);

    /// 获取首句候选的拼音切分
    size_t get_spl_start(const uint16 *&spl_start);

    /// 获取指定候选
    char16* get_candidate(size_t cand_id, char16 *cand_str, size_t max_len);

    /// 获取首选候选（全句候选）
    char16* get_candidate0(char16* cand_str, size_t max_len,
                           uint16 *retstr_len, bool only_unfixed);

    /// 选择候选
    size_t choose(size_t cand_id);

    /// 取消上次选择
    size_t cancel_last_choice();

    /// 获取固定汉字长度
    size_t get_fixedlen();

    /// 获取预测结果
    size_t get_predicts(const char16 fixed_buf[],
                        char16 predict_buf[][kMaxPredictSize + 1],
                        size_t buf_len);
};
}

#endif  // PINYINIME_ANDPY_INCLUDE_MATRIXSEARCH_H__
