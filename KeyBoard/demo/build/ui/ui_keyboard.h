/********************************************************************************
** Form generated from reading UI file 'KeyBoard.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYBOARD_H
#define UI_KEYBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_KeyBoard
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *keyBoardTitleWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *keyBoardIconLabel;
    QLabel *keyBoardNameLabel;
    QSpacerItem *horizontalSpacer;
    QToolButton *keyBoardMinimizeBtn;
    QToolButton *keyBoardMaximizeBtn;
    QWidget *keyBoardBodyWidget;

    void setupUi(QWidget *KeyBoard)
    {
        if (KeyBoard->objectName().isEmpty())
            KeyBoard->setObjectName("KeyBoard");
        KeyBoard->resize(926, 465);
        verticalLayout = new QVBoxLayout(KeyBoard);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        keyBoardTitleWidget = new QWidget(KeyBoard);
        keyBoardTitleWidget->setObjectName("keyBoardTitleWidget");
        horizontalLayout = new QHBoxLayout(keyBoardTitleWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        keyBoardIconLabel = new QLabel(keyBoardTitleWidget);
        keyBoardIconLabel->setObjectName("keyBoardIconLabel");
        keyBoardIconLabel->setScaledContents(true);

        horizontalLayout->addWidget(keyBoardIconLabel);

        keyBoardNameLabel = new QLabel(keyBoardTitleWidget);
        keyBoardNameLabel->setObjectName("keyBoardNameLabel");

        horizontalLayout->addWidget(keyBoardNameLabel);

        horizontalSpacer = new QSpacerItem(827, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        keyBoardMinimizeBtn = new QToolButton(keyBoardTitleWidget);
        keyBoardMinimizeBtn->setObjectName("keyBoardMinimizeBtn");

        horizontalLayout->addWidget(keyBoardMinimizeBtn);

        keyBoardMaximizeBtn = new QToolButton(keyBoardTitleWidget);
        keyBoardMaximizeBtn->setObjectName("keyBoardMaximizeBtn");

        horizontalLayout->addWidget(keyBoardMaximizeBtn);

        horizontalLayout->setStretch(2, 1);

        verticalLayout->addWidget(keyBoardTitleWidget);

        keyBoardBodyWidget = new QWidget(KeyBoard);
        keyBoardBodyWidget->setObjectName("keyBoardBodyWidget");

        verticalLayout->addWidget(keyBoardBodyWidget);

        verticalLayout->setStretch(1, 1);

        retranslateUi(KeyBoard);

        QMetaObject::connectSlotsByName(KeyBoard);
    } // setupUi

    void retranslateUi(QWidget *KeyBoard)
    {
        KeyBoard->setWindowTitle(QCoreApplication::translate("KeyBoard", "Form", nullptr));
        keyBoardIconLabel->setText(QString());
        keyBoardNameLabel->setText(QString());
        keyBoardMinimizeBtn->setText(QString());
        keyBoardMaximizeBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class KeyBoard: public Ui_KeyBoard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYBOARD_H
