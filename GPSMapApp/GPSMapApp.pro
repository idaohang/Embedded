#-------------------------------------------------
#
# Project created by QtCreator 2013-12-14T19:10:21
#
#-------------------------------------------------

QT       += core gui

TARGET = GPSMapApp
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    road.cpp \
    posNode.cpp \
    gpsreader.cpp \
    mapform.cpp

HEADERS  += widget.h \
    road.h \
    posNode.h \
    gpsreader.h \
    mapform.h

FORMS    += widget.ui \
    mapform.ui
