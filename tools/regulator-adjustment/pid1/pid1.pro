#-------------------------------------------------
#
# Project created by QtCreator 2018-10-16T17:11:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets \
 printsupport

TARGET = pid1
TEMPLATE = app

QMAKE_CFLAGS += -Wno-expansion-to-defined
QMAKE_CXXFLAGS += -Wno-expansion-to-defined

#DEFINES += PID_DISCRETE
#DEFINES += PID_RECURRENT
DEFINES += PID_RECURRENT_FIXED32

INCLUDEPATH += ../../../src/c/include/

SOURCES  += main.cpp\
            mainwindow.cpp \
            qcustomplot.cpp \
            cgraph.cpp \
    pid/autopid.cpp \
    ctestmotor.cpp

HEADERS  += mainwindow.h \
            qcustomplot.h \
            cgraph.h \
            pid/pid.h \
    pid/autopid.hpp \
    ctestmotor.h

FORMS    +=
