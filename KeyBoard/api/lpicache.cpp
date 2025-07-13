#include <assert.h>
#include "lpicache.h"

namespace ime_pinyin {

// 静态实例指针初始化
LpiCache* LpiCache::instance_ = NULL;

// 构造函数：初始化拼音项缓存
LpiCache::LpiCache() {
    // 为所有有效拼音ID分配缓存空间（每ID最多kMaxLpiCachePerId个条目）
    lpi_cache_ = new LmaPsbItem[kFullSplIdStart * kMaxLpiCachePerId];
    // 为每个拼音ID的缓存长度分配空间
    lpi_cache_len_ = new uint16[kFullSplIdStart];

    // 确保内存分配成功
    assert(NULL != lpi_cache_);
    assert(NULL != lpi_cache_len_);

    // 初始化所有拼音ID的缓存长度为0
    for (uint16 id = 0; id < kFullSplIdStart; id++)
        lpi_cache_len_[id] = 0;
}

// 析构函数：释放缓存资源
LpiCache::~LpiCache() {
    if (NULL != lpi_cache_)
        delete [] lpi_cache_;  // 释放条目缓存

    if (NULL != lpi_cache_len_)
        delete [] lpi_cache_len_;  // 释放长度数组
}

// 获取单例实例
LpiCache& LpiCache::get_instance() {
    if (NULL == instance_) {
        instance_ = new LpiCache();  // 首次调用时创建实例
        assert(NULL != instance_);   // 确保实例创建成功
    }
    return *instance_;  // 返回单例引用
}

// 检查指定拼音ID是否有缓存
bool LpiCache::is_cached(uint16 splid) {
    // 仅处理有效拼音ID（小于kFullSplIdStart）
    if (splid >= kFullSplIdStart)
        return false;
    // 根据缓存长度判断是否存在缓存
    return lpi_cache_len_[splid] != 0;
}

// 将拼音项存入缓存
size_t LpiCache::put_cache(uint16 splid, LmaPsbItem lpi_items[],
                           size_t lpi_num) {
    // 计算实际可缓存数量（不超过单ID最大限制）
    uint16 num = kMaxLpiCachePerId;
    if (num > lpi_num)
        num = static_cast<uint16>(lpi_num);

    // 定位该拼音ID的缓存起始位置
    LmaPsbItem *lpi_cache_this = lpi_cache_ + splid * kMaxLpiCachePerId;

    // 复制数据到缓存
    for (uint16 pos = 0; pos < num; pos++)
        lpi_cache_this[pos] = lpi_items[pos];

    // 更新缓存长度并返回实际缓存数量
    lpi_cache_len_[splid] = num;
    return num;
}

// 从缓存获取拼音项
size_t LpiCache::get_cache(uint16 splid, LmaPsbItem lpi_items[],
                           size_t lpi_max) {
    // 限制获取数量不超过实际缓存量和请求上限
    if (lpi_max > lpi_cache_len_[splid])
        lpi_max = lpi_cache_len_[splid];

    // 定位该拼音ID的缓存起始位置
    LmaPsbItem *lpi_cache_this = lpi_cache_ + splid * kMaxLpiCachePerId;

    // 复制缓存数据到输出数组
    for (uint16 pos = 0; pos < lpi_max; pos++) {
        lpi_items[pos] = lpi_cache_this[pos];
    }
    return lpi_max;  // 返回实际获取数量
}

}  // namespace ime_pinyin
