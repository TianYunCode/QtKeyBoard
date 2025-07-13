/********************************************************************************
** Form generated from reading UI file 'KeyBoardDemo.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYBOARDDEMO_H
#define UI_KEYBOARDDEMO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_KeyBoardDemo
{
public:
    QGridLayout *gridLayout;
    QTextEdit *textEdit;
    QTextEdit *textEdit_2;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QWidget *KeyBoardDemo)
    {
        if (KeyBoardDemo->objectName().isEmpty())
            KeyBoardDemo->setObjectName("KeyBoardDemo");
        KeyBoardDemo->resize(764, 352);
        gridLayout = new QGridLayout(KeyBoardDemo);
        gridLayout->setObjectName("gridLayout");
        textEdit = new QTextEdit(KeyBoardDemo);
        textEdit->setObjectName("textEdit");

        gridLayout->addWidget(textEdit, 0, 0, 1, 1);

        textEdit_2 = new QTextEdit(KeyBoardDemo);
        textEdit_2->setObjectName("textEdit_2");

        gridLayout->addWidget(textEdit_2, 0, 1, 1, 1);

        lineEdit = new QLineEdit(KeyBoardDemo);
        lineEdit->setObjectName("lineEdit");

        gridLayout->addWidget(lineEdit, 1, 0, 1, 1);

        lineEdit_2 = new QLineEdit(KeyBoardDemo);
        lineEdit_2->setObjectName("lineEdit_2");

        gridLayout->addWidget(lineEdit_2, 1, 1, 1, 1);

        pushButton = new QPushButton(KeyBoardDemo);
        pushButton->setObjectName("pushButton");

        gridLayout->addWidget(pushButton, 2, 0, 1, 1);

        pushButton_2 = new QPushButton(KeyBoardDemo);
        pushButton_2->setObjectName("pushButton_2");

        gridLayout->addWidget(pushButton_2, 2, 1, 1, 1);


        retranslateUi(KeyBoardDemo);

        QMetaObject::connectSlotsByName(KeyBoardDemo);
    } // setupUi

    void retranslateUi(QWidget *KeyBoardDemo)
    {
        KeyBoardDemo->setWindowTitle(QCoreApplication::translate("KeyBoardDemo", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("KeyBoardDemo", "PushButton", nullptr));
        pushButton_2->setText(QCoreApplication::translate("KeyBoardDemo", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class KeyBoardDemo: public Ui_KeyBoardDemo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYBOARDDEMO_H
