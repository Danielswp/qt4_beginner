TEMPLATE = lib
TARGET = Math
CONFIG += shared dll
CONFIG += qtc_runnable

win32 {
    DEFINES += MATH_LIBRARY
}
HEADERS += normal.h
SOURCES += normal.cpp


# 在 Math.pro 中添加：
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/../bin/debug
else: DESTDIR = $$OUT_PWD/../bin/release
# 同时在 GUI.pro 中删除 QMAKE_POST_LINK 拷贝代码
