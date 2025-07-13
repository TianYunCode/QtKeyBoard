#ifndef PINYINIME_INCLUDE_USERDICT_H__
#define PINYINIME_INCLUDE_USERDICT_H__

#define ___CACHE_ENABLED___      ///< 启用缓存功能
#define ___SYNC_ENABLED___       ///< 启用同步功能
#define ___PREDICT_ENABLED___    ///< 启用预测功能

// 调试性能操作
// #define ___DEBUG_PERF___

#ifdef _WIN32
#include <time.h>
#include <winsock.h> // timeval
#else
#include <pthread.h>
#include <sys/time.h>
#endif
#include "atomdictbase.h"

namespace ime_pinyin {

/**
 * 用户词典类（继承自AtomDictBase）
 * 实现用户自定义词条的管理和持久化
 */
class UserDict : public AtomDictBase {
public:
    UserDict();   ///< 构造函数
    ~UserDict();  ///< 析构函数

    /// 加载用户词典
    bool load_dict(const char *file_name, LemmaIdType start_id,
                   LemmaIdType end_id);

    /// 关闭词典
    bool close_dict();

    /// 获取词条数量
    size_t number_of_lemmas();

    /// 重置里程碑
    void reset_milestones(uint16 from_step, MileStoneHandle from_handle);

    /// 扩展词典
    MileStoneHandle extend_dict(MileStoneHandle from_handle,
                                const DictExtPara *dep, LmaPsbItem *lpi_items,
                                size_t lpi_max, size_t *lpi_num);

    /// 获取拼音ID流对应的候选词条
    size_t get_lpis(const uint16 *splid_str, uint16 splid_str_len,
                    LmaPsbItem *lpi_items, size_t lpi_max);

    /// 获取词条ID对应的汉字字符串
    uint16 get_lemma_str(LemmaIdType id_lemma, char16* str_buf,
                         uint16 str_max);

    /// 获取词条ID对应的拼音ID
    uint16 get_lemma_splids(LemmaIdType id_lemma, uint16 *splids,
                            uint16 splids_max, bool arg_valid);

    /// 基于历史汉字进行预测
    size_t predict(const char16 last_hzs[], uint16 hzs_len,
                   NPredictItem *npre_items, size_t npre_max,
                   size_t b4_used);

    /**
   * 添加词条（需完整拼音ID）
   *
   * @param lemma_str 汉字字符串
   * @param splids 拼音ID数组
   * @param lemma_len 词条长度
   * @param count 词频
   * @return 词条ID
   */
    LemmaIdType put_lemma(char16 lemma_str[], uint16 splids[],
                          uint16 lemma_len, uint16 count);

    /**
   * 更新词条频率
   *
   * @param lemma_id 词条ID
   * @param delta_count 频率增量
   * @param selected 是否被用户选中
   * @return 更新后的词条ID
   */
    LemmaIdType update_lemma(LemmaIdType lemma_id, int16 delta_count,
                             bool selected);

    /// 获取词条ID
    LemmaIdType get_lemma_id(char16 lemma_str[], uint16 splids[],
                             uint16 lemma_len);

    /// 获取词条分数
    LmaScoreType get_lemma_score(LemmaIdType lemma_id);

    /// 通过汉字和拼音获取词条分数
    LmaScoreType get_lemma_score(char16 lemma_str[], uint16 splids[],
                                 uint16 lemma_len);

    /// 移除词条
    bool remove_lemma(LemmaIdType lemma_id);

    /// 获取总词条数
    size_t get_total_lemma_count();

    /// 设置其他词典的总词频
    void set_total_lemma_count_of_others(size_t count);

    /// 刷新缓存到磁盘
    void flush_cache();

    /**
   * 设置词典限制参数
   *
   * @param max_lemma_count 最大词条数
   * @param max_lemma_size 最大词典尺寸
   * @param reclaim_ratio 回收比例（1-100）
   */
    void set_limit(uint32 max_lemma_count, uint32 max_lemma_size,
                   uint32 reclaim_ratio);

    /// 回收空间
    void reclaim();

    /// 碎片整理
    void defragment();

#ifdef ___SYNC_ENABLED___
    /// 清除指定范围的同步词条
    void clear_sync_lemmas(unsigned int start, unsigned int end);

    /// 获取待同步词条数
    int get_sync_count();

    /**
   * 添加词条（不带同步标记）
   *
   * @param lmt 最后修改时间戳
   * @return 词条ID
   */
    LemmaIdType put_lemma_no_sync(char16 lemma_str[], uint16 splids[],
                                  uint16 lemma_len, uint16 count, uint64 lmt);

