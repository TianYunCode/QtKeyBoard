INCLUDEPATH     += $$PWD/
DEPENDPATH      += $$PWD/

HEADERS += \
    $$PWD/api/atomdictbase.h \
    $$PWD/api/dictbuilder.h \
    $$PWD/api/dictdef.h \
    $$PWD/api/dictlist.h \
    $$PWD/api/dicttrie.h \
    $$PWD/api/lpicache.h \
    $$PWD/api/matrixsearch.h \
    $$PWD/api/mystdlib.h \
    $$PWD/api/ngram.h \
    $$PWD/api/pinyinime.h \
    $$PWD/api/pinyinime_global.h \
    $$PWD/api/searchutility.h \
    $$PWD/api/spellingtable.h \
    $$PWD/api/spellingtrie.h \
    $$PWD/api/splparser.h \
    $$PWD/api/sync.h \
    $$PWD/api/userdict.h \
    $$PWD/api/utf16char.h \
    $$PWD/api/utf16reader.h \
    $$PWD/src/KeyBoard.h
    

SOURCES += \
    $$PWD/api/dictbuilder.cpp \
    $$PWD/api/dictlist.cpp \
    $$PWD/api/dicttrie.cpp \
    $$PWD/api/lpicache.cpp \
    $$PWD/api/matrixsearch.cpp \
    $$PWD/api/mystdlib.cpp \
    $$PWD/api/ngram.cpp \
    $$PWD/api/pinyinime.cpp \
    $$PWD/api/searchutility.cpp \
    $$PWD/api/spellingtable.cpp \
    $$PWD/api/spellingtrie.cpp \
    $$PWD/api/splparser.cpp \
    $$PWD/api/sync.cpp \
    $$PWD/api/userdict.cpp \
    $$PWD/api/utf16char.cpp \
    $$PWD/api/utf16reader.cpp \
    $$PWD/src/KeyBoard.cpp

FORMS += \
    $$PWD/src/KeyBoard.ui

RESOURCES += \
    $$PWD/res/res.qrc
