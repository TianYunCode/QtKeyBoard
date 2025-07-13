#ifndef PINYINIME_INCLUDE_SYNC_H__
#define PINYINIME_INCLUDE_SYNC_H__

#define ___SYNC_ENABLED___  ///< 同步功能启用标志

#ifdef ___SYNC_ENABLED___

#include "userdict.h"

namespace ime_pinyin {

/**
 * 用户词典同步类（非线程安全）
 *
 * 典型调用流程：
 *   begin() ->
 *   put_lemmas() x N ->
 *   {
 *     get_lemmas() ->
 *     [ get_last_got_count() ] ->
 *     clear_last_got() ->
 *   } x N ->
 *   finish()
 */
class Sync {
public:
    Sync();   ///< 构造函数
    ~Sync();  ///< 析构函数

    // ===================== 同步参数常量 =====================
    static const int kUserDictMaxLemmaCount = 5000;    ///< 最大用户词条数
    static const int kUserDictMaxLemmaSize = 200000;   ///< 最大用户词典尺寸
    static const int kUserDictRatio = 20;             ///< 用户词典比例系数

    /**
   * 开始同步会话
   *
   * @param filename 词典文件名
   * @return 成功返回true
   */
    bool begin(const char * filename);

    /**
   * 合并从同步服务器下载的词条到本地词典
   *
   * @param lemmas 词条字符串（UTF16LE编码）
   * @param len 字符串长度
   * @return 成功合并的词条数量
   */
    int put_lemmas(char16 * lemmas, int len);

    /**
   * 获取本地新增用户词条
   *
   * @param str 输出缓冲区（UTF16LE编码）
   * @param size 缓冲区大小
   * @return 返回的缓冲区长度（UTF16LE单位）
   */
    int get_lemmas(char16 * str, int size);

    /// 获取最近get_lemmas()返回的词条数
    int get_last_got_count();

    /// 获取需要同步的总词条数
    int get_total_count();

    /// 清除最近get_lemmas()获取的词条
    void clear_last_got();

    /// 结束同步会话
    void finish();

    /// 获取同步容量
    int get_capacity();

private:
    UserDict * userdict_;  ///< 用户词典对象
    char * dictfile_;      ///< 词典文件路径
    int last_count_;       ///< 最近处理的词条数
};

}

#endif

#endif  // PINYINIME_INCLUDE_SYNC_H__
