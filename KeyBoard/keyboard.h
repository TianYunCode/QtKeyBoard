// #ifndef KEYBOARD_H
// #define KEYBOARD_H

// #include <QMainWindow>
// #include <QPushButton>
// #include <QMap>
// #include <QMouseEvent>
// #include <keyboard/api/pinyinime.h>
// #include "customerqpushbutton.h"

// QT_BEGIN_NAMESPACE
// namespace Ui { class keyboard; }
// QT_END_NAMESPACE

// // 拼音词典资源路径
// #define PINYIN_DICT_PATH ":/pinyinData/dict_pinyin.dat"

// // 用户词典资源路径
// #define USER_PINYIN_DICT_PATH ":/pinyinData/user_dict_pinyin.dat"

// /**
//  * 软键盘主窗口类
//  * 实现中英文混合输入功能，集成Google拼音输入法引擎
//  */
// class keyboard : public QMainWindow
// {
//     Q_OBJECT

// public:
//     /**
//      * 构造函数
//      *
//      * @param parent 父窗口
//      * @param datapath 系统词典路径（默认使用资源文件）
//      * @param userdatapt 用户词典路径（默认使用资源文件）
//      */
//     explicit keyboard(QWidget *parent = nullptr,
//                       const char* datapath = PINYIN_DICT_PATH,
//                       const char* userdatapt = USER_PINYIN_DICT_PATH);
//     ~keyboard();

//     /// 初始化键盘Tab键顺序
//     void InitTabOrder(void);

//     /// 初始化按键映射表
//     void Init_MAP_Key_value(void);

// public:
//     QMap<int,QPushButton *>keyboardmap;  ///< 物理按键映射表（按键ID->按钮）
//     QMap<size_t,QPushButton *>Resultmap; ///< 候选词结果映射表（索引->按钮）

//     bool keycapsmode = false;  ///< 大写锁定状态
//     bool IsZhMode = false;     ///< 中文输入模式状态

// private:
//     const char *dict_datapath = nullptr;      ///< 系统词典路径
//     const char *dict_userdatapath = nullptr;  ///< 用户词典路径

//     QStringList ResultStr;          ///< 候选词结果列表
//     quint16 CurrentResultPageNUM = 1;  ///< 当前候选词页码
//     quint16 TotalResultPageNUM = 1;    ///< 候选词总页数

//     QPoint m_dragPosition;  // 拖动位置变量
//     bool m_dragging = false; // 拖动状态标志

// public:
//     /// 键盘输入模式枚举
//     enum KEYBOARD_MODE
//     {
//         NUM_ONLY = 0,   ///< 纯数字模式
//         EN_ONLY,        ///< 纯英文模式
//         ANY,            ///< 中英文混合模式
//     };

//     /**
//      * 显示软键盘
//      *
//      * @param title 输入提示标题（默认："请输入内容："）
//      * @param str 初始文本内容（默认空）
//      */
//     void showKeyboard(QString title = "请输入内容：" ,QString str = "");

//     /// 设置输入提示标题
//     void set_editTips(QString title = "请输入内容：");

//     /// 设置输入框内容
//     void set_editBox(QString str = "");

//     /// 设置键盘输入模式
//     void set_keyboardmode(KEYBOARD_MODE mode);

// protected:
//     /// 鼠标按下事件
//     void mousePressEvent(QMouseEvent *event) override;

//     /// 鼠标移动事件
//     void mouseMoveEvent(QMouseEvent *event) override;

//     /// 鼠标释放事件
//     void mouseReleaseEvent(QMouseEvent *event) override;

// private:
//     /// 初始化Google拼音引擎
//     void Google_PinyinInit(const char* datapath = nullptr ,const char* userdatapt = nullptr);

//     /// 初始化候选词区域
//     void ZhResultWigdetInit();

//     /// 设置大写锁定模式
//     void set_capsLockmode(bool enable);

//     /// 更新符号显示状态
//     void UpdateSymbolDisplay();

//     /// 写入字母到输入框
//     void writeABC(int index);

//     /**
//      * 追加内容到输入窗口
//      *
//      * @param str 待添加字符串
//      * @param result 是否为候选词选择（影响输入逻辑）
//      */
//     void AppendOnKeyInputWindow(QString &str,bool result = false);

//     /// 显示搜索结果（候选词列表）
//     void ShowSearchResult(QStringList &result);

//     /// 显示下一页候选词
//     void ShowNextPageResult();

//     /// 显示上一页候选词
//     void ShowLastPageResult();

//     /// 隐藏候选词区域
//     void SetResultHidden();

//     /// 设置数字键可用状态
//     void SetEnableNUM(bool state);

//     /// 设置字母键可用状态
//     void SetEnableEN(bool state);

