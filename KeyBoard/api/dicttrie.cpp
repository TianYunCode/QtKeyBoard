#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "dicttrie.h"
#include "dictbuilder.h"
#include "lpicache.h"
#include "mystdlib.h"
#include "ngram.h"

namespace ime_pinyin {

// 构造函数：初始化字典树
DictTrie::DictTrie() {
    // 获取拼音Trie树的单例实例
    spl_trie_ = SpellingTrie::get_cpinstance();

    // 初始化指针和变量
    root_ = NULL;               // 根节点（LE0类型）
    splid_le0_index_ = NULL;    // LE0节点拼音索引
    lma_node_num_le0_ = 0;      // LE0节点数量
    nodes_ge1_ = NULL;          // GE1类型节点数组
    lma_node_num_ge1_ = 0;      // GE1节点数量
    lma_idx_buf_ = NULL;        // 词条索引缓冲区
    lma_idx_buf_len_ = 0;       // 索引缓冲区长度
    total_lma_num_ = 0;         // 总词条数量
    top_lmas_num_ = 0;          // 高频词条数量
    dict_list_ = NULL;          // 词典列表

    parsing_marks_ = NULL;      // 解析标记数组
    mile_stones_ = NULL;        // 解析里程碑数组
    // 重置解析状态
    reset_milestones(0, kFirstValidMileStoneHandle);
}

// 析构函数：释放资源
DictTrie::~DictTrie() {
    free_resource(true);
}

// 释放资源（可选择是否释放词典列表）
void DictTrie::free_resource(bool free_dict_list) {
    // 释放LE0节点
    if (NULL != root_)
        free(root_);
    root_ = NULL;

    // 释放拼音索引
    if (NULL != splid_le0_index_)
        free(splid_le0_index_);
    splid_le0_index_ = NULL;

    // 释放GE1节点
    if (NULL != nodes_ge1_)
        free(nodes_ge1_);
    nodes_ge1_ = NULL;

    // 释放词条索引缓冲区
    if (NULL != lma_idx_buf_)
        free(lma_idx_buf_);
    lma_idx_buf_ = NULL;

    // 可选释放词典列表
    if (free_dict_list) {
        if (NULL != dict_list_) {
            delete dict_list_;
        }
        dict_list_ = NULL;
    }

    // 释放解析相关资源
    if (parsing_marks_)
        delete [] parsing_marks_;
    parsing_marks_ = NULL;

    if (mile_stones_)
        delete [] mile_stones_;
    mile_stones_ = NULL;

    // 重置里程碑状态
    reset_milestones(0, kFirstValidMileStoneHandle);
}

// 获取GE1节点的子节点偏移量
inline size_t DictTrie::get_son_offset(const LmaNodeGE1 *node) {
    return ((size_t)node->son_1st_off_l + ((size_t)node->son_1st_off_h << 16));
}

// 获取GE1节点的同音词索引偏移量
inline size_t DictTrie::get_homo_idx_buf_offset(const LmaNodeGE1 *node) {
    return ((size_t)node->homo_idx_buf_off_l +
            ((size_t)node->homo_idx_buf_off_h << 16));
}

// 从索引缓冲区获取词条ID
inline LemmaIdType DictTrie::get_lemma_id(size_t id_offset) {
    LemmaIdType id = 0;
    // 从字节缓冲区重构词条ID（小端存储）
    for (uint16 pos = kLemmaIdSize - 1; pos > 0; pos--)
        id = (id << 8) + lma_idx_buf_[id_offset * kLemmaIdSize + pos];
    id = (id << 8) + lma_idx_buf_[id_offset * kLemmaIdSize];
    return id;
}

#ifdef ___BUILD_MODEL___  // 构建模型专用代码
// 构建字典（从原始文件）
bool DictTrie::build_dict(const char* fn_raw, const char* fn_validhzs) {
    DictBuilder* dict_builder = new DictBuilder();

    // 释放现有资源并构建新字典
    free_resource(true);
    return dict_builder->build_dict(fn_raw, fn_validhzs, this);
}

// 保存字典到文件流
bool DictTrie::save_dict(FILE *fp) {
    if (NULL == fp)
        return false;

    // 依次写入字典数据
    if (fwrite(&lma_node_num_le0_, sizeof(uint32), 1, fp) != 1)
        return false;

    if (fwrite(&lma_node_num_ge1_, sizeof(uint32), 1, fp) != 1)
        return false;

    if (fwrite(&lma_idx_buf_len_, sizeof(uint32), 1, fp) != 1)
        return false;

    if (fwrite(&top_lmas_num_, sizeof(uint32), 1, fp) != 1)
        return false;

    // 写入节点数据
    if (fwrite(root_, sizeof(LmaNodeLE0), lma_node_num_le0_, fp)
        != lma_node_num_le0_)
        return false;

    if (fwrite(nodes_ge1_, sizeof(LmaNodeGE1), lma_node_num_ge1_, fp)
        != lma_node_num_ge1_)
        return false;

    // 写入词条索引
    if (fwrite(lma_idx_buf_, sizeof(unsigned char), lma_idx_buf_len_, fp) !=
        lma_idx_buf_len_)
        return false;

    return true;
}

// 保存字典到文件
bool DictTrie::save_dict(const char *filename) {
    if (NULL == filename)
        return false;

    // 检查必要组件
    if (NULL == root_ || NULL == dict_list_)
        return false;

    // 获取拼音Trie和N-Gram实例
    SpellingTrie &spl_trie = SpellingTrie::get_instance();
    NGram &ngram = NGram::get_instance();

    // 打开文件并保存所有组件
    FILE *fp = fopen(filename, "wb");
    if (NULL == fp)
        return false;

    if (!spl_trie.save_spl_trie(fp) || !dict_list_->save_list(fp) ||
        !save_dict(fp) || !ngram.save_ngram(fp)) {
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}
#endif  // ___BUILD_MODEL___

// 从文件流加载字典
bool DictTrie::load_dict(QFile *fp) {
    if (NULL == fp)
        return false;

    // 读取字典元数据
    if (fp->read((char *)&lma_node_num_le0_, sizeof(uint32)) != sizeof(uint32))
        return false;

    if (fp->read((char *)&lma_node_num_ge1_, sizeof(uint32)) != sizeof(uint32))
        return false;

    if (fp->read((char *)&lma_idx_buf_len_, sizeof(uint32)) != sizeof(uint32))
        return false;

    if (fp->read((char *)&top_lmas_num_, sizeof(uint32)) != sizeof(uint32) ||
        top_lmas_num_ >= lma_idx_buf_len_)
        return false;

    // 释放旧资源
    free_resource(false);

    // 分配内存空间
    root_ = static_cast<LmaNodeLE0*>
        (malloc(lma_node_num_le0_ * sizeof(LmaNodeLE0)));
    nodes_ge1_ = static_cast<LmaNodeGE1*>
        (malloc(lma_node_num_ge1_ * sizeof(LmaNodeGE1)));
    lma_idx_buf_ = (unsigned char*)malloc(lma_idx_buf_len_);
    total_lma_num_ = lma_idx_buf_len_ / kLemmaIdSize;  // 计算总词条数

    // 分配拼音索引空间
    size_t buf_size = SpellingTrie::get_instance().get_spelling_num() + 1;
    assert(lma_node_num_le0_ <= buf_size);
    splid_le0_index_ = static_cast<uint16*>(malloc(buf_size * sizeof(uint16)));

    // 初始化解析空间
    parsing_marks_ = new ParsingMark[kMaxParsingMark];
    mile_stones_ = new MileStone[kMaxMileStone];
    reset_milestones(0, kFirstValidMileStoneHandle);

    // 检查内存分配
    if (NULL == root_ || NULL == nodes_ge1_ || NULL == lma_idx_buf_ ||
        NULL == splid_le0_index_ || NULL == parsing_marks_ ||
        NULL == mile_stones_) {
        free_resource(false);
        return false;
    }

    // 读取节点数据
    if (fp->read((char *)root_, sizeof(LmaNodeLE0) * lma_node_num_le0_)
        != sizeof(LmaNodeLE0) * lma_node_num_le0_)
        return false;

    if (fp->read((char *)nodes_ge1_, sizeof(LmaNodeGE1) * lma_node_num_ge1_)
        != sizeof(LmaNodeGE1) * lma_node_num_ge1_)
        return false;

    // 读取词条索引
    if (fp->read((char *)lma_idx_buf_, sizeof(unsigned char) * lma_idx_buf_len_) !=
        sizeof(unsigned char) * lma_idx_buf_len_)
        return false;

    // 构建LE0节点的拼音快速索引
    uint16 last_splid = kFullSplIdStart;
    size_t last_pos = 0;
    for (size_t i = 1; i < lma_node_num_le0_; i++) {
        // 填充索引间隙
        for (uint16 splid = last_splid; splid < root_[i].spl_idx; splid++)
            splid_le0_index_[splid - kFullSplIdStart] = last_pos;

        // 设置当前拼音索引
        splid_le0_index_[root_[i].spl_idx - kFullSplIdStart] =
            static_cast<uint16>(i);
        last_splid = root_[i].spl_idx;
        last_pos = i;
    }

    // 填充剩余拼音索引
    for (uint16 splid = last_splid + 1;
         splid < buf_size + kFullSplIdStart; splid++) {
        assert(static_cast<size_t>(splid - kFullSplIdStart) < buf_size);
        splid_le0_index_[splid - kFullSplIdStart] = last_pos + 1;
    }

    return true;
}

// 从文件加载字典
bool DictTrie::load_dict(const char *filename, LemmaIdType start_id,
                         LemmaIdType end_id) {
    // 参数校验
    if (NULL == filename || end_id <= start_id)
        return false;

    // 打开文件
    QFile file(QString::fromUtf8(filename));
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QFile *fp = &file;

    // 释放旧资源
    free_resource(true);

    // 创建词典列表
    dict_list_ = new DictList();
    if (NULL == dict_list_) {
        return false;
    }

    // 获取拼音Trie和N-Gram实例
    SpellingTrie &spl_trie = SpellingTrie::get_instance();
    NGram &ngram = NGram::get_instance();

    // 加载所有组件
    if (!spl_trie.load_spl_trie(fp) || !dict_list_->load_list(fp) ||
        !load_dict(fp) || !ngram.load_ngram(fp) ||
        total_lma_num_ > end_id - start_id + 1) {
        free_resource(true);
        return false;
    }

    return true;
}

// 从文件描述符加载字典
bool DictTrie::load_dict_fd(int sys_fd, long start_offset,
                            long length, LemmaIdType start_id,
                            LemmaIdType end_id) {
    // 参数校验
    if (start_offset < 0 || length <= 0 || end_id <= start_id)
        return false;

    // 打开文件
    QFile file;
    if (!file.open(sys_fd, QIODevice::ReadOnly))
        return false;
    QFile *fp = &file;

    // 定位到指定偏移
    if (!fp->seek(start_offset)) {
        return false;
    }

    // 释放旧资源
    free_resource(true);

    // 创建词典列表
    dict_list_ = new DictList();
    if (NULL == dict_list_) {
        return false;
    }

    // 获取拼音Trie和N-Gram实例
    SpellingTrie &spl_trie = SpellingTrie::get_instance();
    NGram &ngram = NGram::get_instance();

    // 加载所有组件
    if (!spl_trie.load_spl_trie(fp) || !dict_list_->load_list(fp) ||
        !load_dict(fp) || !ngram.load_ngram(fp) ||
        fp->pos() < start_offset + length ||
        total_lma_num_ > end_id - start_id + 1) {
        free_resource(true);
        return false;
    }

    return true;
}

// 填充单字词条概率项（LE0节点）
size_t DictTrie::fill_lpi_buffer(LmaPsbItem lpi_items[], size_t lpi_max,
                                 LmaNodeLE0 *node) {
    size_t lpi_num = 0;
    NGram& ngram = NGram::get_instance();
    // 遍历同音词条
    for (size_t homo = 0; homo < (size_t)node->num_of_homo; homo++) {
        // 获取词条ID
        lpi_items[lpi_num].id = get_lemma_id(node->homo_idx_buf_off +
                                             homo);
        lpi_items[lpi_num].lma_len = 1;  // 词条长度为1（单字）
        // 获取一元概率
        lpi_items[lpi_num].psb =
            static_cast<LmaScoreType>(ngram.get_uni_psb(lpi_items[lpi_num].id));
        lpi_num++;
        if (lpi_num >= lpi_max)  // 缓冲区满则退出
            break;
    }

    return lpi_num;
}

// 填充多字词条概率项（GE1节点）
size_t DictTrie::fill_lpi_buffer(LmaPsbItem lpi_items[], size_t lpi_max,
                                 size_t homo_buf_off, LmaNodeGE1 *node,
                                 uint16 lma_len) {
    size_t lpi_num = 0;
    NGram& ngram = NGram::get_instance();
    // 遍历同音词条
    for (size_t homo = 0; homo < (size_t)node->num_of_homo; homo++) {
        // 获取词条ID
        lpi_items[lpi_num].id = get_lemma_id(homo_buf_off + homo);
        lpi_items[lpi_num].lma_len = lma_len;  // 设置词条长度
        // 获取一元概率
        lpi_items[lpi_num].psb =
            static_cast<LmaScoreType>(ngram.get_uni_psb(lpi_items[lpi_num].id));
        lpi_num++;
        if (lpi_num >= lpi_max)  // 缓冲区满则退出
            break;
    }

    return lpi_num;
}

// 重置解析里程碑
void DictTrie::reset_milestones(uint16 from_step, MileStoneHandle from_handle) {
    if (0 == from_step) {
        // 完全重置
        parsing_marks_pos_ = 0;
        mile_stones_pos_ = kFirstValidMileStoneHandle;
    } else {
        // 部分重置（从指定里程碑开始）
        if (from_handle > 0 && from_handle < mile_stones_pos_) {
            mile_stones_pos_ = from_handle;
            MileStone *mile_stone = mile_stones_ + from_handle;
            parsing_marks_pos_ = mile_stone->mark_start;
        }
    }
}

// 扩展字典解析
MileStoneHandle DictTrie::extend_dict(MileStoneHandle from_handle,
                                      const DictExtPara *dep,
                                      LmaPsbItem *lpi_items, size_t lpi_max,
                                      size_t *lpi_num) {
    if (NULL == dep)
        return 0;

    // 根据当前状态选择扩展方式
    if (0 == from_handle) {
        // 从根节点开始扩展
        assert(0 == dep->splids_extended);
        return extend_dict0(from_handle, dep, lpi_items, lpi_max, lpi_num);
    }

    if (1 == dep->splids_extended) {
        // 从LE0到GE1的扩展
        return extend_dict1(from_handle, dep, lpi_items, lpi_max, lpi_num);
    }

    // 从GE1到GE1的扩展
    return extend_dict2(from_handle, dep, lpi_items, lpi_max, lpi_num);
}

// LE0节点扩展（根节点）
MileStoneHandle DictTrie::extend_dict0(MileStoneHandle from_handle,
                                       const DictExtPara *dep,
                                       LmaPsbItem *lpi_items,
                                       size_t lpi_max, size_t *lpi_num) {
    assert(NULL != dep && 0 == from_handle);
    *lpi_num = 0;
    MileStoneHandle ret_handle = 0;

    uint16 splid = dep->splids[dep->splids_extended];
    uint16 id_start = dep->id_start;
    uint16 id_num = dep->id_num;

    // 检查拼音缓存
    LpiCache& lpi_cache = LpiCache::get_instance();
    bool cached = lpi_cache.is_cached(splid);

    // 开始扩展
    LmaNodeLE0 *node = root_;
    // 获取拼音范围
    size_t son_start = splid_le0_index_[id_start - kFullSplIdStart];
    size_t son_end = splid_le0_index_[id_start + id_num - kFullSplIdStart];

    // 遍历拼音范围内的节点
    for (size_t son_pos = son_start; son_pos < son_end; son_pos++) {
        assert(1 == node->son_1st_off);
        LmaNodeLE0 *son = root_ + son_pos;
        assert(son->spl_idx >= id_start && son->spl_idx < id_start + id_num);

        // 填充概率项（如果需要）
        if (!cached && *lpi_num < lpi_max) {
            bool need_lpi = true;
            // 对于半角韵母，只取第一个节点
            if (spl_trie_->is_half_id_yunmu(splid) && son_pos != son_start)
                need_lpi = false;

            if (need_lpi)
                *lpi_num += fill_lpi_buffer(lpi_items + (*lpi_num),
                                            lpi_max - *lpi_num, son);
        }

        // 创建新里程碑（如果需要）
        if (son->spl_idx == id_start) {
            if (mile_stones_pos_ < kMaxMileStone &&
                parsing_marks_pos_ < kMaxParsingMark) {
                parsing_marks_[parsing_marks_pos_].node_offset = son_pos;
                parsing_marks_[parsing_marks_pos_].node_num = id_num;
                mile_stones_[mile_stones_pos_].mark_start = parsing_marks_pos_;
                mile_stones_[mile_stones_pos_].mark_num = 1;
                ret_handle = mile_stones_pos_;
                parsing_marks_pos_++;
                mile_stones_pos_++;
            }
        }

        if (son->spl_idx >= id_start + id_num -1)
            break;
    }

    return ret_handle;
}

// LE0到GE1节点扩展
MileStoneHandle DictTrie::extend_dict1(MileStoneHandle from_handle,
                                       const DictExtPara *dep,
                                       LmaPsbItem *lpi_items,
                                       size_t lpi_max, size_t *lpi_num) {
    assert(NULL != dep && from_handle > 0 && from_handle < mile_stones_pos_);
    MileStoneHandle ret_handle = 0;
    size_t ret_val = 0;

    uint16 id_start = dep->id_start;
    uint16 id_num = dep->id_num;

    // 获取当前里程碑
    MileStone *mile_stone = mile_stones_ + from_handle;

    // 遍历所有标记
    for (uint16 h_pos = 0; h_pos < mile_stone->mark_num; h_pos++) {
        ParsingMark p_mark = parsing_marks_[mile_stone->mark_start + h_pos];
        uint16 ext_num = p_mark.node_num;
        for (uint16 ext_pos = 0; ext_pos < ext_num; ext_pos++) {
            LmaNodeLE0 *node = root_ + p_mark.node_offset + ext_pos;
            size_t found_start = 0;
            size_t found_num = 0;

            // 遍历子节点
            for (size_t son_pos = 0; son_pos < (size_t)node->num_of_son; son_pos++) {
                assert(node->son_1st_off <= lma_node_num_ge1_);
                LmaNodeGE1 *son = nodes_ge1_ + node->son_1st_off + son_pos;

                // 检查是否在目标拼音范围内
                if (son->spl_idx >= id_start
                    && son->spl_idx < id_start + id_num) {
                    // 填充概率项
                    if (*lpi_num < lpi_max) {
                        size_t homo_buf_off = get_homo_idx_buf_offset(son);
                        *lpi_num += fill_lpi_buffer(lpi_items + (*lpi_num),
                                                    lpi_max - *lpi_num, homo_buf_off, son,
                                                    2);
                    }

                    // 记录匹配节点
                    if (0 == found_num) {
                        found_start = son_pos;
                    }
                    found_num++;
                }

                // 检查结束条件
                if (son->spl_idx >= id_start + id_num - 1 || son_pos ==
                                                                 (size_t)node->num_of_son - 1) {
                    if (found_num > 0) {
                        // 创建新标记
                        if (mile_stones_pos_ < kMaxMileStone &&
                            parsing_marks_pos_ < kMaxParsingMark) {
                            parsing_marks_[parsing_marks_pos_].node_offset =
                                node->son_1st_off + found_start;
                            parsing_marks_[parsing_marks_pos_].node_num = found_num;
                            if (0 == ret_val)
                                mile_stones_[mile_stones_pos_].mark_start =
                                    parsing_marks_pos_;
                            parsing_marks_pos_++;
                        }
                        ret_val++;
                    }
                    break;
                }
            }
        }
    }

    // 创建新里程碑
    if (ret_val > 0) {
        mile_stones_[mile_stones_pos_].mark_num = ret_val;
        ret_handle = mile_stones_pos_;
        mile_stones_pos_++;
        ret_val = 1;
    }

    return ret_handle;
}

// GE1到GE1节点扩展
MileStoneHandle DictTrie::extend_dict2(MileStoneHandle from_handle,
                                       const DictExtPara *dep,
                                       LmaPsbItem *lpi_items,
                                       size_t lpi_max, size_t *lpi_num) {
    assert(NULL != dep && from_handle > 0 && from_handle < mile_stones_pos_);
    MileStoneHandle ret_handle = 0;
    size_t ret_val = 0;

    uint16 id_start = dep->id_start;
    uint16 id_num = dep->id_num;

    // 获取当前里程碑
    MileStone *mile_stone = mile_stones_ + from_handle;

    // 遍历所有标记
    for (uint16 h_pos = 0; h_pos < mile_stone->mark_num; h_pos++) {
        ParsingMark p_mark = parsing_marks_[mile_stone->mark_start + h_pos];
        uint16 ext_num = p_mark.node_num;
        for (uint16 ext_pos = 0; ext_pos < ext_num; ext_pos++) {
            LmaNodeGE1 *node = nodes_ge1_ + p_mark.node_offset + ext_pos;
            size_t found_start = 0;
            size_t found_num = 0;

            // 遍历子节点
            for (size_t son_pos = 0; son_pos < (size_t)node->num_of_son; son_pos++) {
                assert(node->son_1st_off_l > 0 || node->son_1st_off_h > 0);
                LmaNodeGE1 *son = nodes_ge1_ + get_son_offset(node) + son_pos;

                // 检查是否在目标拼音范围内
                if (son->spl_idx >= id_start
                    && son->spl_idx < id_start + id_num) {
                    // 填充概率项
                    if (*lpi_num < lpi_max) {
                        size_t homo_buf_off = get_homo_idx_buf_offset(son);
                        *lpi_num += fill_lpi_buffer(lpi_items + (*lpi_num),
                                                    lpi_max - *lpi_num, homo_buf_off, son,
                                                    dep->splids_extended + 1);
                    }

                    // 记录匹配节点
                    if (0 == found_num) {
                        found_start = son_pos;
                    }
                    found_num++;
                }

                // 检查结束条件
                if (son->spl_idx >= id_start + id_num - 1 || son_pos ==
                                                                 (size_t)node->num_of_son - 1) {
                    if (found_num > 0) {
                        // 创建新标记
                        if (mile_stones_pos_ < kMaxMileStone &&
                            parsing_marks_pos_ < kMaxParsingMark) {
                            parsing_marks_[parsing_marks_pos_].node_offset =
                                get_son_offset(node) + found_start;
                            parsing_marks_[parsing_marks_pos_].node_num = found_num;
                            if (0 == ret_val)
                                mile_stones_[mile_stones_pos_].mark_start =
                                    parsing_marks_pos_;
                            parsing_marks_pos_++;
                        }
                        ret_val++;
                    }
                    break;
                }
            }
        }
    }

    // 创建新里程碑
    if (ret_val > 0) {
        mile_stones_[mile_stones_pos_].mark_num = ret_val;
        ret_handle = mile_stones_pos_;
        mile_stones_pos_++;
    }

    return ret_handle;
}

// 尝试扩展拼音序列
bool DictTrie::try_extend(const uint16 *splids, uint16 splid_num,
                          LemmaIdType id_lemma) {
    if (0 == splid_num || NULL == splids)
        return false;

    // 从根节点开始
    void *node = root_ + splid_le0_index_[splids[0] - kFullSplIdStart];

    // 遍历拼音序列
    for (uint16 pos = 1; pos < splid_num; pos++) {
        if (1 == pos) {  // 第二层（LE0到GE1）
            LmaNodeLE0 *node_le0 = reinterpret_cast<LmaNodeLE0*>(node);
            LmaNodeGE1 *node_son;
            uint16 son_pos;
            // 在子节点中查找匹配
            for (son_pos = 0; son_pos < static_cast<uint16>(node_le0->num_of_son);
                 son_pos++) {
                assert(node_le0->son_1st_off <= lma_node_num_ge1_);
                node_son = nodes_ge1_ + node_le0->son_1st_off + son_pos;
                if (node_son->spl_idx == splids[pos])
                    break;
            }
            // 找到则继续，否则失败
            if (son_pos < node_le0->num_of_son)
                node = reinterpret_cast<void*>(node_son);
            else
                return false;
        } else {  // 第三层及以上（GE1到GE1）
            LmaNodeGE1 *node_ge1 = reinterpret_cast<LmaNodeGE1*>(node);
            LmaNodeGE1 *node_son;
            uint16 son_pos;
            // 在子节点中查找匹配
            for (son_pos = 0; son_pos < static_cast<uint16>(node_ge1->num_of_son);
                 son_pos++) {
                assert(node_ge1->son_1st_off_l > 0 || node_ge1->son_1st_off_h > 0);
                node_son = nodes_ge1_ + get_son_offset(node_ge1) + son_pos;
                if (node_son->spl_idx == splids[pos])
                    break;
            }
            // 找到则继续，否则失败
            if (son_pos < node_ge1->num_of_son)
                node = reinterpret_cast<void*>(node_son);
            else
                return false;
        }
    }

    // 检查词条ID是否匹配
    if (1 == splid_num) {  // 单字词
        LmaNodeLE0* node_le0 = reinterpret_cast<LmaNodeLE0*>(node);
        size_t num_of_homo = (size_t)node_le0->num_of_homo;
        for (size_t homo_pos = 0; homo_pos < num_of_homo; homo_pos++) {
            LemmaIdType id_this = get_lemma_id(node_le0->homo_idx_buf_off + homo_pos);
            char16 str[2];
            get_lemma_str(id_this, str, 2);
            if (id_this == id_lemma)
                return true;
        }
    } else {  // 多字词
        LmaNodeGE1* node_ge1 = reinterpret_cast<LmaNodeGE1*>(node);
        size_t num_of_homo = (size_t)node_ge1->num_of_homo;
        for (size_t homo_pos = 0; homo_pos < num_of_homo; homo_pos++) {
            size_t node_homo_off = get_homo_idx_buf_offset(node_ge1);
            if (get_lemma_id(node_homo_off + homo_pos) == id_lemma)
                return true;
        }
    }

    return false;
}

// 获取词条概率项（LPI）
size_t DictTrie::get_lpis(const uint16* splid_str, uint16 splid_str_len,
                          LmaPsbItem* lma_buf, size_t max_lma_buf) {
    if (splid_str_len > kMaxLemmaSize)
        return 0;

#define MAX_EXTENDBUF_LEN 200  // 最大扩展缓冲区长度

    // 使用联合缓冲区避免内存碎片
    size_t* node_buf1[MAX_EXTENDBUF_LEN];
    size_t* node_buf2[MAX_EXTENDBUF_LEN];
    LmaNodeLE0** node_fr_le0 = reinterpret_cast<LmaNodeLE0**>(node_buf1);
    LmaNodeLE0** node_to_le0 = reinterpret_cast<LmaNodeLE0**>(node_buf2);
    LmaNodeGE1** node_fr_ge1 = NULL;
    LmaNodeGE1** node_to_ge1 = NULL;

    // 从根节点开始
    size_t node_fr_num = 1;
    size_t node_to_num = 0;
    node_fr_le0[0] = root_;
    if (NULL == node_fr_le0[0])
        return 0;

    size_t spl_pos = 0;

    // 逐层扩展
    while (spl_pos < splid_str_len) {
        uint16 id_num = 1;
        uint16 id_start = splid_str[spl_pos];
        // 处理半角拼音
        if (spl_trie_->is_half_id(splid_str[spl_pos])) {
            id_num = spl_trie_->half_to_full(splid_str[spl_pos], &id_start);
            assert(id_num > 0);
        }

        // 不同层级的扩展逻辑
        if (0 == spl_pos) {  // 根节点到LE0节点
            for (size_t node_fr_pos = 0; node_fr_pos < node_fr_num; node_fr_pos++) {
                LmaNodeLE0 *node = node_fr_le0[node_fr_pos];
                assert(node == root_ && 1 == node_fr_num);
                // 获取拼音范围内的子节点
                size_t son_start = splid_le0_index_[id_start - kFullSplIdStart];
                size_t son_end = splid_le0_index_[id_start + id_num - kFullSplIdStart];
                for (size_t son_pos = son_start; son_pos < son_end; son_pos++) {
                    assert(1 == node->son_1st_off);
                    LmaNodeLE0 *node_son = root_ + son_pos;
                    assert(node_son->spl_idx >= id_start
                           && node_son->spl_idx < id_start + id_num);
                    // 添加到目标缓冲区
                    if (node_to_num < MAX_EXTENDBUF_LEN) {
                        node_to_le0[node_to_num] = node_son;
                        node_to_num++;
                    }
                    // 提前结束（已到最后一个）
                    if (node_son->spl_idx >= id_start + id_num - 1)
                        break;
                }
            }

            spl_pos++;
            if (spl_pos >= splid_str_len || node_to_num == 0)
                break;
            // 准备下一轮扩展
            LmaNodeLE0** node_tmp = node_fr_le0;
            node_fr_le0 = node_to_le0;
            node_to_le0 = NULL;
            node_to_ge1 = reinterpret_cast<LmaNodeGE1**>(node_tmp);
        } else if (1 == spl_pos) {  // LE0到GE1节点
            for (size_t node_fr_pos = 0; node_fr_pos < node_fr_num; node_fr_pos++) {
                LmaNodeLE0 *node = node_fr_le0[node_fr_pos];
                for (size_t son_pos = 0; son_pos < (size_t)node->num_of_son;
                     son_pos++) {
                    assert(node->son_1st_off <= lma_node_num_ge1_);
                    LmaNodeGE1 *node_son = nodes_ge1_ + node->son_1st_off + son_pos;
                    // 检查是否在拼音范围内
                    if (node_son->spl_idx >= id_start
                        && node_son->spl_idx < id_start + id_num) {
                        if (node_to_num < MAX_EXTENDBUF_LEN) {
                            node_to_ge1[node_to_num] = node_son;
                            node_to_num++;
                        }
                    }
                    // 提前结束（已到最后一个）
                    if (node_son->spl_idx >= id_start + id_num - 1)
                        break;
                }
            }

            spl_pos++;
            if (spl_pos >= splid_str_len || node_to_num == 0)
                break;
            // 准备下一轮扩展
            node_fr_ge1 = node_to_ge1;
            node_to_ge1 = reinterpret_cast<LmaNodeGE1**>(node_fr_le0);
            node_fr_le0 = NULL;
            node_to_le0 = NULL;
        } else {  // GE1到GE1节点
            for (size_t node_fr_pos = 0; node_fr_pos < node_fr_num; node_fr_pos++) {
                LmaNodeGE1 *node = node_fr_ge1[node_fr_pos];
                for (size_t son_pos = 0; son_pos < (size_t)node->num_of_son;
                     son_pos++) {
                    assert(node->son_1st_off_l > 0 || node->son_1st_off_h > 0);
                    LmaNodeGE1 *node_son = nodes_ge1_ + get_son_offset(node) + son_pos;
                    // 检查是否在拼音范围内
                    if (node_son->spl_idx >= id_start
                        && node_son->spl_idx < id_start + id_num) {
                        if (node_to_num < MAX_EXTENDBUF_LEN) {
                            node_to_ge1[node_to_num] = node_son;
                            node_to_num++;
                        }
                    }
                    // 提前结束（已到最后一个）
                    if (node_son->spl_idx >= id_start + id_num - 1)
                        break;
                }
            }

            spl_pos++;
            if (spl_pos >= splid_str_len || node_to_num == 0)
                break;
            // 准备下一轮扩展
            LmaNodeGE1 **node_tmp = node_fr_ge1;
            node_fr_ge1 = node_to_ge1;
            node_to_ge1 = node_tmp;
        }

        // 更新节点数量
        node_fr_num = node_to_num;
        node_to_num = 0;
    }

    // 没有找到匹配则返回
    if (0 == node_to_num)
        return 0;

    // 获取N-Gram实例
    NGram &ngram = NGram::get_instance();
    size_t lma_num = 0;

    // 特殊处理：单字韵母拼音（如a, o, e）
    if (1 == splid_str_len && spl_trie_->is_half_id_yunmu(splid_str[0]))
        node_to_num = node_to_num > 0 ? 1 : 0;

    // 收集词条概率项
    for (size_t node_pos = 0; node_pos < node_to_num; node_pos++) {
        size_t num_of_homo = 0;
        if (spl_pos <= 1) {  // LE0节点（单字词）
            LmaNodeLE0* node_le0 = node_to_le0[node_pos];
            num_of_homo = (size_t)node_le0->num_of_homo;
            for (size_t homo_pos = 0; homo_pos < num_of_homo; homo_pos++) {
                size_t ch_pos = lma_num + homo_pos;
                // 填充词条信息
                lma_buf[ch_pos].id =
                    get_lemma_id(node_le0->homo_idx_buf_off + homo_pos);
                lma_buf[ch_pos].lma_len = 1;
                lma_buf[ch_pos].psb =
                    static_cast<LmaScoreType>(ngram.get_uni_psb(lma_buf[ch_pos].id));

                if (lma_num + homo_pos >= max_lma_buf - 1)
                    break;
            }
        } else {  // GE1节点（多字词）
            LmaNodeGE1* node_ge1 = node_to_ge1[node_pos];
            num_of_homo = (size_t)node_ge1->num_of_homo;
            for (size_t homo_pos = 0; homo_pos < num_of_homo; homo_pos++) {
                size_t ch_pos = lma_num + homo_pos;
                size_t node_homo_off = get_homo_idx_buf_offset(node_ge1);
                // 填充词条信息
                lma_buf[ch_pos].id = get_lemma_id(node_homo_off + homo_pos);
                lma_buf[ch_pos].lma_len = splid_str_len;
                lma_buf[ch_pos].psb =
                    static_cast<LmaScoreType>(ngram.get_uni_psb(lma_buf[ch_pos].id));

                if (lma_num + homo_pos >= max_lma_buf - 1)
                    break;
            }
        }

        lma_num += num_of_homo;
        if (lma_num >= max_lma_buf) {  // 缓冲区满
            lma_num = max_lma_buf;
            break;
        }
    }
    return lma_num;
}

// 获取词条字符串
uint16 DictTrie::get_lemma_str(LemmaIdType id_lemma, char16 *str_buf,
                               uint16 str_max) {
    return dict_list_->get_lemma_str(id_lemma, str_buf, str_max);
}

// 获取词条的拼音ID序列
uint16 DictTrie::get_lemma_splids(LemmaIdType id_lemma, uint16 *splids,
                                  uint16 splids_max, bool arg_valid) {
    // 获取词条字符串
    char16 lma_str[kMaxLemmaSize + 1];
    uint16 lma_len = get_lemma_str(id_lemma, lma_str, kMaxLemmaSize + 1);
    assert((!arg_valid && splids_max >= lma_len) || lma_len == splids_max);

    // 准备拼音矩阵
    uint16 spl_mtrx[kMaxLemmaSize * 5];
    uint16 spl_start[kMaxLemmaSize + 1];
    spl_start[0] = 0;
    uint16 try_num = 1;

    // 遍历每个汉字
    for (uint16 pos = 0; pos < lma_len; pos++) {
        uint16 cand_splids_this = 0;
        // 如果参数有效且是完整拼音ID
        if (arg_valid && spl_trie_->is_full_id(splids[pos])) {
            spl_mtrx[spl_start[pos]] = splids[pos];
            cand_splids_this = 1;
        } else {
            // 获取汉字对应的拼音ID
            cand_splids_this = dict_list_->get_splids_for_hanzi(lma_str[pos],
                                                                arg_valid ? splids[pos] : 0, spl_mtrx + spl_start[pos],
                                                                kMaxLemmaSize * 5 - spl_start[pos]);
            assert(cand_splids_this > 0);
        }
        spl_start[pos + 1] = spl_start[pos] + cand_splids_this;
        try_num *= cand_splids_this;  // 计算总尝试次数
    }

    // 尝试所有可能的拼音组合
    for (uint16 try_pos = 0; try_pos < try_num; try_pos++) {
        uint16 mod = 1;
        // 生成当前尝试的拼音序列
        for (uint16 pos = 0; pos < lma_len; pos++) {
            uint16 radix = spl_start[pos + 1] - spl_start[pos];
            splids[pos] = spl_mtrx[ spl_start[pos] + try_pos / mod % radix];
            mod *= radix;
        }

        // 检查是否匹配
        if (try_extend(splids, lma_len, id_lemma))
            return lma_len;
    }

    return 0;  // 没有找到匹配
}

// 设置非系统词条的总频率
void DictTrie::set_total_lemma_count_of_others(size_t count) {
    NGram& ngram = NGram::get_instance();
    ngram.set_total_freq_none_sys(count);
}

// SCI ID转汉字
void DictTrie::convert_to_hanzis(char16 *str, uint16 str_len) {
    return dict_list_->convert_to_hanzis(str, str_len);
}

// 汉字转SCI ID（未完全实现）
void DictTrie::convert_to_scis_ids(char16 *str, uint16 str_len) {
    return dict_list_->convert_to_scis_ids(str, str_len);
}

// 获取词条ID（通过汉字）
LemmaIdType DictTrie::get_lemma_id(const char16 lemma_str[], uint16 lemma_len) {
    if (NULL == lemma_str || lemma_len > kMaxLemmaSize)
        return 0;

    return dict_list_->get_lemma_id(lemma_str, lemma_len);
}

// 获取高频词预测
size_t DictTrie::predict_top_lmas(size_t his_len, NPredictItem *npre_items,
                                  size_t npre_max, size_t b4_used) {
    NGram &ngram = NGram::get_instance();

    size_t item_num = 0;
    // 计算高频词在索引缓冲区的位置
    size_t top_lmas_id_offset = lma_idx_buf_len_ / kLemmaIdSize - top_lmas_num_;
    size_t top_lmas_pos = 0;

    // 填充高频词预测项
    while (item_num < npre_max && top_lmas_pos < top_lmas_num_) {
        memset(npre_items + item_num, 0, sizeof(NPredictItem));
        LemmaIdType top_lma_id = get_lemma_id(top_lmas_id_offset + top_lmas_pos);
        top_lmas_pos += 1;
        // 获取词条字符串
        if (dict_list_->get_lemma_str(top_lma_id,
                                      npre_items[item_num].pre_hzs,
                                      kMaxLemmaSize - 1) == 0) {
            continue;
        }
        // 获取概率
        npre_items[item_num].psb = ngram.get_uni_psb(top_lma_id);
        npre_items[item_num].his_len = his_len;
        item_num++;
    }
    return item_num;
}

// 预测输入（基于历史）
size_t DictTrie::predict(const char16 *last_hzs, uint16 hzs_len,
                         NPredictItem *npre_items, size_t npre_max,
                         size_t b4_used) {
    return dict_list_->predict(last_hzs, hzs_len, npre_items, npre_max, b4_used);
}
}  // namespace ime_pinyin
