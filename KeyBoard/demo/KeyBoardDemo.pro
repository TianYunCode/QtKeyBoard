QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

include($$PWD/../KeyBoard.pri)

SOURCES += \
    main.cpp \
    KeyBoardDemo.cpp

HEADERS += \
    KeyBoardDemo.h

FORMS += \
    KeyBoardDemo.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DESTDIR         = $$PWD/build/app     #可执行文件存放路径
OBJECTS_DIR     = $$PWD/build/obj     #中间文件存放路径（.o 文件）
MOC_DIR         = $$PWD/build/moc     #元对象文件存放路径（moc文件）
UI_DIR          = $$PWD/build/ui      #UI文件生成的头文件存放路径
RCC_DIR         = $$PWD/build/rcc     #资源文件生成的头文件存放路径
