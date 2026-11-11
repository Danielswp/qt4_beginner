QT += core gui opengl widgets sql network xml
win32: LIBS += -lopengl32 -lglu32

CONFIG += debug_and_release warn_on
TEMPLATE = app
TARGET = GUI

# ===== 输出目录 =====
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/../bin/debug
else: DESTDIR = $$OUT_PWD/../bin/release


# ===== 头文件 =====
INCLUDEPATH += $$PWD/../Math


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

HEADERS += \
    glwidget.h \
    mainui.h

FORMS += \
    mainui.ui

RESOURCES += \
    resources/gui_resource.qrc




