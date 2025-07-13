#ifndef PINYINIME_INCLUDE_DICTDEF_H__
#define PINYINIME_INCLUDE_DICTDEF_H__

#include <stdlib.h>
#include "./utf16char.h"

namespace ime_pinyin {

// 构建二进制字典模型时启用以下行
// #define ___BUILD_MODEL___

typedef unsigned char      uint8;   ///< 8位无符号整数
typedef unsigned short     uint16;  ///< 16位无符号整数
typedef unsigned int       uint32;  ///< 32位无符号整数

typedef signed char        int8;    ///< 8位有符号整数
typedef short              int16;   ///< 16位有符号整数
typedef int                int32;   ///< 32位有符号整数
typedef long long          int64;   ///< 64位有符号整数
typedef unsigned long long uint64;  ///< 64位无符号整数

const bool kPrintDebug0 = false;  ///< 调试信息输出级别0
const bool kPrintDebug1 = false;  ///< 调试信息输出级别1
const bool kPrintDebug2 = false;  ///< 调试信息输出级别2

// 词条最大长度
const size_t kMaxLemmaSize = 8;

// 拼音（拼写）最大长度
const size_t kMaxPinyinSize = 6;

// 半拼音ID数量（中文拼音为29个）
const size_t kHalfSpellingIdNum = 29;

// 最大完整拼音数量（中文约410个）
// 若需增大此值，需同时更新SpellingNode等结构确保能存储拼音ID
const size_t kMaxSpellingNum = 512 - kHalfSpellingIdNum - 1;  // -1因0不使用
const size_t kMaxSearchSteps = 40;  ///< 最大搜索步数

// 单字符预测后续字符的最大长度
const size_t kMaxPredictSize = (kMaxLemmaSize - 1);

// 词条ID类型（必须为size_t）
typedef size_t LemmaIdType;
const size_t kLemmaIdSize = 3;  ///< 存储中实际占用3字节
const size_t kLemmaIdComposing = 0xffffff;  ///< 组合输入标识

typedef uint16 LmaScoreType;  ///< 词条分数类型
typedef uint16 KeyScoreType;  ///< 按键分数类型

// 为预测目的保留的最高分词条数量
const size_t kTopScoreLemmaNum = 10;

// 不同长度词条的最大预测数量
const size_t kMaxPredictNumByGt3 = 1;  ///> 3字以上预测数
const size_t kMaxPredictNumBy3 = 2;    ///> 3字词预测数
const size_t kMaxPredictNumBy2 = 2;    ///> 2字词预测数

// 系统词典词条ID范围（含结束值）
const LemmaIdType kSysDictIdEnd = 500000;

// 用户词典起始ID
const LemmaIdType kUserDictIdStart = 500001;

// 用户词典词条ID范围（含结束值）
const LemmaIdType kUserDictIdEnd = 600000;

/// 拼音ID结构（半ID占5位，全ID占11位）
typedef struct {
    uint16 half_splid:5;  ///< 半拼音ID
    uint16 full_splid:11; ///< 完整拼音ID
} SpellingId, *PSpellingId;


/**
 * 不同层级使用不同节点类型
 * 测试字典构建结果统计：
 *              | 根节点 | 0级节点 | 1级节点 | 2级节点 | 3级节点
 * 最大子节点数 | 406   | 280     | 41      | 2       | -
 * 最大同音词数 | 0     | 90      | 23      | 2       | 2
 * 节点总数     | 1     | 406     | 31766   | 13516   | 993
 * 同音词总数   | 9     | 5674    | 44609   | 12667   | 995
 *
 * 根节点和0级节点数不超过500
 * 据此使用两类节点：根/0级节点，及>0级节点
 *
 * LE = 小于等于0级
 * 节点占16字节，总空间 < 16*500 = 8KB
 */
struct LmaNodeLE0 {
    uint32 son_1st_off;      ///< 首子节点偏移
    uint32 homo_idx_buf_off; ///< 同音词索引缓冲区偏移
    uint16 spl_idx;          ///< 拼音ID索引
    uint16 num_of_son;       ///< 子节点数量
    uint16 num_of_homo;      ///< 同音词数量
};

/**
 * GE = 大于等于1级
 * 节点占8字节
 */
struct LmaNodeGE1 {
    uint16 son_1st_off_l;        ///< 首子节点偏移低16位
    uint16 homo_idx_buf_off_l;   ///< 同音词索引缓冲区偏移低16位
    uint16 spl_idx;              ///< 拼音ID索引
    unsigned char num_of_son;    ///< 子节点数量
    unsigned char num_of_homo;   ///< 同音词数量
    unsigned char son_1st_off_h;         ///< 首子节点偏移高8位
    unsigned char homo_idx_buf_off_h;    ///< 同音词索引缓冲区偏移高8位
};

#ifdef ___BUILD_MODEL___
/// 单字符项定义
struct SingleCharItem {
    float freq;      ///< 使用频率
    char16 hz;      ///< 汉字
    SpellingId splid;///< 拼音ID
};

/// 词条项定义
struct LemmaEntry {
    LemmaIdType idx_by_py;  ///< 按拼音索引ID
    LemmaIdType idx_by_hz;  ///< 按汉字索引ID
    char16 hanzi_str[kMaxLemmaSize + 1];  ///< 汉字字符串

    /// 每个汉字对应的单字符项ID
    uint16 hanzi_scis_ids[kMaxLemmaSize];

    uint16 spl_idx_arr[kMaxLemmaSize + 1];  ///< 拼音ID数组
    char pinyin_str[kMaxLemmaSize][kMaxPinyinSize + 1]; ///< 拼音字符串数组
    unsigned char hz_str_len;  ///< 汉字串长度
    float freq;                ///< 词频
};
#endif  // ___BUILD_MODEL___

}  //  namespace ime_pinyin

#endif  // PINYINIME_INCLUDE_DICTDEF_H__
