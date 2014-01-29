#-------------------------------------------------
#
# Project created by QtCreator 2014-01-29T19:16:41
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = PortAudioTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lportaudio
