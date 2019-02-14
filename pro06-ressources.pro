#-------------------------------------------------
#
# Project created by QtCreator 2019-01-12T12:27:11
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++14

LIBS += -lGLU

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pro06-ressources
TEMPLATE = app


SOURCES += main.cpp\
        princ.cpp \
        glarea.cpp

HEADERS  += princ.h \
        glarea.h

FORMS    += princ.ui

RESOURCES += \
    pro06-ressources.qrc
