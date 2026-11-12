QT += core gui opengl widgets sql network xml
win32: LIBS += -lopengl32 -lglu32

CONFIG += debug_and_release warn_on
TEMPLATE = app
TARGET = GUI

# ===== 调试 =====
CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS_RELEASE += -Zi
    QMAKE_LFLAGS_RELEASE += /DEBUG
}

# ===== 输出目录 =====
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/../bin/debug
else: DESTDIR = $$OUT_PWD/../bin/release


# ===== 头文件 =====
INCLUDEPATH += $$PWD/../Math
INCLUDEPATH += $$PWD/../../3rdparty

# ===== 链接 =====
CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../bin/debug -lMath
} else {
    LIBS += -L$$OUT_PWD/../bin/release -lMath
}


# ===== 代码相关 =====
SOURCES += main.cpp \
    glwidget.cpp \
    mainui.cpp

SOURCES += $$PWD/../../3rdparty/cJSON/cJSON.c

HEADERS += \
    glwidget.h \
    mainui.h


FORMS += \
    mainui.ui

RESOURCES += \
    resources/gui_resource.qrc




