#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictbuilder.h"
#include "dicttrie.h"
#include "mystdlib.h"
#include "ngram.h"
#include "searchutility.h"
#include "spellingtable.h"
#include "spellingtrie.h"
#include "splparser.h"
#include "utf16reader.h"

namespace ime_pinyin {

#ifdef ___BUILD_MODEL___  // 仅在构建模型时编译以下代码

static const size_t kReadBufLen = 512;          // 读取缓冲区长度
static const size_t kSplTableHashLen = 2000;    // 拼音表哈希长度

// 比较两个单字项(SingleCharItem)：先按汉字，再按拼音ID，最后按频率
int cmp_scis_hz_splid_freq(const void* p1, const void* p2) {
    const SingleCharItem *s1, *s2;
    s1 = static_cast<const SingleCharItem*>(p1);
    s2 = static_cast<const SingleCharItem*>(p2);

    if (s1->hz < s2->hz)
        return -1;
    if (s1->hz > s2->hz)
        return 1;

    if (s1->splid.half_splid < s2->splid.half_splid)
        return -1;
    if (s1->splid.half_splid > s2->splid.half_splid)
        return 1;

    if (s1->splid.full_splid < s2->splid.full_splid)
        return -1;
    if (s1->splid.full_splid > s2->splid.full_splid)
        return 1;

    if (s1->freq > s2->freq)
        return -1;
    if (s1->freq < s2->freq)
        return 1;
    return 0;
}

// 比较两个单字项：仅按汉字和拼音ID（忽略频率）
int cmp_scis_hz_splid(const void* p1, const void* p2) {
    const SingleCharItem *s1, *s2;
    s1 = static_cast<const SingleCharItem*>(p1);
    s2 = static_cast<const SingleCharItem*>(p2);

    // 比较汉字
    if (s1->hz < s2->hz)
        return -1;
    if (s1->hz > s2->hz)
        return 1;

    // 比较半角拼音ID
    if (s1->splid.half_splid < s2->splid.half_splid)
        return -1;
    if (s1->splid.half_splid > s2->splid.half_splid)
        return 1;

    // 比较全角拼音ID
    if (s1->splid.full_splid < s2->splid.full_splid)
        return -1;
    if (s1->splid.full_splid > s2->splid.full_splid)
        return 1;

    return 0;
}

// 比较两个词条(LemmaEntry)：按汉字字符串长度和内容排序
int cmp_lemma_entry_hzs(const void* p1, const void* p2) {
    size_t size1 = utf16_strlen(((const LemmaEntry*)p1)->hanzi_str);
    size_t size2 = utf16_strlen(((const LemmaEntry*)p2)->hanzi_str);
    if (size1 < size2)
        return -1;
    else if (size1 > size2)
        return 1;

    // 长度相同则按内容比较
    return utf16_strcmp(((const LemmaEntry*)p1)->hanzi_str,
                        ((const LemmaEntry*)p2)->hanzi_str);
}

// 比较两个UTF-16字符
int compare_char16(const void* p1, const void* p2) {
    if (*((const char16*)p1) < *((const char16*)p2))
        return -1;
    if (*((const char16*)p1) > *((const char16*)p2))
        return 1;
    return 0;
}

// 比较两个词条：先按拼音序列，拼音相同则按频率降序
int compare_py(const void* p1, const void* p2) {
    int ret = utf16_strcmp(((const LemmaEntry*)p1)->spl_idx_arr,
                           ((const LemmaEntry*)p2)->spl_idx_arr);

    if (0 != ret)
        return ret;

    return static_cast<int>(((const LemmaEntry*)p2)->freq) -
           static_cast<int>(((const LemmaEntry*)p1)->freq);
}

// 比较两个词条：先按汉字字符串，再按拼音序列
int cmp_lemma_entry_hzspys(const void* p1, const void* p2) {
    size_t size1 = utf16_strlen(((const LemmaEntry*)p1)->hanzi_str);
    size_t size2 = utf16_strlen(((const LemmaEntry*)p2)->hanzi_str);
    if (size1 < size2)
        return -1;
    else if (size1 > size2)
        return 1;
    int ret = utf16_strcmp(((const LemmaEntry*)p1)->hanzi_str,
                           ((const LemmaEntry*)p2)->hanzi_str);

    if (0 != ret)
        return ret;

    ret = utf16_strcmp(((const LemmaEntry*)p1)->spl_idx_arr,
                       ((const LemmaEntry*)p2)->spl_idx_arr);
    return ret;
}

// 比较两个词条：仅按拼音序列
int compare_splid2(const void* p1, const void* p2) {
    int ret = utf16_strcmp(((const LemmaEntry*)p1)->spl_idx_arr,
                           ((const LemmaEntry*)p2)->spl_idx_arr);
    return ret;
}

// 构造函数：初始化所有指针为空
DictBuilder::DictBuilder() {
    lemma_arr_ = NULL;          // 词条数组
    lemma_num_ = 0;             // 词条数量

    scis_ = NULL;               // 单字项数组
    scis_num_ = 0;              // 单字项数量

    lma_nodes_le0_ = NULL;      // 层级0的词条节点
    lma_nodes_ge1_ = NULL;      // 层级1及以上的词条节点

    lma_nds_used_num_le0_ = 0;  // 已使用的层级0节点数
    lma_nds_used_num_ge1_ = 0;  // 已使用的层级1及以上节点数

    homo_idx_buf_ = NULL;       // 同音词索引缓冲区
    homo_idx_num_eq1_ = 0;      // 单音节同音词数量
    homo_idx_num_gt1_ = 0;      // 多音节同音词数量

    top_lmas_ = NULL;           // 高频词数组
    top_lmas_num_ = 0;          // 高频词数量

    spl_table_ = NULL;          // 拼音表
    spl_parser_ = NULL;         // 拼音解析器
}

// 析构函数：释放所有资源
DictBuilder::~DictBuilder() {
    free_resource();
}

// 分配资源：根据词条数量初始化各种缓冲区
bool DictBuilder::alloc_resource(size_t lma_num) {
    if (0 == lma_num)
        return false;

    free_resource();  // 先释放已有资源

    lemma_num_ = lma_num;
    lemma_arr_ = new LemmaEntry[lemma_num_];  // 分配词条数组

    top_lmas_num_ = 0;
    top_lmas_ = new LemmaEntry[kTopScoreLemmaNum];  // 分配高频词数组

    // 单字项缓冲区按最大可能大小分配（词条数 * 最大词长）
    scis_num_ = lemma_num_ * kMaxLemmaSize;
    scis_ = new SingleCharItem[scis_num_];

    // 根节点和第一层节点数不超过最大拼音数+1
    lma_nds_used_num_le0_ = 0;
    lma_nodes_le0_ = new LmaNodeLE0[kMaxSpellingNum + 1];

    // 其他层节点数不超过词条数
    lma_nds_used_num_ge1_ = 0;
    lma_nodes_ge1_ = new LmaNodeGE1[lemma_num_];

    homo_idx_buf_ = new LemmaIdType[lemma_num_];  // 同音词索引缓冲区
    spl_table_ = new SpellingTable();             // 创建拼音表
    spl_parser_ = new SpellingParser();           // 创建拼音解析器

    // 检查所有分配是否成功
    if (NULL == lemma_arr_ || NULL == top_lmas_ ||
        NULL == scis_ || NULL == spl_table_ ||
        NULL == spl_parser_ || NULL == lma_nodes_le0_ ||
        NULL == lma_nodes_ge1_ || NULL == homo_idx_buf_) {
        free_resource();
        return false;
    }

    // 初始化分配的内存
    memset(lemma_arr_, 0, sizeof(LemmaEntry) * lemma_num_);
    memset(scis_, 0, sizeof(SingleCharItem) * scis_num_);
    memset(lma_nodes_le0_, 0, sizeof(LmaNodeLE0) * (kMaxSpellingNum + 1));
    memset(lma_nodes_ge1_, 0, sizeof(LmaNodeGE1) * lemma_num_);
    memset(homo_idx_buf_, 0, sizeof(LemmaIdType) * lemma_num_);
    spl_table_->init_table(kMaxPinyinSize, kSplTableHashLen, true);  // 初始化拼音表

    return true;
}

// 从文件读取有效汉字列表
char16* DictBuilder::read_valid_hanzis(const char *fn_validhzs, size_t *num) {
    if (NULL == fn_validhzs || NULL == num)
        return NULL;

    *num = 0;
    FILE *fp = fopen(fn_validhzs, "rb");  // 以二进制模式打开文件
    if (NULL == fp)
        return NULL;

    // 检查UTF-16文件头 (BOM)
    char16 utf16header;
    if (fread(&utf16header, sizeof(char16), 1, fp) != 1 ||
        0xfeff != utf16header) {
        fclose(fp);
        return NULL;
    }

    // 获取文件大小并计算汉字数量
    fseek(fp, 0, SEEK_END);
    *num = ftell(fp) / sizeof(char16);
    assert(*num >= 1);
    *num -= 1;  // 减去BOM的大小

    // 分配存储空间
    char16 *hzs = new char16[*num];
    if (NULL == hzs) {
        fclose(fp);
        return NULL;
    }

    // 读取汉字数据（跳过BOM）
    fseek(fp, 2, SEEK_SET);
    if (fread(hzs, sizeof(char16), *num, fp) != *num) {
        fclose(fp);
        delete [] hzs;
        return NULL;
    }
    fclose(fp);

    // 对汉字进行排序
    myqsort(hzs, *num, sizeof(char16), compare_char16);
    return hzs;
}

// 检查单个汉字是否在有效汉字列表中
bool DictBuilder::hz_in_hanzis_list(const char16 *hzs, size_t hzs_len,
                                    char16 hz) {
    if (NULL == hzs)
        return false;

    // 使用二分查找
    char16 *found;
    found = static_cast<char16*>(
        mybsearch(&hz, hzs, hzs_len, sizeof(char16), compare_char16));
    if (NULL == found)
        return false;

    assert(*found == hz);
    return true;
}

// 检查整个字符串是否都在有效汉字列表中
bool DictBuilder::str_in_hanzis_list(const char16 *hzs, size_t hzs_len,
                                     const char16 *str, size_t str_len) {
    if (NULL == hzs || NULL == str)
        return false;

    // 逐个字符检查
    for (size_t pos = 0; pos < str_len; pos++) {
        if (!hz_in_hanzis_list(hzs, hzs_len, str[pos]))
            return false;
    }
    return true;
}

// 获取频率最高的词条（高频词）
void DictBuilder::get_top_lemmas() {
    top_lmas_num_ = 0;
    if (NULL == lemma_arr_)
        return;

    // 遍历所有词条，维护一个大小为kTopScoreLemmaNum的高频词列表
    for (size_t pos = 0; pos < lemma_num_; pos++) {
        if (0 == top_lmas_num_) {  // 列表为空时直接添加
            top_lmas_[0] = lemma_arr_[pos];
            top_lmas_num_ = 1;
            continue;
        }

        // 如果当前词条频率高于列表中的最小值
        if (lemma_arr_[pos].freq > top_lmas_[top_lmas_num_ - 1].freq) {
            // 如果列表未满则扩容
            if (kTopScoreLemmaNum > top_lmas_num_)
                top_lmas_num_ += 1;

            // 找到合适的位置插入
            size_t move_pos;
            for (move_pos = top_lmas_num_ - 1; move_pos > 0; move_pos--) {
                top_lmas_[move_pos] = top_lmas_[move_pos - 1];
                if (0 == move_pos - 1 ||
                    (move_pos - 1 > 0 &&
                     top_lmas_[move_pos - 2].freq > lemma_arr_[pos].freq)) {
                    break;
                }
            }
            assert(move_pos > 0);
            top_lmas_[move_pos - 1] = lemma_arr_[pos];  // 插入新词条
        } else if (kTopScoreLemmaNum > top_lmas_num_) {
            // 列表未满且频率不低于最小值，直接添加到末尾
            top_lmas_[top_lmas_num_] = lemma_arr_[pos];
            top_lmas_num_ += 1;
        }
    }

    // 调试输出
    if (kPrintDebug0) {
        printf("\n------Top Lemmas------------------\n");
        for (size_t pos = 0; pos < top_lmas_num_; pos++) {
            printf("--%d, idx:%06d, score:%.5f\n", pos, top_lmas_[pos].idx_by_hz,
                   top_lmas_[pos].freq);
        }
    }
}

// 释放所有分配的资源
void DictBuilder::free_resource() {
    if (NULL != lemma_arr_)
        delete [] lemma_arr_;

    if (NULL != scis_)
        delete [] scis_;

    if (NULL != lma_nodes_le0_)
        delete [] lma_nodes_le0_;

    if (NULL != lma_nodes_ge1_)
        delete [] lma_nodes_ge1_;

    if (NULL != homo_idx_buf_)
        delete [] homo_idx_buf_;

    if (NULL != spl_table_)
        delete spl_table_;

    if (NULL != spl_parser_)
        delete spl_parser_;

    // 重置指针和计数器
    lemma_arr_ = NULL;
    scis_ = NULL;
    lma_nodes_le0_ = NULL;
    lma_nodes_ge1_ = NULL;
    homo_idx_buf_ = NULL;
    spl_table_ = NULL;
    spl_parser_ = NULL;

    lemma_num_ = 0;
    lma_nds_used_num_le0_ = 0;
    lma_nds_used_num_ge1_ = 0;
    homo_idx_num_eq1_ = 0;
    homo_idx_num_gt1_ = 0;
}

// 从原始词典文件读取数据
size_t DictBuilder::read_raw_dict(const char* fn_raw,
                                  const char *fn_validhzs,
                                  size_t max_item) {
    if (NULL == fn_raw) return 0;

    // 使用UTF-16读取器打开文件
    Utf16Reader utf16_reader;
    if (!utf16_reader.open(fn_raw, kReadBufLen * 10))
        return false;

    char16 read_buf[kReadBufLen];  // 读取缓冲区

    // 初始化词条数量（假设为240000）
    size_t lemma_num = 240000;

    // 分配资源（内存缓冲区等）
    if (!alloc_resource(lemma_num)) {
        utf16_reader.close();
    }

    // 读取有效汉字列表
    char16 *valid_hzs = NULL;
    size_t valid_hzs_num = 0;
    valid_hzs = read_valid_hanzis(fn_validhzs, &valid_hzs_num);

    // 逐行读取词典条目
    for (size_t i = 0; i < max_item; i++) {
        // 读取一行
        if (!utf16_reader.readline(read_buf, kReadBufLen)) {
            lemma_num = i;  // 实际读取的词条数
            break;
        }

        size_t token_size;
        char16 *token;
        char16 *to_tokenize = read_buf;

        // 1. 获取汉字字符串（第一个token）
        token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);
        if (NULL == token) {
            free_resource();
            utf16_reader.close();
            return false;
        }

        size_t lemma_size = utf16_strlen(token);

        // 跳过超长词条
        if (lemma_size > kMaxLemmaSize) {
            i--;
            continue;
        }

        // 调试限制：只处理4字以内的词
        if (lemma_size > 4) {
            i--;
            continue;
        }

        // 复制到词条数组
        utf16_strcpy(lemma_arr_[i].hanzi_str, token);
        lemma_arr_[i].hz_str_len = token_size;

        // 2. 获取频率（第二个token）
        token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);
        if (NULL == token) {
            free_resource();
            utf16_reader.close();
            return false;
        }
        lemma_arr_[i].freq = utf16_atof(token);  // 转换为浮点数

