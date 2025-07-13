#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class KeyBoard;
}

class KeyBoard : public QWidget
{
    Q_OBJECT

public:
    explicit KeyBoard(QWidget *parent = nullptr);
    ~KeyBoard();

    void showKeyBoard();

private:
    void init();
    void initUi();

private slots:
    void slotKeyBoardMinimizeBtnClicked();
    void slotKeyBoardMaximizeBtnClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;          // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event) override;           // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event) override;        // 鼠标释放事件

private:
    Ui::KeyBoard *ui;

    QPoint m_dragPosition;      // 拖动位置变量
    bool m_dragging = false;    // 拖动状态标志

    QSize m_keyBoard;
};

#endif // KEYBOARD_H
