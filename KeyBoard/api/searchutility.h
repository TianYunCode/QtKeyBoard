#ifndef PINYINIME_ANDPY_INCLUDE_SEARCHCOMMON_H__
#define PINYINIME_ANDPY_INCLUDE_SEARCHCOMMON_H__

#include <stdlib.h>
#include "./spellingtrie.h"

namespace ime_pinyin {

typedef uint16 PoolPosType;  ///< 池位置类型（用于标识池大小，如ID池等）
typedef uint16 MileStoneHandle;  ///< 里程碑句柄类型（用于标识原子词典中的解析里程碑）

/// 词条及其概率分数结构
typedef struct {
    size_t id:(kLemmaIdSize * 8);  ///< 词条ID
    size_t lma_len:4;              ///< 词条长度
    uint16 psb;                    ///< 概率分数（值越低可能性越高）
    char16 hanzi;                  ///< 单字符项对应的汉字（多字符项忽略）
} LmaPsbItem, *PLmaPsbItem;

/// 扩展字符串的词条概率分数结构
typedef struct {
    LmaPsbItem lpi;                ///< 基础词条信息
    char16 str[kMaxLemmaSize + 1]; ///< 词条字符串
} LmaPsbStrItem, *PLmaPsbStrItem;

/// 预测项结构
typedef struct {
    float psb;                     ///< 概率值
    char16 pre_hzs[kMaxPredictSize]; ///< 预测汉字串
    uint16 his_len;                ///< 用于预测的历史长度
} NPredictItem, *PNPredictItem;

/**
 * 词典扩展参数结构（所有词典接收相同的DictExtPara和词典特定的MileStoneHandle进行扩展）
 *
 * 用户输入新字符时，每个词典的AtomDictBase::extend_dict()至少被调用一次
 *
 * 示例：输入"wm"时extend_dict()调用两次，参数分别为：
 * 1. splids={w,m}; splids_extended=1; ext_len=1; step_no=1; splid_end_split=false;
 *    id_start=wa; id_num='w'开头的ID数
 * 2. splids={m}; splids_extended=0; ext_len=1; step_no=1; splid_end_split=false;
 *    id_start=wa; id_num='w'开头的ID数
 *
 * 输入"women"时可能的参数：
 * splids={wo,men}; splids_extended=1; ext_len=3; step_no=4; splid_end_split=false;
 * id_start=men; id_num=1
 */
typedef struct {
    uint16 splids[kMaxSearchSteps];  ///< 待扩展拼音ID数组（含splids_extended+1个ID）
    uint16 splids_extended;          ///< 先前已使用的ID数（splids[splids_extended]为新增ID）
    uint16 ext_len;                  ///< 扩展步长（新增拼音ID的字符串长度）
    uint16 step_no;                  ///< 当前扩展步号（输入拼音字符串中的结束位置）
    bool splid_end_split;            ///< 新增拼音ID是否以分隔符结束
    uint16 id_start;                 ///< 新增ID对应的起始ID（半ID时为对应全ID的首个）
    uint16 id_num;                   ///< 新增ID对应的ID数量（全ID时为1）
}DictExtPara, *PDictExtPara;

/// 检查词条ID是否属于系统词典
bool is_system_lemma(LemmaIdType lma_id);
/// 检查词条ID是否属于用户词典
bool is_user_lemma(LemmaIdType lma_id);
/// 检查词条ID是否为组词短语
bool is_composing_lemma(LemmaIdType lma_id);

// ===================== 比较函数集 =====================
int cmp_lpi_with_psb(const void *p1, const void *p2);          ///< 按概率分数比较词条
int cmp_lpi_with_unified_psb(const void *p1, const void *p2);  ///< 按统一概率分数比较词条
int cmp_lpi_with_id(const void *p1, const void *p2);           ///< 按ID比较词条
int cmp_lpi_with_hanzi(const void *p1, const void *p2);        ///< 按汉字比较词条
int cmp_lpsi_with_str(const void *p1, const void *p2);         ///< 按字符串比较词条

// 按长度分组的汉字比较函数
int cmp_hanzis_1(const void *p1, const void *p2);  ///< 1字词比较
int cmp_hanzis_2(const void *p1, const void *p2);  ///< 2字词比较
int cmp_hanzis_3(const void *p1, const void *p2);  ///< 3字词比较
int cmp_hanzis_4(const void *p1, const void *p2);  ///< 4字词比较
int cmp_hanzis_5(const void *p1, const void *p2);  ///< 5字词比较
int cmp_hanzis_6(const void *p1, const void *p2);  ///< 6字词比较
int cmp_hanzis_7(const void *p1, const void *p2);  ///< 7字词比较
int cmp_hanzis_8(const void *p1, const void *p2);  ///< 8字词比较

// 预测项比较函数
int cmp_npre_by_score(const void *p1, const void *p2);           ///< 按分数比较预测项
int cmp_npre_by_hislen_score(const void *p1, const void *p2);    ///< 按历史长度和分数比较
int cmp_npre_by_hanzi_score(const void *p1, const void *p2);     ///< 按汉字和分数比较

/// 去除重复预测项
size_t remove_duplicate_npre(NPredictItem *npre_items, size_t npre_num);

/// 将大小对齐到size_t边界
size_t align_to_size_t(size_t size);

}  // namespace

#endif  // PINYINIME_ANDPY_INCLUDE_SEARCHCOMMON_H__