        // 过滤低频多字词
        if (lemma_size > 1 && lemma_arr_[i].freq < 60) {
            i--;
            continue;
        }

        // 3. 获取GBK标记（第三个token）
        token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);
        assert(NULL != token);
        int gbk_flag = utf16_atoi(token);  // 转换为整数

        // 汉字有效性检查
        if (NULL == valid_hzs || 0 == valid_hzs_num) {
            // 无有效汉字列表时：跳过GBK扩展字符
            if (0 != gbk_flag) {
                i--;
                continue;
            }
        } else {
            // 有有效汉字列表时：跳过不在列表中的词
            if (!str_in_hanzis_list(valid_hzs, valid_hzs_num,
                                    lemma_arr_[i].hanzi_str, lemma_arr_[i].hz_str_len)) {
                i--;
                continue;
            }
        }

        // 4. 处理拼音字符串（每个汉字对应一个拼音）
        bool spelling_not_support = false;
        for (size_t hz_pos = 0; hz_pos < (size_t)lemma_arr_[i].hz_str_len;
             hz_pos++) {
            // 获取一个拼音
            token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);
            if (NULL == token) {
                free_resource();
                utf16_reader.close();
                return false;
            }

            assert(utf16_strlen(token) <= kMaxPinyinSize);

            // 复制并格式化拼音字符串
            utf16_strcpy_tochar(lemma_arr_[i].pinyin_str[hz_pos], token);
            format_spelling_str(lemma_arr_[i].pinyin_str[hz_pos]);

            // 将拼音加入拼音表
            if (!spl_table_->put_spelling(lemma_arr_[i].pinyin_str[hz_pos],
                                          lemma_arr_[i].freq)) {
                spelling_not_support = true;  // 不支持的拼音
                break;
            }
        }

        // 5. 检查是否完全解析（无多余token）
        token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);
        if (spelling_not_support || NULL != token) {
            i--;  // 解析失败则跳过该词条
            continue;
        }
    }

    // 清理资源
    delete [] valid_hzs;
    utf16_reader.close();

    printf("read succesfully, lemma num: %d\n", lemma_num);
    return lemma_num;
}