// signals:
//     /**
//      * 编辑完成信号
//      *
//      * @param 最终确认的文本内容
//      */
//     void editisModifiedok(QString);

// private slots:
//     // ===================== 功能键槽函数 =====================
//     void on_key_clear_clicked();  ///< 清空输入框
//     void on_key_caps_clicked();   ///< 大小写切换
//     void on_key_del_clicked();    ///< 删除字符
//     void on_key_ok_clicked();     ///< 确认输入

//     // ===================== 数字键槽函数 =====================
//     void on_key_1_clicked();
//     void on_key_2_clicked();
//     void on_key_3_clicked();
//     void on_key_4_clicked();
//     void on_key_5_clicked();
//     void on_key_6_clicked();
//     void on_key_7_clicked();
//     void on_key_8_clicked();
//     void on_key_9_clicked();
//     void on_key_0_clicked();

//     // ===================== 字母键槽函数 =====================
//     void on_key_q_clicked();
//     void on_key_w_clicked();
//     void on_key_e_clicked();
//     void on_key_r_clicked();
//     void on_key_t_clicked();
//     void on_key_y_clicked();
//     void on_key_u_clicked();
//     void on_key_i_clicked();
//     void on_key_o_clicked();
//     void on_key_p_clicked();
//     void on_key_a_clicked();
//     void on_key_s_clicked();
//     void on_key_d_clicked();
//     void on_key_f_clicked();
//     void on_key_g_clicked();
//     void on_key_h_clicked();
//     void on_key_j_clicked();
//     void on_key_k_clicked();
//     void on_key_l_clicked();
//     void on_key_z_clicked();
//     void on_key_x_clicked();
//     void on_key_c_clicked();
//     void on_key_v_clicked();
//     void on_key_b_clicked();
//     void on_key_n_clicked();
//     void on_key_m_clicked();

//     // ===================== 特殊功能槽函数 =====================
//     void on_key_language_clicked();  ///< 中英文切换
//     void on_key_comma_clicked();     ///< 逗号
//     void on_key_dash_clicked();      ///< 破折号
//     void on_key_virgule_clicked();   ///< 斜杠
//     void on_key_colon_clicked();     ///< 冒号
//     void on_key_at_clicked();        ///< @符号
//     void on_key_point_clicked();     ///< 句点

//     // ===================== 输入处理槽函数 =====================
//     void on_key_pinyininput_textChanged(const QString &arg1);  ///< 拼音输入变化
//     void on_NEXTpushButton_clicked();  ///< 下一页候选词
//     void on_LASTpushButton_clicked();  ///< 上一页候选词

//     // ===================== 候选词选择槽函数 =====================
//     void slotResult(customerqpushbutton *pbtn);

// private:
//     Ui::keyboard *ui;  ///< UI界面指针

//     QVector<QPushButton*> candidateButtons; // 存储候选词按钮
// };

// #endif // KEYBOARD_H

/**********************************************************************************************/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QMainWindow>
#include <QPushButton>
#include <QMap>
#include <QMouseEvent>
#include <QMessageBox>
#include <api/pinyinime.h>

QT_BEGIN_NAMESPACE
namespace Ui { class keyboard; }
QT_END_NAMESPACE

// 拼音词典资源路径
#define PINYIN_DICT_PATH ":/pinyinData/dict_pinyin.dat"

// 用户词典资源路径
#define USER_PINYIN_DICT_PATH ":/pinyinData/user_dict_pinyin.dat"

// 每页显示的候选词数量
#define CANDIDATE_WORDS_PER_PAGE 8

/**
 * 软键盘主窗口类
 * 实现中英文混合输入功能，集成Google拼音输入法引擎
 */
class keyboard : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * 构造函数
     *
     * @param parent 父窗口
     * @param datapath 系统词典路径（默认使用资源文件）
     * @param userdatapt 用户词典路径（默认使用资源文件）
     */
    explicit keyboard(QWidget *parent = nullptr, const char* datapath = PINYIN_DICT_PATH, const char* userdatapt = USER_PINYIN_DICT_PATH);
    ~keyboard();

    /// 初始化键盘Tab键顺序
    void InitTabOrder(void);

    /// 初始化按键映射表
    void Init_MAP_Key_value(void);

public:
    QMap<int,QPushButton *>keyboardmap;  ///< 物理按键映射表（按键ID->按钮）

    bool keycapsmode = false;  ///< 大写锁定状态
    bool IsZhMode = false;     ///< 中文输入模式状态

private:
    const char *dict_datapath = nullptr;      ///< 系统词典路径
    const char *dict_userdatapath = nullptr;  ///< 用户词典路径

    QStringList ResultStr;          ///< 候选词结果列表
    quint16 CurrentResultPageNUM = 1;  ///< 当前候选词页码
    quint16 TotalResultPageNUM = 1;    ///< 候选词总页数

    QPoint m_dragPosition;  // 拖动位置变量
    bool m_dragging = false; // 拖动状态标志

