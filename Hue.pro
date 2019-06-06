#-------------------------------------------------
#
# Project created by QtCreator 2018-02-23T17:20:01
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hue
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lighttable.cpp \
    huewrapper.cpp

HEADERS  += mainwindow.h \
    lighttable.h \
    huewrapper.h

FORMS    += mainwindow.ui