// 构建词典的主函数
bool DictBuilder::build_dict(const char *fn_raw,
                             const char *fn_validhzs,
                             DictTrie *dict_trie) {
    if (NULL == fn_raw || NULL == dict_trie)
        return false;

    // 1. 读取原始词典
    lemma_num_ = read_raw_dict(fn_raw, fn_validhzs, 240000);
    if (0 == lemma_num_)
        return false;

    // 2. 排列拼音表并构建拼音树
    size_t spl_item_size;  // 每个拼音项的大小
    size_t spl_num;        // 拼音数量
    const char* spl_buf = spl_table_->arrange(&spl_item_size, &spl_num);
    if (NULL == spl_buf) {
        free_resource();
        return false;
    }

    // 初始化拼音树单例
    SpellingTrie &spl_trie = SpellingTrie::get_instance();
    if (!spl_trie.construct(spl_buf, spl_item_size, spl_num,
                            spl_table_->get_score_amplifier(),
                            spl_table_->get_average_score())) {
        free_resource();
        return false;
    }
    printf("spelling tree construct successfully.\n");

    // 3. 将拼音字符串转换为ID
    for (size_t i = 0; i < lemma_num_; i++) {
        for (size_t hz_pos = 0; hz_pos < (size_t)lemma_arr_[i].hz_str_len;
             hz_pos++) {
            uint16 spl_idxs[2];      // 拼音ID缓冲区
            uint16 spl_start_pos[3]; // 拼音起始位置（未使用）
            bool is_pre = true;       // 标记为前缀匹配
            // 将拼音字符串转换为ID
            int spl_idx_num =
                spl_parser_->splstr_to_idxs(lemma_arr_[i].pinyin_str[hz_pos],
                                            strlen(lemma_arr_[i].pinyin_str[hz_pos]),
                                            spl_idxs, spl_start_pos, 2, is_pre);
            assert(1 == spl_idx_num);  // 应成功转换一个ID

            // 如果是半角ID，转换为全角ID
            if (spl_trie.is_half_id(spl_idxs[0])) {
                uint16 num = spl_trie.half_to_full(spl_idxs[0], spl_idxs);
                assert(0 != num);
            }
            lemma_arr_[i].spl_idx_arr[hz_pos] = spl_idxs[0];  // 存储全角拼音ID
        }
    }

    // 4. 按汉字排序词条并分配唯一ID
    sort_lemmas_by_hz();

    // 5. 构建单字项列表
    scis_num_ = build_scis();

    // 6. 初始化词典列表
    dict_trie->dict_list_ = new DictList();
    bool dl_success = dict_trie->dict_list_->init_list(scis_, scis_num_,
                                                       lemma_arr_, lemma_num_);
    assert(dl_success);

    // 7. 构建N-Gram模型（一元语法）
    NGram& ngram = NGram::get_instance();
    ngram.build_unigram(lemma_arr_, lemma_num_,
                        lemma_arr_[lemma_num_ - 1].idx_by_hz + 1);

    // 8. 按拼音序列排序词条
    myqsort(lemma_arr_, lemma_num_, sizeof(LemmaEntry), compare_py);

    // 9. 获取高频词
    get_top_lemmas();

// 调试统计信息
#ifdef ___DO_STATISTICS___
    stat_init();
#endif

    // 10. 构建词典树
    lma_nds_used_num_le0_ = 1;  // 根节点
    bool dt_success = construct_subset(static_cast<void*>(lma_nodes_le0_),
                                       lemma_arr_, 0, lemma_num_, 0);
    if (!dt_success) {
        free_resource();
        return false;
    }

// 打印统计信息
#ifdef ___DO_STATISTICS___
    stat_print();
#endif

    // 11. 将构建好的数据转移到DictTrie中
    dict_trie->root_ = new LmaNodeLE0[lma_nds_used_num_le0_];
    dict_trie->nodes_ge1_ = new LmaNodeGE1[lma_nds_used_num_ge1_];
    size_t lma_idx_num = homo_idx_num_eq1_ + homo_idx_num_gt1_ + top_lmas_num_;
    dict_trie->lma_idx_buf_ = new unsigned char[lma_idx_num * kLemmaIdSize];
    assert(NULL != dict_trie->root_);
    assert(NULL != dict_trie->lma_idx_buf_);

    // 设置DictTrie的成员变量
    dict_trie->lma_node_num_le0_ = lma_nds_used_num_le0_;
    dict_trie->lma_node_num_ge1_ = lma_nds_used_num_ge1_;
    dict_trie->lma_idx_buf_len_ = lma_idx_num * kLemmaIdSize;
    dict_trie->top_lmas_num_ = top_lmas_num_;

    // 复制节点数据
    memcpy(dict_trie->root_, lma_nodes_le0_,
           sizeof(LmaNodeLE0) * lma_nds_used_num_le0_);
    memcpy(dict_trie->nodes_ge1_, lma_nodes_ge1_,
           sizeof(LmaNodeGE1) * lma_nds_used_num_ge1_);

    // 复制同音词索引
    for (size_t pos = 0; pos < homo_idx_num_eq1_ + homo_idx_num_gt1_; pos++) {
        id_to_charbuf(dict_trie->lma_idx_buf_ + pos * kLemmaIdSize,
                      homo_idx_buf_[pos]);
    }

    // 复制高频词索引
    for (size_t pos = homo_idx_num_eq1_ + homo_idx_num_gt1_;
         pos < lma_idx_num; pos++) {
        LemmaIdType idx =
            top_lmas_[pos - homo_idx_num_eq1_ - homo_idx_num_gt1_].idx_by_hz;
        id_to_charbuf(dict_trie->lma_idx_buf_ + pos * kLemmaIdSize, idx);
    }

    // 调试输出
    if (kPrintDebug0) {
        printf("homo_idx_num_eq1_: %d\n", homo_idx_num_eq1_);
        printf("homo_idx_num_gt1_: %d\n", homo_idx_num_gt1_);
        printf("top_lmas_num_: %d\n", top_lmas_num_);
    }

    free_resource();  // 释放构建用资源

    if (kPrintDebug0) {
        printf("Building dict succeds\n");
    }
    return dt_success;
}

