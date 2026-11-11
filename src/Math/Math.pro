TEMPLATE = lib
TARGET = Math
CONFIG += shared dll
CONFIG += qtc_runnable

win32 {
    DEFINES += MATH_LIBRARY
}


# ===== 输出目录 =====
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/../bin/debug
else: DESTDIR = $$OUT_PWD/../bin/release



# ===== 代码相关 =====
HEADERS += normal.h
SOURCES += normal.cpp

