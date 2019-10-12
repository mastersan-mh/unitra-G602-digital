#-------------------------------------------------
#
# Project created by QtCreator 2018-10-16T17:11:31
#
#-------------------------------------------------

QT       += core gui serialport

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
            settingsdialog.cpp \
            qcustomplot.cpp \
            console.cpp \
            cgraph.cpp \
            ctestmotor.cpp \
            pid/autopid.cpp \
            pid/pidadjustp.cpp \
            pid/fan.cpp \
            GClasses/GCommBase.cpp \
            comm.cpp \
            RPCClient.cpp \
            Device.cpp \
    DeviceViewModel.cpp \
    CSlidingWindow.cpp

HEADERS  += mainwindow.h \
            mainwindow_ui.h \
            defs.h \
            settingsdialog_ui.h \
            settingsdialog.h \
            qcustomplot.h \
            console.h \
            cgraph.h \
            ctestmotor.h \
            pid/pid.h \
            pid/autopid.hpp \
            pid/pidadjustp.h \
            pid/fan.h \
            GClasses/GCommBase.hpp \
            GClasses/GRPCDefs.hpp \
            comm.hpp \
            RPCClient.hpp \
            Device.hpp \
    DeviceViewModel.hpp \
    CSlidingWindow.hpp

FORMS    +=

RESOURCES += \
    res.qrc