// 将词条ID转换为字节缓冲区（小端存储）
void DictBuilder::id_to_charbuf(unsigned char *buf, LemmaIdType id) {
    if (NULL == buf) return;
    for (size_t pos = 0; pos < kLemmaIdSize; pos++) {
        (buf)[pos] = (unsigned char)(id >> (pos * 8));  // 按字节存储
    }
}

// 设置GE1节点的子节点偏移量（处理32位偏移）
void DictBuilder::set_son_offset(LmaNodeGE1 *node, size_t offset) {
    node->son_1st_off_l = static_cast<uint16>(offset);      // 低16位
    node->son_1st_off_h = static_cast<unsigned char>(offset >> 16);  // 高8位
}

// 设置GE1节点的同音词缓冲区偏移量
void DictBuilder:: set_homo_id_buf_offset(LmaNodeGE1 *node, size_t offset) {
    node->homo_idx_buf_off_l = static_cast<uint16>(offset);      // 低16位
    node->homo_idx_buf_off_h = static_cast<unsigned char>(offset >> 16);  // 高8位
}

// 格式化拼音字符串：转换为大写，"ZH"/"CH"/"SH"转为"Zh"/"Ch"/"Sh"
void DictBuilder::format_spelling_str(char *spl_str) {
    if (NULL == spl_str)
        return;

    uint16 pos = 0;
    while ('\0' != spl_str[pos]) {
        // 转换为大写
        if (spl_str[pos] >= 'a' && spl_str[pos] <= 'z')
            spl_str[pos] = spl_str[pos] - 'a' + 'A';

        // 特殊处理声母"ZH/CH/SH"
        if (1 == pos && 'H' == spl_str[pos]) {
            if ('C' == spl_str[0] || 'S' == spl_str[0] || 'Z' == spl_str[0]) {
                spl_str[pos] = 'h';  // 第二个字母转为小写
            }
        }
        pos++;
    }
}

