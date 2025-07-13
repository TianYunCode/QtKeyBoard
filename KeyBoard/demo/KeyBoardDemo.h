#ifndef KEYBOARDDEMO_H
#define KEYBOARDDEMO_H

#include <QWidget>

namespace Ui {
class KeyBoardDemo;
}

class KeyBoardDemo : public QWidget
{
    Q_OBJECT

public:
    explicit KeyBoardDemo(QWidget *parent = nullptr);
    ~KeyBoardDemo();

signals:
    void signalShowKeyBoard(QObject *watched);

private slots:
    void onShowKeyBoard(QObject *watched);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override; // 声明事件过滤器

private:
    Ui::KeyBoardDemo *ui;
};

#endif // KEYBOARDDEMO_H
