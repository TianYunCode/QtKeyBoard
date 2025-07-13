#include "KeyBoard.h"
#include "ui_KeyBoard.h"

KeyBoard::KeyBoard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KeyBoard)
{
    ui->setupUi(this);
    init();
}

KeyBoard::~KeyBoard()
{
    delete ui;
}

void KeyBoard::showKeyBoard()
{
    this->setFixedSize(m_keyBoard);

    this->show();

    ui->keyBoardBodyWidget->show();

    ui->keyBoardMaximizeBtn->show();

    ui->keyBoardMinimizeBtn->show();

    ui->keyBoardNameLabel->show();
}

void KeyBoard::init()
{
    initUi();

    this->setWindowFlag(Qt::FramelessWindowHint);                           // 设置窗口无边框
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);   // 设置窗口置顶

    connect(ui->keyBoardMinimizeBtn, &QToolButton::clicked, this, &KeyBoard::slotKeyBoardMinimizeBtnClicked);
    connect(ui->keyBoardMaximizeBtn, &QToolButton::clicked, this, &KeyBoard::slotKeyBoardMaximizeBtnClicked);

    m_keyBoard = this->size();
}

void KeyBoard::initUi()
{
    // 加载图片并缩放到指定大小
    QPixmap pixmap(":/image/keyboard_icon.svg");
    QPixmap scaledPixmap = pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->keyBoardIconLabel->setPixmap(scaledPixmap);

    // 创建字体对象并设置大小
    QFont font;
    font.setPointSize(14);  // 使用点为单位设置字体大小
    ui->keyBoardNameLabel->setFont(font);
    ui->keyBoardNameLabel->setText("虚拟键盘");

    ui->keyBoardMinimizeBtn->setIcon(QIcon(":/image/minimize_icon.svg"));
    ui->keyBoardMinimizeBtn->setIconSize(QSize(32,32));

    ui->keyBoardMaximizeBtn->setIcon(QIcon(":/image/maximize_icon.svg"));
    ui->keyBoardMaximizeBtn->setIconSize(QSize(32,32));
}

void KeyBoard::slotKeyBoardMinimizeBtnClicked()
{
    m_keyBoard = this->size();

    QSize iconsSize = ui->keyBoardIconLabel->size();

    ui->keyBoardBodyWidget->hide();

    ui->keyBoardMaximizeBtn->hide();

    ui->keyBoardMinimizeBtn->hide();

    ui->keyBoardNameLabel->hide();

    this->setFixedSize(iconsSize);
}

void KeyBoard::slotKeyBoardMaximizeBtnClicked()
{
    this->hide();
}

void KeyBoard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = true;

        // 记录鼠标按下时的全局位置与窗口位置的差值
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }

    QWidget::mousePressEvent(event); // 确保基类事件处理
}

void KeyBoard::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton))
    {
        // 计算新窗口位置: 当前鼠标全局位置 - 初始差值
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }

    QWidget::mouseMoveEvent(event); // 确保基类事件处理
}

void KeyBoard::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false; // 停止拖动
        event->accept();
    }

    QWidget::mouseReleaseEvent(event); // 确保基类事件处理
}