// 按汉字排序词条并分配唯一ID
LemmaIdType DictBuilder::sort_lemmas_by_hz() {
    if (NULL == lemma_arr_ || 0 == lemma_num_)
        return 0;

    // 按汉字字符串排序
    myqsort(lemma_arr_, lemma_num_, sizeof(LemmaEntry), cmp_lemma_entry_hzs);

    // 分配唯一ID（从1开始）
    lemma_arr_[0].idx_by_hz = 1;
    LemmaIdType idx_max = 1;
    for (size_t i = 1; i < lemma_num_; i++) {
        if (utf16_strcmp(lemma_arr_[i].hanzi_str, lemma_arr_[i-1].hanzi_str)) {
            // 新词条
            idx_max++;
            lemma_arr_[i].idx_by_hz = idx_max;
        } else {
            // 同形词（不同读音）分配新ID
            idx_max++;
            lemma_arr_[i].idx_by_hz = idx_max;
        }
    }
    return idx_max + 1;  // 返回最大ID+1（即词条总数+1）
}

// 构建单字项列表
size_t DictBuilder::build_scis() {
    if (NULL == scis_ || lemma_num_ * kMaxLemmaSize > scis_num_)
        return 0;

    SpellingTrie &spl_trie = SpellingTrie::get_instance();

    // 第0项为空（ID 0无效）
    scis_[0].freq = 0;
    scis_[0].hz = 0;
    scis_[0].splid.full_splid = 0;
    scis_[0].splid.half_splid = 0;
    scis_num_ = 1;  // 从索引1开始使用

    // 遍历所有词条的所有汉字
    for (size_t pos = 0; pos < lemma_num_; pos++) {
        size_t hz_num = lemma_arr_[pos].hz_str_len;
        for (size_t hzpos = 0; hzpos < hz_num; hzpos++) {
            scis_[scis_num_].hz = lemma_arr_[pos].hanzi_str[hzpos];  // 汉字
            scis_[scis_num_].splid.full_splid = lemma_arr_[pos].spl_idx_arr[hzpos];  // 全拼ID
            scis_[scis_num_].splid.half_splid =  // 计算半拼ID
                spl_trie.full_to_half(scis_[scis_num_].splid.full_splid);
            // 单字词使用原频率，多字词中的字频率设为极小值
            if (1 == hz_num)
                scis_[scis_num_].freq = lemma_arr_[pos].freq;
            else
                scis_[scis_num_].freq = 0.000001;
            scis_num_++;
        }
    }

    // 按汉字、拼音ID、频率排序
    myqsort(scis_, scis_num_, sizeof(SingleCharItem), cmp_scis_hz_splid_freq);

    // 去重（相同汉字+相同拼音ID视为重复）
    size_t unique_scis_num = 1;
    for (size_t pos = 1; pos < scis_num_; pos++) {
        if (scis_[pos].hz == scis_[pos - 1].hz &&
            scis_[pos].splid.full_splid == scis_[pos - 1].splid.full_splid)
            continue;  // 跳过重复项
        scis_[unique_scis_num] = scis_[pos];
        // 更新半拼ID（可能因拼音树变化而不同）
        scis_[unique_scis_num].splid.half_splid =
            spl_trie.full_to_half(scis_[pos].splid.full_splid);
        unique_scis_num++;
    }

    scis_num_ = unique_scis_num;  // 更新有效数量

    // 更新词条中的单字项ID
    for (size_t pos = 0; pos < lemma_num_; pos++) {
        size_t hz_num = lemma_arr_[pos].hz_str_len;
        for (size_t hzpos = 0; hzpos < hz_num; hzpos++) {
            SingleCharItem key;
            key.hz = lemma_arr_[pos].hanzi_str[hzpos];
            key.splid.full_splid = lemma_arr_[pos].spl_idx_arr[hzpos];
            key.splid.half_splid = spl_trie.full_to_half(key.splid.full_splid);

            // 在单字项列表中二分查找
            SingleCharItem *found;
            found = static_cast<SingleCharItem*>(mybsearch(&key, scis_,
                                                            unique_scis_num,
                                                            sizeof(SingleCharItem),
                                                            cmp_scis_hz_splid));
            assert(found);  // 必须找到

            // 存储单字项索引和更新后的拼音ID
            lemma_arr_[pos].hanzi_scis_ids[hzpos] =
                static_cast<uint16>(found - scis_);
            lemma_arr_[pos].spl_idx_arr[hzpos] = found->splid.full_splid;
        }
    }

    return scis_num_;  // 返回唯一单字项数量
}

