/**
 * 本类定义了AtomDictBase类，它是所有原子词典的基类。
 * 原子词典由解码器类MatrixSearch管理。
 *
 * 当用户向拼音字符串追加新字符时，所有启用的原子词典的extend_dict()
 * 将被至少调用一次，以获取在此步结束的候选（参数中同时提供起始步信息）。
 * 通常调用extend_dict()时，会传入先前步骤返回的MileStoneHandle对象
 * 以加速查找过程，若扩展成功将返回新的MileStoneHandle对象。
 *
 * 返回的MileStoneHandle对象应保持有效，直到调用reset_milestones()函数
 * 并通知该对象被重置为止。
 *
 * 原子词典通常使用步信息管理其MileStoneHandle对象，
 * 或按升序排列对象以简化重置操作。
 *
 * 当解码器加载词典时，会为原子词典提供起始词条ID（lemma id），
 * 用于将内部ID映射到全局ID。原子词典与外部组件交互时需使用全局ID。
 */

#ifndef PINYINIME_INCLUDE_ATOMDICTBASE_H__
#define PINYINIME_INCLUDE_ATOMDICTBASE_H__

#include <stdlib.h>
#include "./dictdef.h"
#include "./searchutility.h"

namespace ime_pinyin {
class AtomDictBase {
public:
    virtual ~AtomDictBase() {}

    /**
   * 从文件加载原子词典
   *
   * @param file_name 要加载的词典文件名
   * @param start_id 本词典使用的起始ID
   * @param end_id 本词典可用的结束ID（含）。用户词典始终使用最后的ID空间，
   *        可忽略此参数；其他原子词典应检查此参数
   * @return 成功返回true
   */
    virtual bool load_dict(const char *file_name, LemmaIdType start_id,
                           LemmaIdType end_id) = 0;

    /**
   * 关闭原子词典
   *
   * @return 成功返回true
   */
    virtual bool close_dict() = 0;

    /**
   * 获取词典中的总词条数
   *
   * @return 总词条数
   */
    virtual size_t number_of_lemmas() = 0;

    /**
   * 当用户从输入字符串删除字符或开始新输入时，
   * 由解码器调用此函数。
   *
   * 不同原子词典的实现方式可能不同。原子词典可根据具体实现，
   * 使用以下任一参数（或同时使用）重置对应的MileStoneHandle对象：
   *
   * 示例：若原子词典使用步信息管理MileStoneHandle对象，
   * 可用from_step标识需重置的对象；若另一原子词典未使用详细步信息，
   * 仅使用升序句柄（同一步骤中，先调用者句柄更小），
   * 则可轻松重置大于from_handle的句柄。
   *
   * 解码器始终按步重置解码状态。开始重置时，会调用原子词典的reset_milestones()，
   * 并传入步信息和该步最早调用extend_dict()返回的句柄。
   *
   * 若原子词典未实现增量搜索，可完全忽略此函数。
   *
   * @param from_step 从该步（含）开始重置MileStoneHandle对象
   * @param from_handle 步骤from_step的最早MileStoneHandle对象
   */
    virtual void reset_milestones(uint16 from_step,
                                  MileStoneHandle from_handle) = 0;

    /**
   * 在词典中进行扩展。返回的句柄在调用reset_milestones()前应保持有效
   *
   * @param from_handle 先前返回的扩展句柄（不含新拼音ID），用于加速扩展
   * @param dep 扩展参数
   * @param lpi_items 用于填充匹配的词条
   * @param lpi_max 缓冲区长度
   * @param lpi_num 返回新增项数量
   * @return 扩展的新里程碑句柄，失败返回0
   */
    virtual MileStoneHandle extend_dict(MileStoneHandle from_handle,
                                        const DictExtPara *dep,
                                        LmaPsbItem *lpi_items,
                                        size_t lpi_max, size_t *lpi_num) = 0;

    /**
   * 根据拼音ID流获取带分数的词条项。
   * 原子词典无需对返回项排序。
   *
   * @param splid_str 拼音ID流缓冲区
   * @param splid_str_len 拼音ID流长度
   * @param lpi_items 返回匹配的词条项（含分数）
   * @param lpi_max 返回结果缓冲区最大长度
   * @return 已填入lpi_items的匹配项数量
   */
    virtual size_t get_lpis(const uint16 *splid_str, uint16 splid_str_len,
                            LmaPsbItem *lpi_items, size_t lpi_max) = 0;

