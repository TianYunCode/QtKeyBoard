#ifndef PINYINIME_INCLUDE_NGRAM_H__
#define PINYINIME_INCLUDE_NGRAM_H__

#include <stdio.h>
#include <stdlib.h>
#include "./dictdef.h"
#include <QFile>

namespace ime_pinyin {

typedef unsigned char CODEBOOK_TYPE;  ///< 码本类型定义

static const size_t kCodeBookSize = 256;  ///< 码本大小

/**
 * N元语言模型类
 */
class NGram {
public:
    static const LmaScoreType kMaxScore = 0x3fff;  ///< 词条最大分数值

    // 为减少存储空间，原始对数概率值通过kLogValueAmplifier放大
    // 使用LmaScoreType存储，分数值越低表示词频越高
    static const int kLogValueAmplifier = -800;  ///< 对数概率放大系数

    // 系统词条总词频（非真实值，仅用于调整用户词典变化时的系统词条分数）
    // 当前版本系统词频固定，后续版本考虑改为可调整
    static const size_t kSysDictTotalFreq = 100000000;  ///< 系统词典总词频

private:
    static NGram* instance_;      ///< 单例实例指针
    bool initialized_;            ///< 初始化标志
    uint32 idx_num_;              ///< 索引数量
    size_t total_freq_none_sys_;  ///< 非系统词典总词频

    /// 系统词典分数补偿值（用户添加新词条时用于归一化分数）
    float sys_score_compensation_;

#ifdef ___BUILD_MODEL___
    double *freq_codes_df_;  ///< 词频码本（双精度版，构建模型用）
#endif
    LmaScoreType *freq_codes_;   ///< 词频码本
    CODEBOOK_TYPE *lma_freq_idx_;///< 词条词频索引

public:
    NGram();   ///< 构造函数
    ~NGram();  ///< 析构函数

    /// 获取单例实例引用
    static NGram& get_instance();

    /// 保存N元模型到文件
    bool save_ngram(FILE *fp);

    /// 从文件加载N元模型
    bool load_ngram(QFile *fp);

    /// 设置非系统词典总词频
    void set_total_freq_none_sys(size_t freq_none_sys);

    /// 获取指定词条的一元概率
    float get_uni_psb(LemmaIdType lma_id);

    /**
   * 将概率值转换为分数
   *
   * @param psb 概率值
   * @return 转换后的分数（分数越低表示概率越高）
   *
   * @note 运行时需要浮点数保证精度，但存储时限制在kMaxScore内
   */
    static float convert_psb_to_score(double psb);

#ifdef ___BUILD_MODEL___
    /**
   * 构建一元模型（用于模型构建）
   *
   * @param lemma_arr 词条数组
   * @param num 词条数量
   * @param next_idx_unused 下一个未使用ID
   * @return 成功返回true
   */
    bool build_unigram(LemmaEntry *lemma_arr, size_t num,
                       LemmaIdType next_idx_unused);
#endif
};
}

#endif  // PINYINIME_INCLUDE_NGRAM_H__
