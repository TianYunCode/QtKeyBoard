#ifndef PINYINIME_GLOBAL_H
#define PINYINIME_GLOBAL_H

#include <QtCore/qglobal.h>  // 包含Qt全局定义

// 动态库导入导出宏定义
#if defined(PINYINIME_LIBRARY)
#  define PINYINIMESHARED_EXPORT Q_DECL_EXPORT  ///< 构建库时导出符号
#else
#  define PINYINIMESHARED_EXPORT Q_DECL_IMPORT  ///< 使用库时导入符号
#endif

#endif // PINYINIME_GLOBAL_H