    /**
   * 通过词条ID获取词条字符串（中文字符串）
   *
   * @param id_lemma 要获取字符串的词条ID
   * @param str_buf 返回中文字符串的缓冲区
   * @param str_max 缓冲区最大长度
   * @return 字符串长度（失败返回0）
   */
    virtual uint16 get_lemma_str(LemmaIdType id_lemma, char16 *str_buf,
                                 uint16 str_max) = 0;

    /**
   * 获取给定词条ID的完整拼音ID。
   * 若缓冲区过短则返回0。
   *
   * @param splids 返回拼音ID的缓冲区
   * @param splids_max splids的最大缓冲区长度
   * @param arg_valid 指示传入参数是否已初始化且有效。
   *        为true时，splids和splids_max有效，且splids中可能存在需更新的半ID。
   *        此时splids_max表示splids中的有效ID数量。
   * @return 缓冲区中的ID数量
   */
    virtual uint16 get_lemma_splids(LemmaIdType id_lemma, uint16 *splids,
                                    uint16 splids_max, bool arg_valid) = 0;

    /**
   * 用于预测的功能。
   * 无需对新添项排序。
   *
   * @param last_hzs 最后n个中文字符（汉字），
   *        长度应小于等于kMaxPredictSize
   * @param hzs_len 指定历史记录长度（≤kMaxPredictSize）
   * @param npre_items 用于返回结果
   * @param npre_max 返回结果的缓冲区长度
   * @param b4_used 其他原子词典的预测结果数量（从npre_items[-b4_used]开始）。
   *        原子词典可忽略此参数
   * @return 本原子词典的预测结果数量
   */
    virtual size_t predict(const char16 last_hzs[], uint16 hzs_len,
                           NPredictItem *npre_items, size_t npre_max,
                           size_t b4_used) = 0;

    /**
   * 向词典添加词条。若词典允许添加新项且该项不存在，则添加。
   *
   * @param lemma_str 词条的中文字符串
   * @param splids 词条的拼音ID
   * @param lemma_len 中文词条长度
   * @param count 该词条的频率计数
   */
    virtual LemmaIdType put_lemma(char16 lemma_str[], uint16 splids[],
                                  uint16 lemma_len, uint16 count) = 0;

    /**
   * 更新词条的出现计数
   *
   * @param lemma_id 待更新的词条ID
   * @param delta_count 需调整的频率计数值
   * @param selected 指示该词条是否被用户选中并提交到目标编辑框
   * @return 成功返回ID，失败返回0
   */
    virtual LemmaIdType update_lemma(LemmaIdType lemma_id, int16 delta_count,
                                     bool selected) = 0;

    /**
   * 获取给定词条的词条ID
   *
   * @param lemma_str 词条的中文字符串
   * @param splids 词条的拼音ID
   * @param lemma_len 词条长度
   * @return 匹配的词条ID（失败返回0）
   */
    virtual LemmaIdType get_lemma_id(char16 lemma_str[], uint16 splids[],
                                     uint16 lemma_len) = 0;

    /**
   * 获取词条分数
   *
   * @param lemma_id 要获取分数的词条ID
   * @return 词条分数（失败返回0）
   */
    virtual LmaScoreType get_lemma_score(LemmaIdType lemma_id) = 0;

    /**
   * 获取词条分数
   *
   * @param lemma_str 词条的中文字符串
   * @param splids 词条的拼音ID
   * @param lemma_len 词条长度
   * @return 词条分数（失败返回0）
   */
    virtual LmaScoreType get_lemma_score(char16 lemma_str[], uint16 splids[],
                                         uint16 lemma_len) = 0;

    /**
   * 若词典允许，从中移除词条
   *
   * @param lemma_id 待移除的词条ID
   * @return 成功返回true
   */
    virtual bool remove_lemma(LemmaIdType lemma_id) = 0;

    /**
   * 获取本原子词典的总出现次数
   *
   * @return 总出现次数
   */
    virtual size_t get_total_lemma_count() = 0;

    /**
   * 设置其他原子词典的总出现次数
   *
   * @param count 其他原子词典的总出现次数
   */
    virtual void set_total_lemma_count_of_others(size_t count) = 0;

    /**
   * 通知原子词典将缓存数据刷新到持久存储（必要时）
   */
    virtual void flush_cache() = 0;
};
}

#endif  // PINYINIME_INCLUDE_ATOMDICTBASE_H__
