#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dictlist.h"
#include "mystdlib.h"
#include "ngram.h"
#include "searchutility.h"

namespace ime_pinyin {

// 构造函数：初始化词典列表
DictList::DictList() {
    initialized_ = false;  // 初始化状态标记
    scis_num_ = 0;         // 单字项数量
    scis_hz_ = NULL;       // 单字项汉字数组
    scis_splid_ = NULL;    // 单字项拼音ID数组
    buf_ = NULL;           // 词典内容缓冲区
    spl_trie_ = SpellingTrie::get_cpinstance();  // 获取拼音trie实例

    // 断言确保最大词长常量正确
    assert(kMaxLemmaSize == 8);

    // 初始化不同长度汉字的比较函数数组
    cmp_func_[0] = cmp_hanzis_1;  // 1字比较
    cmp_func_[1] = cmp_hanzis_2;  // 2字比较
    cmp_func_[2] = cmp_hanzis_3;  // 3字比较
    cmp_func_[3] = cmp_hanzis_4;  // 4字比较
    cmp_func_[4] = cmp_hanzis_5;  // 5字比较
    cmp_func_[5] = cmp_hanzis_6;  // 6字比较
    cmp_func_[6] = cmp_hanzis_7;  // 7字比较
    cmp_func_[7] = cmp_hanzis_8;  // 8字比较
}

// 析构函数：释放资源
DictList::~DictList() {
    free_resource();
}

// 分配资源：词典缓冲区和单字项数组
bool DictList::alloc_resource(size_t buf_size, size_t scis_num) {
    // 分配词典内容缓冲区
    buf_ = static_cast<char16*>(malloc(buf_size * sizeof(char16)));
    if (NULL == buf_)
        return false;

    scis_num_ = scis_num;

    // 分配单字项汉字数组
    scis_hz_ = static_cast<char16*>(malloc(scis_num_ * sizeof(char16)));
    if (NULL == scis_hz_)
        return false;

    // 分配单字项拼音ID数组
    scis_splid_ = static_cast<SpellingId*>
        (malloc(scis_num_ * sizeof(SpellingId)));

    if (NULL == scis_splid_)
        return false;

    return true;
}

// 释放资源
void DictList::free_resource() {
    if (NULL != buf_)
        free(buf_);
    buf_ = NULL;

    if (NULL != scis_hz_)
        free(scis_hz_);
    scis_hz_ = NULL;

    if (NULL != scis_splid_)
        free(scis_splid_);
    scis_splid_ = NULL;
}

#ifdef ___BUILD_MODEL___  // 构建模型专用代码
// 初始化词典列表（构建时使用）
bool DictList::init_list(const SingleCharItem *scis, size_t scis_num,
                         const LemmaEntry *lemma_arr, size_t lemma_num) {
    // 参数检查
    if (NULL == scis || 0 == scis_num || NULL == lemma_arr || 0 == lemma_num)
        return false;

    initialized_ = false;  // 重置初始化标记

    // 清理现有资源
    if (NULL != buf_)
        free(buf_);

    // 计算所需缓冲区大小
    size_t buf_size = calculate_size(lemma_arr, lemma_num);
    if (0 == buf_size)
        return false;

    // 分配资源
    if (!alloc_resource(buf_size, scis_num))
        return false;

    // 填充单字项数据
    fill_scis(scis, scis_num);

    // 填充词典内容
    fill_list(lemma_arr, lemma_num);

    initialized_ = true;  // 设置初始化完成标记
    return true;
}

// 计算词典缓冲区大小
size_t DictList::calculate_size(const LemmaEntry* lemma_arr, size_t lemma_num) {
    size_t last_hz_len = 0;  // 上一个词条长度
    size_t list_size = 0;    // 列表总大小
    size_t id_num = 0;       // ID计数器

    for (size_t i = 0; i < lemma_num; i++) {
        if (0 == i) {  // 处理第一个词条
            last_hz_len = lemma_arr[i].hz_str_len;

            // 验证长度和ID
            assert(last_hz_len > 0);
            assert(lemma_arr[0].idx_by_hz == 1);

            id_num++;
            // 设置起始位置和ID
            start_pos_[0] = 0;
            start_id_[0] = id_num;

            last_hz_len = 1;
            list_size += last_hz_len;
        } else {
            size_t current_hz_len = lemma_arr[i].hz_str_len;

            // 确保词条按长度排序
            assert(current_hz_len >= last_hz_len);

            if (current_hz_len == last_hz_len) {  // 相同长度词条
                list_size += current_hz_len;
                id_num++;
            } else {  // 新长度词条
                // 填充中间长度
                for (size_t len = last_hz_len; len < current_hz_len - 1; len++) {
                    start_pos_[len] = start_pos_[len - 1];
                    start_id_[len] = start_id_[len - 1];
                }

                // 设置新长度起始位置
                start_pos_[current_hz_len - 1] = list_size;

                id_num++;
                start_id_[current_hz_len - 1] = id_num;

                last_hz_len = current_hz_len;
                list_size += current_hz_len;
            }
        }
    }

    // 处理最大词长后的位置
    for (size_t i = last_hz_len; i <= kMaxLemmaSize; i++) {
        if (0 == i) {
            start_pos_[0] = 0;
            start_id_[0] = 1;
        } else {
            start_pos_[i] = list_size;
            start_id_[i] = id_num;
        }
    }

    return start_pos_[kMaxLemmaSize];  // 返回总大小
}

// 填充单字项数据
void DictList::fill_scis(const SingleCharItem *scis, size_t scis_num) {
    assert(scis_num_ == scis_num);  // 验证数量一致

    // 逐个复制单字项
    for (size_t pos = 0; pos < scis_num_; pos++) {
        scis_hz_[pos] = scis[pos].hz;            // 汉字
        scis_splid_[pos] = scis[pos].splid;      // 拼音ID
    }
}

// 填充词典内容
void DictList::fill_list(const LemmaEntry* lemma_arr, size_t lemma_num) {
    size_t current_pos = 0;

    // 复制第一个词条
    utf16_strncpy(buf_, lemma_arr[0].hanzi_str,
                  lemma_arr[0].hz_str_len);

    current_pos = lemma_arr[0].hz_str_len;

    size_t id_num = 1;

    // 复制后续词条
    for (size_t i = 1; i < lemma_num; i++) {
        utf16_strncpy(buf_ + current_pos, lemma_arr[i].hanzi_str,
                      lemma_arr[i].hz_str_len);

        id_num++;
        current_pos += lemma_arr[i].hz_str_len;
    }

    // 验证最终位置和ID计数
    assert(current_pos == start_pos_[kMaxLemmaSize]);
    assert(id_num == start_id_[kMaxLemmaSize]);
}

// 查找以特定汉字开头的双字词
char16* DictList::find_pos2_startedbyhz(char16 hz_char) {
    // 在双字词区域二分查找
    char16 *found_2w = static_cast<char16*>
        (mybsearch(&hz_char, buf_ + start_pos_[1],
                   (start_pos_[2] - start_pos_[1]) / 2,
                   sizeof(char16) * 2, cmp_hanzis_1));
    if (NULL == found_2w)
        return NULL;

    // 找到第一个匹配项
    while (found_2w > buf_ + start_pos_[1] && *found_2w == *(found_2w - 1))
        found_2w -= 2;

    return found_2w;
}
#endif  // ___BUILD_MODEL___

// 查找以特定汉字序列开头的词条
char16* DictList::find_pos_startedbyhzs(const char16 last_hzs[],
                                        size_t word_len, int (*cmp_func)(const void *, const void *)) {
    // 在指定长度区域二分查找
    char16 *found_w = static_cast<char16*>
        (mybsearch(last_hzs, buf_ + start_pos_[word_len - 1],
                   (start_pos_[word_len] - start_pos_[word_len - 1])
                       / word_len,
                   sizeof(char16) * word_len, cmp_func));

    if (NULL == found_w)
        return NULL;

    // 回溯到第一个匹配项
    while (found_w > buf_ + start_pos_[word_len -1] &&
           cmp_func(found_w, found_w - word_len) == 0)
        found_w -= word_len;

    return found_w;
}

// 预测输入：根据历史输入预测后续可能词条
size_t DictList::predict(const char16 last_hzs[], uint16 hzs_len,
                         NPredictItem *npre_items, size_t npre_max,
                         size_t b4_used) {
    // 验证输入长度
    assert(hzs_len <= kMaxPredictSize && hzs_len > 0);

    // 1. 准备工作
    int (*cmp_func)(const void *, const void *) = cmp_func_[hzs_len - 1];  // 获取比较函数
    NGram& ngram = NGram::get_instance();  // 获取N-Gram实例
    size_t item_num = 0;                   // 预测项计数器

    // 2. 执行预测
    for (uint16 pre_len = 1; pre_len <= kMaxPredictSize + 1 - hzs_len;
         pre_len++) {
        uint16 word_len = hzs_len + pre_len;  // 完整词长度

        // 查找匹配词条
        char16 *w_buf = find_pos_startedbyhzs(last_hzs, word_len, cmp_func);
        if (NULL == w_buf)
            continue;

        // 遍历所有匹配项
        while (w_buf < buf_ + start_pos_[word_len] &&
               cmp_func(w_buf, last_hzs) == 0 &&
               item_num < npre_max) {
            memset(npre_items + item_num, 0, sizeof(NPredictItem));
            // 复制预测部分
            utf16_strncpy(npre_items[item_num].pre_hzs, w_buf + hzs_len, pre_len);
            // 获取N-Gram概率
            npre_items[item_num].psb =
                ngram.get_uni_psb((size_t)(w_buf - buf_ - start_pos_[word_len - 1])
                                      / word_len + start_id_[word_len - 1]);
            npre_items[item_num].his_len = hzs_len;
            item_num++;
            w_buf += word_len;  // 移动到下一词条
        }
    }

    // 3. 过滤已存在项
    size_t new_num = 0;
    for (size_t i = 0; i < item_num; i++) {
        // 检查是否已存在
        size_t e_pos;
        for (e_pos = 1; e_pos <= b4_used; e_pos++) {
            if (utf16_strncmp((*(npre_items - e_pos)).pre_hzs, npre_items[i].pre_hzs,
                              kMaxPredictSize) == 0)
                break;
        }
        // 跳过重复项
        if (e_pos <= b4_used)
            continue;

        // 添加新项
        npre_items[new_num] = npre_items[i];
        new_num++;
    }

    return new_num;  // 返回新预测项数量
}

// 获取词条字符串
uint16 DictList::get_lemma_str(LemmaIdType id_lemma, char16 *str_buf,
                               uint16 str_max) {
    // 参数检查
    if (!initialized_ || id_lemma >= start_id_[kMaxLemmaSize] || NULL == str_buf
        || str_max <= 1)
        return 0;

    // 查找词条所在长度范围
    for (uint16 i = 0; i < kMaxLemmaSize; i++) {
        if (i + 1 > str_max - 1)  // 缓冲区不足
            return 0;

        // 找到对应长度区间
        if (start_id_[i] <= id_lemma && start_id_[i + 1] > id_lemma) {
            size_t id_span = id_lemma - start_id_[i];  // 区间内偏移
            uint16 *buf = buf_ + start_pos_[i] + id_span * (i + 1);  // 词条位置

            // 复制词条内容
            for (uint16 len = 0; len <= i; len++) {
                str_buf[len] = buf[len];
            }
            str_buf[i+1] = (char16)'\0';  // 添加结束符
            return i + 1;  // 返回词条长度
        }
    }
    return 0;  // 未找到
}

// 获取汉字的拼音ID
uint16 DictList::get_splids_for_hanzi(char16 hanzi, uint16 half_splid,
                                      uint16 *splids, uint16 max_splids) {
    // 查找汉字位置
    char16 *hz_found = static_cast<char16*>
        (mybsearch(&hanzi, scis_hz_, scis_num_, sizeof(char16), cmp_hanzis_1));
    assert(NULL != hz_found && hanzi == *hz_found);

    // 回溯到第一个匹配项
    while (hz_found > scis_hz_ && hanzi == *(hz_found - 1))
        hz_found--;

    // 检查是否需要严格匹配
    char16 *hz_f = hz_found;
    bool strict = false;
    while (hz_f < scis_hz_ + scis_num_ && hanzi == *hz_f) {
        uint16 pos = hz_f - scis_hz_;
        if (0 == half_splid || scis_splid_[pos].half_splid == half_splid) {
            strict = true;  // 存在严格匹配项
        }
        hz_f++;
    }

    // 收集拼音ID
    uint16 found_num = 0;
    while (hz_found < scis_hz_ + scis_num_ && hanzi == *hz_found) {
        uint16 pos = hz_found - scis_hz_;
        // 根据匹配模式筛选
        if (0 == half_splid ||
            (strict && scis_splid_[pos].half_splid == half_splid) ||
            (!strict && spl_trie_->half_full_compatible(half_splid,
                                                        scis_splid_[pos].full_splid))) {
            // 检查缓冲区空间
            assert(found_num + 1 < max_splids);
            splids[found_num] = scis_splid_[pos].full_splid;  // 存储拼音ID
            found_num++;
        }
        hz_found++;
    }

    return found_num;  // 返回找到的数量
}

// 获取词条ID
LemmaIdType DictList::get_lemma_id(const char16 *str, uint16 str_len) {
    // 参数检查
    if (NULL == str || str_len > kMaxLemmaSize)
        return 0;

    // 查找词条位置
    char16 *found = find_pos_startedbyhzs(str, str_len, cmp_func_[str_len - 1]);
    if (NULL == found)
        return 0;

    // 计算词条ID
    assert(found > buf_);
    assert(static_cast<size_t>(found - buf_) >= start_pos_[str_len - 1]);
    return static_cast<LemmaIdType>
        (start_id_[str_len - 1] +
         (found - buf_ - start_pos_[str_len - 1]) / str_len);
}

// 将SCI ID转换为实际汉字
void DictList::convert_to_hanzis(char16 *str, uint16 str_len) {
    assert(NULL != str);

    // 逐个转换字符
    for (uint16 str_pos = 0; str_pos < str_len; str_pos++) {
        str[str_pos] = scis_hz_[str[str_pos]];  // 替换为实际汉字
    }
}

// 将汉字转换为SCI ID（未实现完整）
void DictList::convert_to_scis_ids(char16 *str, uint16 str_len) {
    assert(NULL != str);

    // 临时实现：设置为默认值
    for (uint16 str_pos = 0; str_pos < str_len; str_pos++) {
        str[str_pos] = 0x100;  // 默认SCI ID
    }
}

// 保存词典列表到文件
bool DictList::save_list(FILE *fp) {
    // 状态检查
    if (!initialized_ || NULL == fp)
        return false;

    // 数据完整性检查
    if (NULL == buf_ || 0 == start_pos_[kMaxLemmaSize] ||
        NULL == scis_hz_ || NULL == scis_splid_ || 0 == scis_num_)
        return false;

    // 依次写入各部分数据
    if (fwrite(&scis_num_, sizeof(uint32), 1, fp) != 1)
        return false;

    if (fwrite(start_pos_, sizeof(uint32), kMaxLemmaSize + 1, fp) !=
        kMaxLemmaSize + 1)
        return false;

    if (fwrite(start_id_, sizeof(uint32), kMaxLemmaSize + 1, fp) !=
        kMaxLemmaSize + 1)
        return false;

    if (fwrite(scis_hz_, sizeof(char16), scis_num_, fp) != scis_num_)
        return false;

    if (fwrite(scis_splid_, sizeof(SpellingId), scis_num_, fp) != scis_num_)
        return false;

    if (fwrite(buf_, sizeof(char16), start_pos_[kMaxLemmaSize], fp) !=
        start_pos_[kMaxLemmaSize])
        return false;

    return true;
}

// 从文件加载词典列表
bool DictList::load_list(QFile *fp) {
    if (NULL == fp)
        return false;

    initialized_ = false;  // 重置初始化状态

    // 读取单字项数量
    if (fp->read((char *)&scis_num_, sizeof(uint32)) != sizeof(uint32))
        return false;

    // 读取起始位置数组
    if (fp->read((char *)start_pos_, sizeof(uint32) * (kMaxLemmaSize + 1)) !=
        sizeof(uint32) * (kMaxLemmaSize + 1))
        return false;

    // 读取起始ID数组
    if (fp->read((char *)start_id_, sizeof(uint32) * (kMaxLemmaSize + 1)) !=
        sizeof(uint32) * (kMaxLemmaSize + 1))
        return false;

    // 释放旧资源
    free_resource();

    // 分配新资源
    if (!alloc_resource(start_pos_[kMaxLemmaSize], scis_num_))
        return false;

    // 读取单字项汉字
    if (fp->read((char *)scis_hz_, sizeof(char16) * scis_num_) != sizeof(char16) * scis_num_)
        return false;

    // 读取单字项拼音ID
    if (fp->read((char *)scis_splid_, sizeof(SpellingId) * scis_num_) != sizeof(SpellingId) * scis_num_)
        return false;

    // 读取词典内容
    if (fp->read((char *)buf_, sizeof(char16) * start_pos_[kMaxLemmaSize]) !=
        sizeof(char16) * start_pos_[kMaxLemmaSize])
        return false;

    initialized_ = true;  // 设置初始化完成
    return true;
}
}  // namespace ime_pinyin
