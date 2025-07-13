#ifndef PINYINIME_ANDPY_INCLUDE_LPICACHE_H__
#define PINYINIME_ANDPY_INCLUDE_LPICACHE_H__

#include <stdlib.h>
#include "./searchutility.h"
#include "./spellingtrie.h"

namespace ime_pinyin {

/**
 * 用于缓存半拼音ID对应的候选词条列表(LmaPsbItem)
 */
class LpiCache {
private:
    static LpiCache *instance_;  ///< 单例实例指针
    static const int kMaxLpiCachePerId = 15;  ///< 每个ID最大缓存候选词条数

    LmaPsbItem *lpi_cache_;      ///< 候选词条缓存区
    uint16 *lpi_cache_len_;      ///< 各ID对应的缓存长度数组

public:
    LpiCache();   ///< 构造函数
    ~LpiCache();  ///< 析构函数

    /// 获取单例实例引用
    static LpiCache& get_instance();

    /**
   * 检查指定拼音ID的候选列表是否已缓存
   *
   * @param splid 拼音ID
   * @return 若为完整拼音ID返回false（仅缓存半拼音ID）
   */
    bool is_cached(uint16 splid);

    /**
   * 将候选列表存入缓存
   *
   * @param splid 半拼音ID
   * @param lpi_items 候选词条数组
   * @param lpi_num 候选词条数量
   * @return 实际缓存数量（若超过缓冲区容量将被截断）
   *
   * @note 调用者需确保：splid为半ID且lpi_items非空
   */
    size_t put_cache(uint16 splid, LmaPsbItem lpi_items[], size_t lpi_num);

    /**
   * 获取缓存的候选列表
   *
   * @param splid 半拼音ID
   * @param lpi_items 输出缓冲区
   * @param lpi_max 缓冲区容量
   * @return 实际获取的候选词条数量
   *
   * @note 调用者需确保：splid为半ID且lpi_items非空
   */
    size_t get_cache(uint16 splid, LmaPsbItem lpi_items[], size_t lpi_max);
};

}  // namespace

#endif  // PINYINIME_ANDPY_INCLUDE_LPICACHE_H__