    /**
    * 从UTF16LE字符串添加词条（不带同步标记）
    *
    * @param lemmas 格式：'wo men,WM,0.32;da jia,DJ,0.12'
    * @param len UTF16LE字符串长度
    * @return 新增词条数
    */
    int put_lemmas_no_sync_from_utf16le_string(char16 * lemmas, int len);

    /**
   * 获取待同步词条（UTF16LE格式）
   *
   * @param str 输出缓冲区
   * @param size 缓冲区大小
   * @param count 返回词条数
   * @return UTF16LE字符串长度
   *
   * @warning 缓冲区必须足够大，否则可能死循环
   */
    int get_sync_lemmas_in_utf16le_string_from_beginning(
        char16 * str, int size, int * count);

#endif

    /// 词典统计信息结构
    struct UserDictStat {
        uint32 version;       ///< 版本号
        const char * file_name;  ///< 文件名
        struct timeval load_time;  ///< 加载时间
        struct timeval last_update;  ///< 最后更新时间
        uint32 disk_size;     ///< 磁盘大小
        uint32 lemma_count;   ///< 词条数
        uint32 lemma_size;    ///< 词条总大小
        uint32 delete_count;  ///< 删除词条数
        uint32 delete_size;   ///< 删除词条大小
#ifdef ___SYNC_ENABLED___
        uint32 sync_count;    ///< 同步词条数
#endif
        uint32 reclaim_ratio; ///< 回收比例
        uint32 limit_lemma_count;  ///< 最大词条数限制
        uint32 limit_lemma_size;   ///< 最大词典尺寸限制
    };

    /// 获取词典状态
    bool state(UserDictStat * stat);

private:
    // ===================== 词典核心数据结构 =====================
    uint32 total_other_nfreq_;  ///< 其他词典总词频
    struct timeval load_time_;  ///< 加载时间
    LemmaIdType start_id_;      ///< 起始词条ID
    uint32 version_;            ///< 词典版本
    uint8 * lemmas_;            ///< 词条存储区

    /// 词条标志位
    static const uint8 kUserDictLemmaFlagRemove = 1;  ///< 删除标志

    /// 词条偏移数组（最高位标记删除状态）
    uint32 * offsets_;
    static const uint32 kUserDictOffsetFlagRemove = (1 << 31);  ///< 偏移删除标志
    static const uint32 kUserDictOffsetMask = ~(kUserDictOffsetFlagRemove);  ///< 偏移掩码

    /// 时间戳参数
    static const uint32 kUserDictLMTBitWidth = 16;     ///< 时间戳位宽
    static const uint32 kUserDictLMTGranularity = 60 * 60 * 24 * 7;  ///< 时间戳粒度（秒）
    static const uint16 kUserDictMaxFrequency = 0xFFFF;  ///< 最大词频

    /// 粗糙UTC时间计算宏
#define COARSE_UTC(year, month, day, hour, minute, second) \
    ( \
            (year - 1970) * 365 * 24 * 60 * 60 + \
              (month - 1) * 30 * 24 * 60 * 60 + \
              (day - 1) * 24 * 60 * 60 + \
              (hour - 0) * 60 * 60 + \
              (minute - 0) * 60 + \
        (second - 0) \
        )
        static const uint64 kUserDictLMTSince = COARSE_UTC(2009, 1, 1, 0, 0, 0);  ///< 起始时间戳

    /// 分数数组（与offsets_对应）
    uint32 * scores_;

    /// ID数组（仅内存有效）
    uint32 * ids_;
#ifdef ___PREDICT_ENABLED___
    uint32 * predicts_;  ///< 预测索引数组
#endif
#ifdef ___SYNC_ENABLED___
    uint32 * syncs_;       ///< 同步索引数组
    size_t sync_count_size_;  ///< 同步计数大小
#endif
    uint32 * offsets_by_id_;  ///< 按ID排序的偏移数组

    size_t lemma_count_left_;  ///< 剩余词条数
    size_t lemma_size_left_;   ///< 剩余词条大小

    const char * dict_file_;   ///< 词典文件路径

    /// 词典信息结构（确保4字节对齐）
    struct UserDictInfo {
        uint32 reclaim_ratio;    ///< 回收比例（1-100）
        uint32 limit_lemma_count;  ///< 最大词条数限制（0=无限制）
        uint32 limit_lemma_size;   ///< 最大词典尺寸限制（0=无限制）
        uint32 lemma_count;      ///< 总词条数（含删除）
        uint32 lemma_size;       ///< 词条总大小（含空闲）
        uint32 free_count;       ///< 空闲词条数
        uint32 free_size;        ///< 空闲词条大小
#ifdef ___SYNC_ENABLED___
        uint32 sync_count;       ///< 同步词条数
#endif
        int32 total_nfreq;       ///< 总归一化词频
    } dict_info_;