public:
    /// 键盘输入模式枚举
    enum KEYBOARD_MODE
    {
        NUM_ONLY = 0,   ///< 纯数字模式
        EN_ONLY,        ///< 纯英文模式
        ANY,            ///< 中英文混合模式
    };

    /**
     * 显示软键盘
     *
     * @param title 输入提示标题（默认："请输入内容："）
     * @param str 初始文本内容（默认空）
     */
    void showKeyboard(QString title = "请输入内容：" ,QString str = "");

    /// 设置输入提示标题
    void set_editTips(QString title = "请输入内容：");

    /// 设置输入框内容
    void set_editBox(QString str = "");

    /// 设置键盘输入模式
    void set_keyboardmode(KEYBOARD_MODE mode);

protected:
    /// 鼠标按下事件
    void mousePressEvent(QMouseEvent *event) override;

    /// 鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;

    /// 鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    /// 初始化Google拼音引擎
    void Google_PinyinInit(const char* datapath = nullptr ,const char* userdatapt = nullptr);

    /// 初始化候选词区域
    void ZhResultWigdetInit();

    /// 设置大写锁定模式
    void set_capsLockmode(bool enable);

    /// 更新符号显示状态
    void UpdateSymbolDisplay();

    /// 写入字母到输入框
    void writeABC(int index);

    /**
     * 追加内容到输入窗口
     *
     * @param str 待添加字符串
     * @param result 是否为候选词选择（影响输入逻辑）
     */
    void AppendOnKeyInputWindow(QString &str,bool result = false);

    /// 显示搜索结果（候选词列表）
    void ShowSearchResult(QStringList &result);

    /// 显示下一页候选词
    void ShowNextPageResult();

    /// 显示上一页候选词
    void ShowLastPageResult();

    /// 隐藏候选词区域
    void SetResultHidden();

    /// 设置数字键可用状态
    void SetEnableNUM(bool state);

    /// 设置字母键可用状态
    void SetEnableEN(bool state);

signals:
    /**
     * 编辑完成信号
     *
     * @param 最终确认的文本内容
     */
    void editisModifiedok(QString);

private slots:
    // ===================== 功能键槽函数 =====================
    void on_key_clear_clicked();  ///< 清空输入框
    void on_key_caps_clicked();   ///< 大小写切换
    void on_key_del_clicked();    ///< 删除字符
    void on_key_ok_clicked();     ///< 确认输入

    // ===================== 数字键槽函数 =====================
    void on_key_1_clicked();
    void on_key_2_clicked();
    void on_key_3_clicked();
    void on_key_4_clicked();
    void on_key_5_clicked();
    void on_key_6_clicked();
    void on_key_7_clicked();
    void on_key_8_clicked();
    void on_key_9_clicked();
    void on_key_0_clicked();

    // ===================== 字母键槽函数 =====================
    void on_key_q_clicked();
    void on_key_w_clicked();
    void on_key_e_clicked();
    void on_key_r_clicked();
    void on_key_t_clicked();
    void on_key_y_clicked();
    void on_key_u_clicked();
    void on_key_i_clicked();
    void on_key_o_clicked();
    void on_key_p_clicked();
    void on_key_a_clicked();
    void on_key_s_clicked();
    void on_key_d_clicked();
    void on_key_f_clicked();
    void on_key_g_clicked();
    void on_key_h_clicked();
    void on_key_j_clicked();
    void on_key_k_clicked();
    void on_key_l_clicked();
    void on_key_z_clicked();
    void on_key_x_clicked();
    void on_key_c_clicked();
    void on_key_v_clicked();
    void on_key_b_clicked();
    void on_key_n_clicked();
    void on_key_m_clicked();

    // ===================== 特殊功能槽函数 =====================
    void on_key_language_clicked();  ///< 中英文切换
    void on_key_comma_clicked();     ///< 逗号
    void on_key_dash_clicked();      ///< 破折号
    void on_key_virgule_clicked();   ///< 斜杠
    void on_key_colon_clicked();     ///< 冒号
    void on_key_at_clicked();        ///< @符号
    void on_key_point_clicked();     ///< 句点

    // ===================== 输入处理槽函数 =====================
    void on_key_pinyininput_textChanged(const QString &arg1);  ///< 拼音输入变化
    void on_NEXTpushButton_clicked();  ///< 下一页候选词
    void on_LASTpushButton_clicked();  ///< 上一页候选词

    // ===================== 候选词选择槽函数 =====================
    void slotCandidateClicked();  // 处理候选词点击

private:
    Ui::keyboard *ui;  ///< UI界面指针

    QVector<QPushButton*> candidateButtons; // 存储候选词按钮
};

#endif // KEYBOARD_H

/**********************************************************************************************/
