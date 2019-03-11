#-------------------------------------------------
#
# Project created by QtCreator 2019-03-10T10:20:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HJGraphics-Qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/local/include

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    BasicGLObject.cpp \
    Camera.cpp \
    DebugUtility.cpp \
    ElementObjects.cpp \
    Light.cpp \
    Material.cpp \
    Model.cpp \
    Scene.cpp \
    Shader.cpp \
    HJGWidget.cpp

HEADERS += \
        mainwindow.h \
    BasicGLObject.h \
    Camera.h \
    DebugUtility.h \
    ElementObjects.h \
    Light.h \
    Material.h \
    Model.h \
    Scene.h \
    Shader.h \
    stb_image.h \
    HJGWidget.h \
    OpenGLHeader.h

FORMS += \
        mainwindow.ui
LIBS += /usr/local/Cellar/assimp/4.0.1/lib/libassimp.4.0.1.dylib