    /// 版本常量
    static const uint32 kUserDictVersion = 0x0ABCDEF0;

    /// 预分配参数
    static const uint32 kUserDictPreAlloc = 32;            ///< 预分配数量
    static const uint32 kUserDictAverageNchar = 8;         ///< 平均词条长度

    /// 词典状态枚举
    enum UserDictState {
        USER_DICT_NONE = 0,      ///< 未初始化
        USER_DICT_SYNC,          ///< 同步状态
#ifdef ___SYNC_ENABLED___
        USER_DICT_SYNC_DIRTY,    ///< 同步数据脏标记
#endif
        USER_DICT_SCORE_DIRTY,   ///< 分数数据脏标记
        USER_DICT_OFFSET_DIRTY,  ///< 偏移数据脏标记
        USER_DICT_LEMMA_DIRTY,   ///< 词条数据脏标记
        USER_DICT_DEFRAGMENTED,  ///< 已碎片整理
    } state_;

    /// 可搜索词条结构
    struct UserDictSearchable {
        uint16 splids_len;           ///< 拼音ID长度
        uint16 splid_start[kMaxLemmaSize];  ///< 拼音起始位置
        uint16 splid_count[kMaxLemmaSize];  ///< 拼音数量
        uint32 signature[kMaxLemmaSize / 4];  ///< 签名（用于模糊匹配）
    };

#ifdef ___CACHE_ENABLED___
    /// 缓存类型枚举
    enum UserDictCacheType {
        USER_DICT_CACHE,        ///< 常规缓存
        USER_DICT_MISS_CACHE,   ///< 未命中缓存
    };

    /// 缓存大小参数
    static const int kUserDictCacheSize = 4;               ///< 常规缓存大小
    static const int kUserDictMissCacheSize = kMaxLemmaSize - 1;  ///< 未命中缓存大小

    /// 未命中缓存结构
    struct UserDictMissCache {
        uint32 signatures[kUserDictMissCacheSize][kMaxLemmaSize / 4];  ///< 签名数组
        uint16 head, tail;  ///< 环形缓冲区指针
    } miss_caches_[kMaxLemmaSize];  ///< 按词长分组的未命中缓存

    /// 常规缓存结构
    struct UserDictCache {
        uint32 signatures[kUserDictCacheSize][kMaxLemmaSize / 4];  ///< 签名数组
        uint32 offsets[kUserDictCacheSize];  ///< 偏移数组
        uint32 lengths[kUserDictCacheSize];  ///< 长度数组
        uint16 head, tail;  ///< 环形缓冲区指针
    } caches_[kMaxLemmaSize];  ///< 按词长分组的常规缓存

    /// 初始化缓存
    void cache_init();

    /// 压入缓存
    void cache_push(UserDictCacheType type,
                    UserDictSearchable *searchable,
                    uint32 offset, uint32 length);

    /// 检查缓存命中
    bool cache_hit(UserDictSearchable *searchable,
                   uint32 *offset, uint32 *length);

    /// 加载缓存
    bool load_cache(UserDictSearchable *searchable,
                    uint32 *offset, uint32 *length);

    /// 保存缓存
    void save_cache(UserDictSearchable *searchable,
                    uint32 offset, uint32 length);

    /// 重置缓存
    void reset_cache();

    /// 加载未命中缓存
    bool load_miss_cache(UserDictSearchable *searchable);

    /// 保存未命中缓存
    void save_miss_cache(UserDictSearchable *searchable);

    /// 重置未命中缓存
    void reset_miss_cache();
#endif

    /// 转换原始分数为LmaScoreType
    LmaScoreType translate_score(int f);

    /// 从原始分数提取频率
    int extract_score_freq(int raw_score);

    /// 从原始分数提取时间戳
    uint64 extract_score_lmt(int raw_score);

    /// 构建原始分数
    inline int build_score(uint64 lmt, int freq);

    /// UTF16LE字符串转int64
    inline int64 utf16le_atoll(uint16 *s, int len);

    /// int64转UTF16LE字符串
    inline int utf16le_lltoa(int64 v, uint16 *s, int size);

    /// 内部添加词条实现
    LemmaIdType _put_lemma(char16 lemma_str[], uint16 splids[],
                           uint16 lemma_len, uint16 count, uint64 lmt);

    /// 内部获取候选词条实现
    size_t _get_lpis(const uint16 *splid_str, uint16 splid_str_len,
                     LmaPsbItem *lpi_items, size_t lpi_max, bool * need_extend);

    /// 内部获取词条分数（通过汉字）
    int _get_lemma_score(char16 lemma_str[], uint16 splids[], uint16 lemma_len);

