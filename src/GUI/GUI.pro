QT += core gui opengl widgets sql network xml
win32: LIBS += -lopengl32 -lglu32

CONFIG += debug_and_release warn_on
TEMPLATE = app
TARGET = GUI

SOURCES += main.cpp \
    mainui.cpp

HEADERS += \
    glwidget.h \
    mainui.h

SOURCES += \
    glwidget.cpp

INCLUDEPATH += $$PWD/../Math


CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/../bin/debug
else: DESTDIR = $$OUT_PWD/../bin/release

# ===== 链接 Math 库 =====

CONFIG(debug, debug|release) {
    # Debug 模式：指向 debug 子目录
    LIBS += -L$$OUT_PWD/../bin/debug -lMath
} else {
    # Release 模式
    LIBS += -L$$OUT_PWD/../bin/release -lMath
}





RESOURCES += \
    resources/gui_resource.qrc

FORMS += \
    mainui.ui


