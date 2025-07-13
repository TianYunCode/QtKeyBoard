#ifndef PINYINIME_INCLUDE_ANDPYIME_H__
#define PINYINIME_INCLUDE_ANDPYIME_H__

#include <stdlib.h>
#include "./dictdef.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace ime_pinyin {

/**
 * 通过系统词典和用户词典文件名打开解码引擎
 *
 * @param fn_sys_dict 系统词典文件名
 * @param fn_usr_dict 用户词典文件名
 * @return 成功打开返回true
 */
bool im_open_decoder(const char *fn_sys_dict, const char *fn_usr_dict);

/**
 * 通过系统词典文件描述符和用户词典文件名打开解码引擎
 * （适用于Android场景，系统词典嵌入在应用APK中）
 *
 * @param sys_fd 包含系统词典的文件描述符
 * @param start_offset 系统词典在文件中的起始偏移
 * @param length 系统词典长度（字节）
 * @param fn_usr_dict 用户词典文件名
 * @return 成功返回true
 */
bool im_open_decoder_fd(int sys_fd, long start_offset, long length,
                        const char *fn_usr_dict);

/// 关闭解码引擎
void im_close_decoder();

/**
 * 设置解码最大限制（不调用则使用默认值）
 * 因屏幕尺寸限制，输入法UI只能显示特定数量的输入字母和输出汉字
 * 当输入/输出超出限制时，引擎将丢弃新增字母
 *
 * @param max_sps_len 最大拼音字符串长度
 * @param max_hzs_len 最大解码中文字符串长度
 */
void im_set_max_lens(size_t max_sps_len, size_t max_hzs_len);

/// 将缓存数据刷入持久存储（运行时部分数据仅存内存）
void im_flush_cache();

/**
 * 使用拼音字符串进行搜索
 * 引擎基于前次搜索结果进行增量搜索
 * 若新字符串与引擎存储的前缀相同，则从前缀末尾继续搜索
 * 全新搜索需先调用im_reset_search()
 * 等效于逐个调用im_add_letter()
 *
 * @param sps_buf 待解码拼音字符串缓冲区
 * @param sps_len 拼音字符串长度
 * @return 候选词数量
 */
size_t im_search(const char* sps_buf, size_t sps_len);

/**
 * 在当前搜索结果中执行删除操作并重新搜索
 *
 * @param pos 删除位置（拼音字符串或拼音ID结果中的位置）
 * @param is_pos_in_splid 指示pos参数类型：
 *        true-拼音ID结果位置, false-拼音字符串位置
 * @param clear_fixed_this_step 是否清除本步固定状态
 * @return 候选词数量
 */
size_t im_delsearch(size_t pos, bool is_pos_in_splid,
                    bool clear_fixed_this_step);

/// 重置先前搜索结果
void im_reset_search();

/**
 * 向解码器维护的拼音字符串添加字母
 * 失败时不执行操作，可通过im_get_sps_str()获取当前拼音字符串
 *
 * @param ch 待添加字母
 * @return 候选词数量
 */
size_t im_add_letter(char ch);

/**
 * 获取解码器维护的拼音字符串
 *
 * @param decoded_len 返回已成功解析的字符数
 * @return 拼音字符串指针
 */
const char *im_get_sps_str(size_t *decoded_len);

/**
 * 获取候选字符串
 *
 * @param cand_id 候选ID（0开始，通常ID0为整句候选）
 * @param cand_str 候选字符串缓冲区
 * @param max_len 缓冲区最大容量
 * @return 成功返回cand_str，失败返回NULL
 */
char16* im_get_candidate(size_t cand_id, char16* cand_str,
                         size_t max_len);

/**
 * 获取拼音字符串的切分信息（起始位置）
 *
 * @param spl_start 返回起始位置数组
 * @return 拼音ID数量L（数组包含L+1个有效元素，spl_start[L]为最后一个拼音ID结束位置）
 */
size_t im_get_spl_start_pos(const uint16 *&spl_start);

/**
 * 选择候选词并固定
 * 若候选词未匹配所有拼音ID，将从首个未固定位置提供新候选
 * 若候选词匹配所有拼音ID，则仅有一个候选（完整固定句）
 *
 * @param cand_id 待选择的候选ID
 * @return 候选词数量（完全固定后仅返回1个候选）
 */
size_t im_choose(size_t cand_id);

/**
 * 取消上次选择（回退im_choose()操作）
 *
 * @return 候选词数量
 */
size_t im_cancel_last_choice();

/**
 * 获取已固定的拼音ID或中文字符数量
 *
 * @return 固定元素数量
 */
size_t im_get_fixed_len();

/// 取消输入状态并重置搜索空间
bool im_cancel_input();

/**
 * 基于给定历史中文字符串获取预测候选
 *
 * @param his_buf 历史缓冲区（以'\0'结尾）
 * @param pre_buf 返回预测结果列表
 * @return 预测结果数量
 */
size_t im_get_predicts(const char16 *his_buf,
                       char16 (*&pre_buf)[kMaxPredictSize + 1]);

/// 启用首字母模式下的声母
void im_enable_shm_as_szm(bool enable);

/// 启用首字母模式下的韵母
void im_enable_ym_as_szm(bool enable);

/**
 * 初始化/反初始化用户词典
 *
 * @param fn_usr_dict 用户词典文件名
 */
void im_init_user_dictionary(const char *fn_usr_dict);

/// 返回用户词典当前状态
bool im_is_user_dictionary_enabled(void);
}

#ifdef __cplusplus
}
#endif

#endif  // PINYINIME_INCLUDE_ANDPYIME_H__