// 递归构建词典树子集
bool DictBuilder::construct_subset(void* parent, LemmaEntry* lemma_arr,
                                   size_t item_start, size_t item_end,
                                   size_t level) {
    if (level >= kMaxLemmaSize || item_end <= item_start)
        return false;

    // 1. 扫描计算子节点数量
    size_t parent_son_num = 0;
    LemmaEntry *lma_last_start = lemma_arr_ + item_start;
    uint16 spl_idx_node = lma_last_start->spl_idx_arr[level];  // 当前层拼音ID

    // 遍历范围内的词条，统计不同拼音ID的数量
    for (size_t i = item_start + 1; i< item_end; i++) {
        LemmaEntry *lma_current = lemma_arr + i;
        uint16 spl_idx_current = lma_current->spl_idx_arr[level];
        if (spl_idx_current != spl_idx_node) {
            parent_son_num++;  // 发现新拼音ID
            spl_idx_node = spl_idx_current;
        }
    }
    parent_son_num++;  // 加上最后一个分组

// 调试统计
#ifdef ___DO_STATISTICS___
    bool allson_noson = true;  // 标记是否所有子节点都没有后代

    assert(level < kMaxLemmaSize);
    if (parent_son_num > max_sonbuf_len_[level])
        max_sonbuf_len_[level] = parent_son_num;

    total_son_num_[level] += parent_son_num;
    total_sonbuf_num_[level] += 1;

    if (parent_son_num == 1)
        sonbufs_num1_++;
    else
        sonbufs_numgt1_++;
    total_lma_node_num_ += parent_son_num;
#endif

    // 2. 更新父节点信息（设置子节点列表偏移）
    LmaNodeLE0 *son_1st_le0 = NULL;  // 用于层级0的子节点
    LmaNodeGE1 *son_1st_ge1 = NULL;  // 用于层级1及以上的子节点
    if (0 == level) {                 // 根节点
        (static_cast<LmaNodeLE0*>(parent))->son_1st_off =
            lma_nds_used_num_le0_;
        son_1st_le0 = lma_nodes_le0_ + lma_nds_used_num_le0_;
        lma_nds_used_num_le0_ += parent_son_num;  // 分配空间

        assert(parent_son_num <= 65535);
        (static_cast<LmaNodeLE0*>(parent))->num_of_son =
            static_cast<uint16>(parent_son_num);
    } else if (1 == level) {          // 根的直接子节点
        (static_cast<LmaNodeLE0*>(parent))->son_1st_off =
            lma_nds_used_num_ge1_;
        son_1st_ge1 = lma_nodes_ge1_ + lma_nds_used_num_ge1_;
        lma_nds_used_num_ge1_ += parent_son_num;

        assert(parent_son_num <= 65535);
        (static_cast<LmaNodeLE0*>(parent))->num_of_son =
            static_cast<uint16>(parent_son_num);
    } else {                          // 其他层级的节点
        set_son_offset((static_cast<LmaNodeGE1*>(parent)),
                       lma_nds_used_num_ge1_);
        son_1st_ge1 = lma_nodes_ge1_ + lma_nds_used_num_ge1_;
        lma_nds_used_num_ge1_ += parent_son_num;

        assert(parent_son_num <= 255);
        (static_cast<LmaNodeGE1*>(parent))->num_of_son =
            (unsigned char)parent_son_num;
    }

    // 3. 逐个构建子节点
    size_t son_pos = 0;
    lma_last_start = lemma_arr_ + item_start;
    spl_idx_node = lma_last_start->spl_idx_arr[level];

    size_t homo_num = 0;  // 当前拼音ID的同音词数量
    // 如果下一个拼音ID为0（结束标记），则当前词条在该节点结束
    if (lma_last_start->spl_idx_arr[level + 1] == 0)
        homo_num = 1;

    size_t item_start_next = item_start;  // 当前分组起始位置

    for (size_t i = item_start + 1; i < item_end; i++) {
        LemmaEntry* lma_current = lemma_arr_ + i;
        uint16 spl_idx_current = lma_current->spl_idx_arr[level];

        if (spl_idx_current == spl_idx_node) {  // 同一分组
            // 检查是否当前层结束
            if (lma_current->spl_idx_arr[level + 1] == 0)
                homo_num++;
        } else {  // 新分组开始
            // 创建节点
            LmaNodeLE0 *node_cur_le0 = NULL;
            LmaNodeGE1 *node_cur_ge1 = NULL;
            if (0 == level) {
                node_cur_le0 = son_1st_le0 + son_pos;
                node_cur_le0->spl_idx = spl_idx_node;  // 设置拼音ID
                node_cur_le0->homo_idx_buf_off = homo_idx_num_eq1_ + homo_idx_num_gt1_;  // 同音词偏移
                node_cur_le0->son_1st_off = 0;  // 子节点偏移（暂设0）
                homo_idx_num_eq1_ += homo_num;   // 更新单音节同音词计数
            } else {
                node_cur_ge1 = son_1st_ge1 + son_pos;
                node_cur_ge1->spl_idx = spl_idx_node;
                set_homo_id_buf_offset(node_cur_ge1,
                                       (homo_idx_num_eq1_ + homo_idx_num_gt1_));
                set_son_offset(node_cur_ge1, 0);
                homo_idx_num_gt1_ += homo_num;  // 更新多音节同音词计数
            }

            // 如果有结束于该节点的词条
            if (homo_num > 0) {
                LemmaIdType* idx_buf = homo_idx_buf_ + homo_idx_num_eq1_ +
                                       homo_idx_num_gt1_ - homo_num;  // 同音词索引位置
                // 设置同音词数量
                if (0 == level) {
                    assert(homo_num <= 65535);
                    node_cur_le0->num_of_homo = static_cast<uint16>(homo_num);
                } else {
                    assert(homo_num <= 255);
                    node_cur_ge1->num_of_homo = (unsigned char)homo_num;
                }

                // 存储同音词ID
                for (size_t homo_pos = 0; homo_pos < homo_num; homo_pos++) {
                    idx_buf[homo_pos] = lemma_arr_[item_start_next + homo_pos].idx_by_hz;
                }

#ifdef ___DO_STATISTICS___
                if (homo_num > max_homobuf_len_[level])
                    max_homobuf_len_[level] = homo_num;

                total_homo_num_[level] += homo_num;
#endif
            }

            // 如果当前分组还有未处理的词条（即有子节点）
            if (i - item_start_next > homo_num) {
                void *next_parent = (0 == level) ?
                                        static_cast<void*>(node_cur_le0) :
                                        static_cast<void*>(node_cur_ge1);
                // 递归构建子树
                construct_subset(next_parent, lemma_arr,
                                 item_start_next + homo_num, i, level + 1);
#ifdef ___DO_STATISTICS___
                total_node_hasson_[level] += 1;
                allson_noson = false;  // 标记存在有子节点的节点
#endif
            }

            // 准备处理下一分组
            lma_last_start = lma_current;
            spl_idx_node = spl_idx_current;
            item_start_next = i;
            homo_num = 0;
            if (lma_current->spl_idx_arr[level + 1] == 0)
                homo_num = 1;

            son_pos++;
        }
    }

    // 4. 处理最后一个分组
    LmaNodeLE0 *node_cur_le0 = NULL;
    LmaNodeGE1 *node_cur_ge1 = NULL;
    if (0 == level) {
        node_cur_le0 = son_1st_le0 + son_pos;
        node_cur_le0->spl_idx = spl_idx_node;
        node_cur_le0->homo_idx_buf_off = homo_idx_num_eq1_ + homo_idx_num_gt1_;
        node_cur_le0->son_1st_off = 0;
        homo_idx_num_eq1_ += homo_num;
    } else {
        node_cur_ge1 = son_1st_ge1 + son_pos;
        node_cur_ge1->spl_idx = spl_idx_node;
        set_homo_id_buf_offset(node_cur_ge1,
                               (homo_idx_num_eq1_ + homo_idx_num_gt1_));
        set_son_offset(node_cur_ge1, 0);
        homo_idx_num_gt1_ += homo_num;
    }

    // 处理最后一个分组的同音词
    if (homo_num > 0) {
        LemmaIdType* idx_buf = homo_idx_buf_ + homo_idx_num_eq1_ +
                               homo_idx_num_gt1_ - homo_num;
        if (0 == level) {
            assert(homo_num <= 65535);
            node_cur_le0->num_of_homo = static_cast<uint16>(homo_num);
        } else {
            assert(homo_num <= 255);
            node_cur_ge1->num_of_homo = (unsigned char)homo_num;
        }

        for (size_t homo_pos = 0; homo_pos < homo_num; homo_pos++) {
            idx_buf[homo_pos] = lemma_arr[item_start_next + homo_pos].idx_by_hz;
        }

#ifdef ___DO_STATISTICS___
        if (homo_num > max_homobuf_len_[level])
            max_homobuf_len_[level] = homo_num;

        total_homo_num_[level] += homo_num;
#endif
    }

    // 处理最后一个分组的子节点
    if (item_end - item_start_next > homo_num) {
        void *next_parent = (0 == level) ?
                                static_cast<void*>(node_cur_le0) :
                                static_cast<void*>(node_cur_ge1);
        construct_subset(next_parent, lemma_arr,
                         item_start_next + homo_num, item_end, level + 1);
#ifdef ___DO_STATISTICS___
        total_node_hasson_[level] += 1;
        allson_noson = false;
#endif
    }

    // 验证子节点数量
    assert(son_pos + 1 == parent_son_num);

#ifdef ___DO_STATISTICS___
    // 统计所有子节点都没有后代的分组
    if (allson_noson) {
        total_sonbuf_allnoson_[level] += 1;
        total_node_in_sonbuf_allnoson_[level] += parent_son_num;
    }
#endif

    return true;
}

