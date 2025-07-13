/********************************************************************************
** Form generated from reading UI file 'keyboard.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYBOARD_H
#define UI_KEYBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_keyboard
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *keyboardgroupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *keyboardtitlelabel;
    QLineEdit *keyboardeditbox;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget;
    QGridLayout *gridLayout_candidateWords;
    QVBoxLayout *verticalLayout;
    QPushButton *NEXTpushButton;
    QPushButton *LASTpushButton;
    QGridLayout *gridLayout;
    QLineEdit *key_pinyininput;
    QPushButton *key_q;
    QPushButton *key_w;
    QPushButton *key_e;
    QPushButton *key_r;
    QPushButton *key_t;
    QPushButton *key_y;
    QPushButton *key_u;
    QPushButton *key_i;
    QPushButton *key_o;
    QPushButton *key_p;
    QPushButton *key_7;
    QPushButton *key_8;
    QPushButton *key_9;
    QPushButton *key_a;
    QPushButton *key_s;
    QPushButton *key_d;
    QPushButton *key_f;
    QPushButton *key_g;
    QPushButton *key_h;
    QPushButton *key_j;
    QPushButton *key_k;
    QPushButton *key_l;
    QPushButton *key_colon;
    QPushButton *key_4;
    QPushButton *key_5;
    QPushButton *key_6;
    QPushButton *key_z;
    QPushButton *key_x;
    QPushButton *key_c;
    QPushButton *key_v;
    QPushButton *key_b;
    QPushButton *key_n;
    QPushButton *key_m;
    QPushButton *key_comma;
    QPushButton *key_dash;
    QPushButton *key_virgule;
    QPushButton *key_1;
    QPushButton *key_2;
    QPushButton *key_3;
    QPushButton *key_caps;
    QPushButton *key_language;
    QPushButton *key_ok;
    QPushButton *key_clear;
    QPushButton *key_del;
    QPushButton *key_at;
    QPushButton *key_0;
    QPushButton *key_point;

    void setupUi(QMainWindow *keyboard)
    {
        if (keyboard->objectName().isEmpty())
            keyboard->setObjectName("keyboard");
        keyboard->resize(1114, 246);
        keyboard->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        centralwidget = new QWidget(keyboard);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setSpacing(1);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        keyboardgroupBox = new QGroupBox(centralwidget);
        keyboardgroupBox->setObjectName("keyboardgroupBox");
        keyboardgroupBox->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));
        horizontalLayout = new QHBoxLayout(keyboardgroupBox);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        keyboardtitlelabel = new QLabel(keyboardgroupBox);
        keyboardtitlelabel->setObjectName("keyboardtitlelabel");

        horizontalLayout->addWidget(keyboardtitlelabel);

        keyboardeditbox = new QLineEdit(keyboardgroupBox);
        keyboardeditbox->setObjectName("keyboardeditbox");

        horizontalLayout->addWidget(keyboardeditbox);

        horizontalLayout->setStretch(1, 1);

        verticalLayout_2->addWidget(keyboardgroupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(1);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");
        widget->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));
        gridLayout_candidateWords = new QGridLayout(widget);
        gridLayout_candidateWords->setSpacing(3);
        gridLayout_candidateWords->setObjectName("gridLayout_candidateWords");
        gridLayout_candidateWords->setContentsMargins(3, 3, 3, 3);

        horizontalLayout_2->addWidget(widget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        NEXTpushButton = new QPushButton(centralwidget);
        NEXTpushButton->setObjectName("NEXTpushButton");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(NEXTpushButton->sizePolicy().hasHeightForWidth());
        NEXTpushButton->setSizePolicy(sizePolicy);
        NEXTpushButton->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        verticalLayout->addWidget(NEXTpushButton);

        LASTpushButton = new QPushButton(centralwidget);
        LASTpushButton->setObjectName("LASTpushButton");
        sizePolicy.setHeightForWidth(LASTpushButton->sizePolicy().hasHeightForWidth());
        LASTpushButton->setSizePolicy(sizePolicy);
        LASTpushButton->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        verticalLayout->addWidget(LASTpushButton);


        horizontalLayout_2->addLayout(verticalLayout);

        horizontalLayout_2->setStretch(0, 1);

        verticalLayout_2->addLayout(horizontalLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        key_pinyininput = new QLineEdit(centralwidget);
        key_pinyininput->setObjectName("key_pinyininput");
        key_pinyininput->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_pinyininput, 0, 0, 1, 13);

        key_q = new QPushButton(centralwidget);
        key_q->setObjectName("key_q");
        key_q->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_q, 1, 0, 1, 1);

        key_w = new QPushButton(centralwidget);
        key_w->setObjectName("key_w");
        key_w->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_w, 1, 1, 1, 1);

        key_e = new QPushButton(centralwidget);
        key_e->setObjectName("key_e");
        key_e->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_e, 1, 2, 1, 1);

        key_r = new QPushButton(centralwidget);
        key_r->setObjectName("key_r");
        key_r->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_r, 1, 3, 1, 1);

        key_t = new QPushButton(centralwidget);
        key_t->setObjectName("key_t");
        key_t->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_t, 1, 4, 1, 1);

        key_y = new QPushButton(centralwidget);
        key_y->setObjectName("key_y");
        key_y->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_y, 1, 5, 1, 1);

        key_u = new QPushButton(centralwidget);
        key_u->setObjectName("key_u");
        key_u->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_u, 1, 6, 1, 1);

        key_i = new QPushButton(centralwidget);
        key_i->setObjectName("key_i");
        key_i->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_i, 1, 7, 1, 1);

        key_o = new QPushButton(centralwidget);
        key_o->setObjectName("key_o");
        key_o->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_o, 1, 8, 1, 1);

        key_p = new QPushButton(centralwidget);
        key_p->setObjectName("key_p");
        key_p->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_p, 1, 9, 1, 1);

        key_7 = new QPushButton(centralwidget);
        key_7->setObjectName("key_7");
        key_7->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_7, 1, 10, 1, 1);

        key_8 = new QPushButton(centralwidget);
        key_8->setObjectName("key_8");
        key_8->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_8, 1, 11, 1, 1);

        key_9 = new QPushButton(centralwidget);
        key_9->setObjectName("key_9");
        key_9->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_9, 1, 12, 1, 1);

        key_a = new QPushButton(centralwidget);
        key_a->setObjectName("key_a");
        key_a->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_a, 2, 0, 1, 1);

        key_s = new QPushButton(centralwidget);
        key_s->setObjectName("key_s");
        key_s->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_s, 2, 1, 1, 1);

        key_d = new QPushButton(centralwidget);
        key_d->setObjectName("key_d");
        key_d->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_d, 2, 2, 1, 1);

        key_f = new QPushButton(centralwidget);
        key_f->setObjectName("key_f");
        key_f->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_f, 2, 3, 1, 1);

        key_g = new QPushButton(centralwidget);
        key_g->setObjectName("key_g");
        key_g->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_g, 2, 4, 1, 1);

        key_h = new QPushButton(centralwidget);
        key_h->setObjectName("key_h");
        key_h->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_h, 2, 5, 1, 1);

        key_j = new QPushButton(centralwidget);
        key_j->setObjectName("key_j");
        key_j->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_j, 2, 6, 1, 1);

        key_k = new QPushButton(centralwidget);
        key_k->setObjectName("key_k");
        key_k->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_k, 2, 7, 1, 1);

        key_l = new QPushButton(centralwidget);
        key_l->setObjectName("key_l");
        key_l->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_l, 2, 8, 1, 1);

        key_colon = new QPushButton(centralwidget);
        key_colon->setObjectName("key_colon");
        key_colon->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_colon, 2, 9, 1, 1);

        key_4 = new QPushButton(centralwidget);
        key_4->setObjectName("key_4");
        key_4->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_4, 2, 10, 1, 1);

        key_5 = new QPushButton(centralwidget);
        key_5->setObjectName("key_5");
        key_5->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_5, 2, 11, 1, 1);

        key_6 = new QPushButton(centralwidget);
        key_6->setObjectName("key_6");
        key_6->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_6, 2, 12, 1, 1);

        key_z = new QPushButton(centralwidget);
        key_z->setObjectName("key_z");
        key_z->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_z, 3, 0, 1, 1);

        key_x = new QPushButton(centralwidget);
        key_x->setObjectName("key_x");
        key_x->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_x, 3, 1, 1, 1);

        key_c = new QPushButton(centralwidget);
        key_c->setObjectName("key_c");
        key_c->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_c, 3, 2, 1, 1);

        key_v = new QPushButton(centralwidget);
        key_v->setObjectName("key_v");
        key_v->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_v, 3, 3, 1, 1);

        key_b = new QPushButton(centralwidget);
        key_b->setObjectName("key_b");
        key_b->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_b, 3, 4, 1, 1);

        key_n = new QPushButton(centralwidget);
        key_n->setObjectName("key_n");
        key_n->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_n, 3, 5, 1, 1);

        key_m = new QPushButton(centralwidget);
        key_m->setObjectName("key_m");
        key_m->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_m, 3, 6, 1, 1);

        key_comma = new QPushButton(centralwidget);
        key_comma->setObjectName("key_comma");
        key_comma->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_comma, 3, 7, 1, 1);

        key_dash = new QPushButton(centralwidget);
        key_dash->setObjectName("key_dash");
        key_dash->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_dash, 3, 8, 1, 1);

        key_virgule = new QPushButton(centralwidget);
        key_virgule->setObjectName("key_virgule");
        key_virgule->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_virgule, 3, 9, 1, 1);

        key_1 = new QPushButton(centralwidget);
        key_1->setObjectName("key_1");
        key_1->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_1, 3, 10, 1, 1);

        key_2 = new QPushButton(centralwidget);
        key_2->setObjectName("key_2");
        key_2->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_2, 3, 11, 1, 1);

        key_3 = new QPushButton(centralwidget);
        key_3->setObjectName("key_3");
        key_3->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_3, 3, 12, 1, 1);

        key_caps = new QPushButton(centralwidget);
        key_caps->setObjectName("key_caps");
        key_caps->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_caps, 4, 0, 1, 1);

        key_language = new QPushButton(centralwidget);
        key_language->setObjectName("key_language");
        key_language->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_language, 4, 1, 1, 1);

        key_ok = new QPushButton(centralwidget);
        key_ok->setObjectName("key_ok");
        key_ok->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_ok, 4, 2, 1, 6);

        key_clear = new QPushButton(centralwidget);
        key_clear->setObjectName("key_clear");
        key_clear->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_clear, 4, 8, 1, 1);

        key_del = new QPushButton(centralwidget);
        key_del->setObjectName("key_del");
        key_del->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_del, 4, 9, 1, 1);

        key_at = new QPushButton(centralwidget);
        key_at->setObjectName("key_at");
        key_at->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_at, 4, 10, 1, 1);

        key_0 = new QPushButton(centralwidget);
        key_0->setObjectName("key_0");
        key_0->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_0, 4, 11, 1, 1);

        key_point = new QPushButton(centralwidget);
        key_point->setObjectName("key_point");
        key_point->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(64, 65, 66);"));

        gridLayout->addWidget(key_point, 4, 12, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        verticalLayout_2->setStretch(2, 1);
        keyboard->setCentralWidget(centralwidget);

        retranslateUi(keyboard);

        QMetaObject::connectSlotsByName(keyboard);
    } // setupUi

    void retranslateUi(QMainWindow *keyboard)
    {
        keyboard->setWindowTitle(QCoreApplication::translate("keyboard", "keyboard", nullptr));
        keyboardgroupBox->setTitle(QCoreApplication::translate("keyboard", "\346\226\207\346\234\254\347\274\226\350\276\221", nullptr));
        keyboardtitlelabel->setText(QCoreApplication::translate("keyboard", "<html><head/><body><p align=\"center\">\350\257\267\350\276\223\345\205\245\345\206\205\345\256\271\357\274\232</p></body></html>", nullptr));
        keyboardeditbox->setText(QString());
        NEXTpushButton->setText(QCoreApplication::translate("keyboard", "=>", nullptr));
        LASTpushButton->setText(QCoreApplication::translate("keyboard", "<=", nullptr));
        key_pinyininput->setText(QString());
        key_q->setText(QCoreApplication::translate("keyboard", "q", nullptr));
        key_w->setText(QCoreApplication::translate("keyboard", "w", nullptr));
        key_e->setText(QCoreApplication::translate("keyboard", "e", nullptr));
        key_r->setText(QCoreApplication::translate("keyboard", "r", nullptr));
        key_t->setText(QCoreApplication::translate("keyboard", "t", nullptr));
        key_y->setText(QCoreApplication::translate("keyboard", "y", nullptr));
        key_u->setText(QCoreApplication::translate("keyboard", "u", nullptr));
        key_i->setText(QCoreApplication::translate("keyboard", "i", nullptr));
        key_o->setText(QCoreApplication::translate("keyboard", "o", nullptr));
        key_p->setText(QCoreApplication::translate("keyboard", "p", nullptr));
        key_7->setText(QCoreApplication::translate("keyboard", "7", nullptr));
        key_8->setText(QCoreApplication::translate("keyboard", "8", nullptr));
        key_9->setText(QCoreApplication::translate("keyboard", "9", nullptr));
        key_a->setText(QCoreApplication::translate("keyboard", "a", nullptr));
        key_s->setText(QCoreApplication::translate("keyboard", "s", nullptr));
        key_d->setText(QCoreApplication::translate("keyboard", "d", nullptr));
        key_f->setText(QCoreApplication::translate("keyboard", "f", nullptr));
        key_g->setText(QCoreApplication::translate("keyboard", "g", nullptr));
        key_h->setText(QCoreApplication::translate("keyboard", "h", nullptr));
        key_j->setText(QCoreApplication::translate("keyboard", "j", nullptr));
        key_k->setText(QCoreApplication::translate("keyboard", "k", nullptr));
        key_l->setText(QCoreApplication::translate("keyboard", "l", nullptr));
        key_colon->setText(QCoreApplication::translate("keyboard", ":", nullptr));
        key_4->setText(QCoreApplication::translate("keyboard", "4", nullptr));
        key_5->setText(QCoreApplication::translate("keyboard", "5", nullptr));
        key_6->setText(QCoreApplication::translate("keyboard", "6", nullptr));
        key_z->setText(QCoreApplication::translate("keyboard", "z", nullptr));
        key_x->setText(QCoreApplication::translate("keyboard", "x", nullptr));
        key_c->setText(QCoreApplication::translate("keyboard", "c", nullptr));
        key_v->setText(QCoreApplication::translate("keyboard", "v", nullptr));
        key_b->setText(QCoreApplication::translate("keyboard", "b", nullptr));
        key_n->setText(QCoreApplication::translate("keyboard", "n", nullptr));
        key_m->setText(QCoreApplication::translate("keyboard", "m", nullptr));
        key_comma->setText(QCoreApplication::translate("keyboard", ",", nullptr));
        key_dash->setText(QCoreApplication::translate("keyboard", "-", nullptr));
        key_virgule->setText(QCoreApplication::translate("keyboard", "/", nullptr));
        key_1->setText(QCoreApplication::translate("keyboard", "1", nullptr));
        key_2->setText(QCoreApplication::translate("keyboard", "2", nullptr));
        key_3->setText(QCoreApplication::translate("keyboard", "3", nullptr));
        key_caps->setText(QCoreApplication::translate("keyboard", "caps", nullptr));
        key_language->setText(QCoreApplication::translate("keyboard", "en/zh", nullptr));
        key_ok->setText(QCoreApplication::translate("keyboard", "ok", nullptr));
        key_clear->setText(QCoreApplication::translate("keyboard", "clear", nullptr));
        key_del->setText(QCoreApplication::translate("keyboard", "del", nullptr));
        key_at->setText(QCoreApplication::translate("keyboard", "@", nullptr));
        key_0->setText(QCoreApplication::translate("keyboard", "0", nullptr));
        key_point->setText(QCoreApplication::translate("keyboard", ".", nullptr));
    } // retranslateUi

};

namespace Ui {
    class keyboard: public Ui_keyboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYBOARD_H