    /// 内部获取词条分数（通过ID）
    int _get_lemma_score(LemmaIdType lemma_id);

    /// 模糊匹配拼音ID前缀
    int is_fuzzy_prefix_spell_id(const uint16 * id1, uint16 len1,
                                 const UserDictSearchable *searchable);

    /// 精确匹配拼音ID前缀
    bool is_prefix_spell_id(const uint16 * fullids,
                            uint16 fulllen, const UserDictSearchable *searchable);

    /// 获取词典文件大小
    uint32 get_dict_file_size(UserDictInfo * info);

    /// 重置词典
    bool reset(const char *file);

    /// 验证词典文件
    bool validate(const char *file);

    /// 加载词典
    bool load(const char *file, LemmaIdType start_id);

    /// 检查词典状态有效性
    bool is_valid_state();

    /// 检查词条ID有效性
    bool is_valid_lemma_id(LemmaIdType id);

    /// 获取最大词条ID
    LemmaIdType get_max_lemma_id();

    /// 设置词条标志
    void set_lemma_flag(uint32 offset, uint8 flag);

    /// 获取词条标志
    char get_lemma_flag(uint32 offset);

    /// 获取词条汉字数
    char get_lemma_nchar(uint32 offset);

    /// 获取词条拼音ID
    uint16 * get_lemma_spell_ids(uint32 offset);

    /// 获取词条汉字
    uint16 * get_lemma_word(uint32 offset);

    /// 准备搜索结构（加速定位）
    void prepare_locate(UserDictSearchable *searchable,
                        const uint16 * splids, uint16 len);

    /// 模糊匹配拼音ID（仅首字母）
    int32 fuzzy_compare_spell_id(const uint16 * id1, uint16 len1,
                                 const UserDictSearchable *searchable);

    /// 精确匹配拼音ID
    bool equal_spell_id(const uint16 * fullids,
                        uint16 fulllen, const UserDictSearchable *searchable);

    /// 定位首个匹配词条
    int32 locate_first_in_offsets(const UserDictSearchable *searchable);

    /// 追加词条到词典
    LemmaIdType append_a_lemma(char16 lemma_str[], uint16 splids[],
                               uint16 lemma_len, uint16 count, uint64 lmt);

    /// 定位词条在词典中的位置
    int32 locate_in_offsets(char16 lemma_str[],
                            uint16 splid_str[], uint16 lemma_len);

    /// 通过偏移索引移除词条
    bool remove_lemma_by_offset_index(int offset_index);
#ifdef ___PREDICT_ENABLED___
    /// 定位预测列表中的插入位置
    uint32 locate_where_to_insert_in_predicts(const uint16 * words,
                                              int lemma_len);

    /// 定位预测列表中的首个匹配项
    int32 locate_first_in_predicts(const uint16 * words, int lemma_len);

    /// 从预测列表中移除词条
    void remove_lemma_from_predict_list(uint32 offset);
#endif
#ifdef ___SYNC_ENABLED___
    /// 将词条加入同步队列
    void queue_lemma_for_sync(LemmaIdType id);

    /// 从同步列表移除词条
    void remove_lemma_from_sync_list(uint32 offset);

    /// 写回同步数据
    void write_back_sync(int fd);
#endif
    /// 写回分数数据
    void write_back_score(int fd);
    /// 写回偏移数据
    void write_back_offset(int fd);
    /// 写回词条数据
    void write_back_lemma(int fd);
    /// 写回全部数据
    void write_back_all(int fd);
    /// 整体写回操作
    void write_back();

    /// 分数-偏移对结构（用于排序）
    struct UserDictScoreOffsetPair {
        int score;            ///< 分数
        uint32 offset_index;  ///< 偏移索引
    };

    /// 交换排序元素
    inline void swap(UserDictScoreOffsetPair * sop, int i, int j);

    /// 堆排序下移操作
    void shift_down(UserDictScoreOffsetPair * sop, int i, int n);

    // ===================== 磁盘存储格式说明 =====================
    // 版本号（4字节）
    // 词条数据区：
    //   [保留字节][汉字数][拼音ID数组][汉字数组]...
    // 偏移索引区：
    //   [按拼音排序的偏移1]...[按拼音排序的偏移N]
#ifdef ___PREDICT_ENABLED___
//   [按汉字排序的偏移1]...[按汉字排序的偏移N]
#endif
// 分数数据区：
//   [分数1]...[分数N]
#ifdef ___SYNC_ENABLED___
// 同步标记区：
//   [同步标记1]...[同步标记N]
#endif
    // 词典信息区（固定大小）
};
}

#endif