// 以下为调试统计代码
#ifdef ___DO_STATISTICS___
void DictBuilder::stat_init() {
    // 初始化所有统计计数器
    memset(max_sonbuf_len_, 0, sizeof(size_t) * kMaxLemmaSize);
    memset(max_homobuf_len_, 0, sizeof(size_t) * kMaxLemmaSize);
    memset(total_son_num_, 0, sizeof(size_t) * kMaxLemmaSize);
    memset(total_node_hasson_, 0, sizeof(size_t) * kMaxLemmaSize);
    memset(total_sonbuf_num_, 0, sizeof(size_t) * kMaxLemmaSize);
    memset(total_sonbuf_allnoson_, 0, sizeof(size_t) * kMaxLemmaSize);
    memset(total_node_in_sonbuf_allnoson_, 0, sizeof(size_t) * kMaxLemmaSize);
    memset(total_homo_num_, 0, sizeof(size_t) * kMaxLemmaSize);

    sonbufs_num1_ = 0;
    sonbufs_numgt1_ = 0;
    total_lma_node_num_ = 0;
}

void DictBuilder::stat_print() {
    // 打印统计信息
    printf("\n------------STAT INFO-------------\n");
    printf("[root is layer -1]\n");
    // 各层子节点缓冲区最大长度
    printf(".. max_sonbuf_len per layer(from layer 0):\n   ");
    for (size_t i = 0; i < kMaxLemmaSize; i++)
        printf("%d, ", max_sonbuf_len_[i]);
    printf("-, \n");

    // 各层同音词缓冲区最大长度
    printf(".. max_homobuf_len per layer:\n   -, ");
    for (size_t i = 0; i < kMaxLemmaSize; i++)
        printf("%d, ", max_homobuf_len_[i]);
    printf("\n");

    // 各层子节点总数
    printf(".. total_son_num per layer:\n   ");
    for (size_t i = 0; i < kMaxLemmaSize; i++)
        printf("%d, ", total_son_num_[i]);
    printf("-, \n");

    // 各层有子节点的节点数
    printf(".. total_node_hasson per layer:\n   1, ");
    for (size_t i = 0; i < kMaxLemmaSize; i++)
        printf("%d, ", total_node_hasson_[i]);
    printf("\n");

    // 各层子节点缓冲区数量
    printf(".. total_sonbuf_num per layer:\n   ");
    for (size_t i = 0; i < kMaxLemmaSize; i++)
        printf("%d, ", total_sonbuf_num_[i]);
    printf("-, \n");

    // 各层无子节点的缓冲区数量
    printf(".. total_sonbuf_allnoson per layer:\n   ");
    for (size_t i = 0; i < kMaxLemmaSize; i++)
        printf("%d, ", total_sonbuf_allnoson_[i]);
    printf("-, \n");

    // 各层无子节点的缓冲区中的节点总数
    printf(".. total_node_in_sonbuf_allnoson per layer:\n   ");
    for (size_t i = 0; i < kMaxLemmaSize; i++)
        printf("%d, ", total_node_in_sonbuf_allnoson_[i]);
    printf("-, \n");

    // 各层同音词总数
    printf(".. total_homo_num per layer:\n   0, ");
    for (size_t i = 0; i < kMaxLemmaSize; i++)
        printf("%d, ", total_homo_num_[i]);
    printf("\n");

    // 特殊统计
    printf(".. son buf allocation number with only 1 son: %d\n", sonbufs_num1_);
    printf(".. son buf allocation number with more than 1 son: %d\n",
           sonbufs_numgt1_);
    printf(".. total lemma node number: %d\n", total_lma_node_num_ + 1);
}
#endif  // ___DO_STATISTICS___

#endif  // ___BUILD_MODEL___
}  // namespace ime_pinyin
