#include "KeyBoardDemo.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KeyBoardDemo demo;
    demo.show();
    return a.exec();
}
