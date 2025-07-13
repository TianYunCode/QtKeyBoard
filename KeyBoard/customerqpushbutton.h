#ifndef CUSTOMERQPUSHBUTTON_H
#define CUSTOMERQPUSHBUTTON_H

#include <QPushButton>

/**
 * 自定义按钮控件（扩展QPushButton功能）
 *
 * 添加了带按钮指针的自定义点击信号，便于识别事件来源
 */
class customerqpushbutton : public QPushButton
{
    Q_OBJECT  // 启用Qt元对象系统

public:
    /// 空文本构造函数
    explicit customerqpushbutton(QWidget *parent = nullptr);

    /// 带文本构造函数
    explicit customerqpushbutton(const QString &text, QWidget *parent = nullptr);

    /// 带图标和文本构造函数
    customerqpushbutton(const QIcon& icon, const QString &text, QWidget *parent = nullptr);

    ~customerqpushbutton();

signals:
    /**
     * 自定义点击信号（携带按钮指针）
     *
     * @param pbtn 被点击的按钮指针
     *
     * @note 相比原生clicked()信号，可识别具体按钮实例
     */
    void clicked(customerqpushbutton *pbtn);

protected:
    /// 重写鼠标释放事件处理
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
};

#endif
