#include "KeyBoardDemo.h"
#include "ui_KeyBoardDemo.h"

#include <QTextEdit>

KeyBoardDemo::KeyBoardDemo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KeyBoardDemo)
{
    ui->setupUi(this);

    // 安装事件过滤器
    ui->textEdit->installEventFilter(this); // 安装到父窗口

    connect(this, &KeyBoardDemo::signalShowKeyBoard, this, &KeyBoardDemo::onShowKeyBoard);
}

KeyBoardDemo::~KeyBoardDemo()
{
    delete ui;
}

void KeyBoardDemo::onShowKeyBoard(QObject *watched)
{

}

bool KeyBoardDemo::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->textEdit && event->type() == QEvent::FocusIn)
    {
        // 发送信号，将编辑框对象给到虚拟键盘
        emit signalShowKeyBoard(watched);
    }

    return QWidget::eventFilter(watched, event); // 传递到基类
}

