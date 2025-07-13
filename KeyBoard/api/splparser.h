#ifndef PINYINIME_INCLUDE_SPLPARSER_H__
#define PINYINIME_INCLUDE_SPLPARSER_H__

#include "./dictdef.h"
#include "./spellingtrie.h"

namespace ime_pinyin {

/**
 * 拼音解析器类（实现拼音字符串到拼音ID流的转换）
 */
class SpellingParser {
protected:
    const SpellingTrie *spl_trie_;  ///< 拼音Trie树引用

public:
    SpellingParser();  ///< 构造函数

    /**
   * 将拼音字符串解析为拼音ID流
   *
   * @param splstr 拼音字符串
   * @param str_len 字符串长度
   * @param splidx 返回的拼音ID数组
   * @param start_pos 返回的起始位置数组
   * @param max_size 数组最大容量
   * @param last_is_pre 返回状态：
   *        - 若整个字符串成功解析，设为true
   *        - 若部分解析，设为最后一个部分是否为完整拼音的前缀
   * @return 解析的拼音ID数量
   *
   * @note 若首字符非['a'-'z']（为分隔符），返回0
   * @note 分隔符只能出现在字符串中间或末尾
   */
    uint16 splstr_to_idxs(const char *splstr, uint16 str_len, uint16 splidx[],
                          uint16 start_pos[], uint16 max_size, bool &last_is_pre);

    /**
   * 功能同splstr_to_idxs()，但将单字符韵母转为完整ID（而非半ID）
   */
    uint16 splstr_to_idxs_f(const char *splstr, uint16 str_len, uint16 splidx[],
                            uint16 start_pos[], uint16 max_size, bool &last_is_pre);

    /**
   * 功能同splstr_to_idxs()，但使用UTF-16字符串
   */
    uint16 splstr16_to_idxs(const char16 *splstr, uint16 str_len, uint16 splidx[],
                            uint16 start_pos[], uint16 max_size, bool &last_is_pre);

    /**
   * 功能同splstr_to_idxs_f()，但使用UTF-16字符串
   */
    uint16 splstr16_to_idxs_f(const char16 *splstr16, uint16 str_len,
                              uint16 splidx[], uint16 start_pos[],
                              uint16 max_size, bool &last_is_pre);

    /**
   * 获取拼音字符串对应的拼音ID
   *
   * @param splstr 拼音字符串
   * @param str_len 字符串长度
   * @param is_pre 返回是否为完整拼音的前缀
   * @return 拼音ID（无效返回0）
   *
   * @note 单字符韵母（如"A"）在首字母模式下返回半ID
   */
    uint16 get_splid_by_str(const char *splstr, uint16 str_len, bool *is_pre);

    /**
   * 获取拼音字符串对应的拼音ID（单字符韵母始终返回完整ID）
   */
    uint16 get_splid_by_str_f(const char *splstr, uint16 str_len, bool *is_pre);

    /// 检查字符是否可解析（排除分隔符）
    bool is_valid_to_parse(char ch);

    /**
   * 获取并行动态规划解析结果（自动纠错未实现）
   *
   * @param full_id_num 返回完整拼音ID数量
   * @param is_pre 返回是否为完整拼音的前缀
   * @return 拼音ID总数
   *
   * @note 首字符为分隔符时返回0
   * @note 调用者需确保参数非空且str_len>0
   */
    uint16 get_splids_parallel(const char *splstr, uint16 str_len,
                               uint16 splidx[], uint16 max_size,
                               uint16 &full_id_num, bool &is_pre);
};
}

#endif  // PINYINIME_INCLUDE_SPLPARSER_H__
