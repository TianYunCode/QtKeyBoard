#include "customerqpushbutton.h"

/**
 * 空文本构造函数
 *
 * @param parent 父窗口指针
 */
customerqpushbutton::customerqpushbutton(QWidget *parent)
    : QPushButton(parent)  // 调用基类构造函数
{
    // 空构造，使用默认设置
}

/**
 * 带文本构造函数
 *
 * @param text 按钮显示文本
 * @param parent 父窗口指针
 */
customerqpushbutton::customerqpushbutton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)  // 调用基类文本构造函数
{
    // 初始化带文本按钮
}

/**
 * 带图标和文本构造函数
 *
 * @param icon 按钮图标
 * @param text 按钮显示文本
 * @param parent 父窗口指针
 */
customerqpushbutton::customerqpushbutton(const QIcon& icon, const QString &text, QWidget *parent)
    : QPushButton(icon, text, parent)  // 调用基类图标+文本构造函数
{
    // 初始化带图标和文本按钮
}

/// 析构函数
customerqpushbutton::~customerqpushbutton()
{
    // 无特殊资源需要释放
}

/**
 * 重写鼠标释放事件处理
 *
 * @param ev 鼠标事件对象
 *
 * @note 当鼠标在按钮上释放时，触发自定义clicked信号
 *       携带当前按钮指针作为参数，便于识别事件源
 */
void customerqpushbutton::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)  // 标记未使用参数，避免编译器警告

    // 发射携带自身指针的点击信号
    emit clicked(this);

    // 可选的：调用基类实现确保标准行为
    // QPushButton::mouseReleaseEvent(ev);
}
