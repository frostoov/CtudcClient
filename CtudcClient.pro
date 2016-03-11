#-------------------------------------------------
#
# Project created by QtCreator 2014-11-23T23:39:22
#
#-------------------------------------------------

QT = core gui widgets printsupport network

TARGET    = CtudcClient
TEMPLATE  = app

QMAKE_CXXFLAGS += -std=c++14

LIBS += -lqcustomplot

HEADERS += \
    controllers/expocontroller.hpp \
    controllers/tdccontroller.hpp \
    controllers/voltagecontroller.hpp \
    net/ctudcconn.hpp \
    tdc/tdc.hpp \
    ui/mainwindow.hpp \
    ui/qconsole.hpp \
    ui/qcontrol.hpp \
    ui/qfrequency.hpp \
    ui/qloopfreq.hpp \
    ui/qsettings.hpp \
    ui/qstatus.hpp \
    utils.hpp \
    ui/qvoltage.hpp \
    net/request.hpp \
    net/response.hpp \
    configparser/appconfigparser.hpp \
    configparser/configparser.hpp

SOURCES += \
    controllers/expocontroller.cpp \
    controllers/tdccontroller.cpp \
    controllers/voltagecontroller.cpp \
    net/ctudcconn.cpp \
    ui/mainwindow.cpp \
    ui/qconsole.cpp \
    ui/qcontrol.cpp \
    ui/qfrequency.cpp \
    ui/qloopfreq.cpp \
    ui/qsettings.cpp \
    ui/qstatus.cpp \
    main.cpp \
    utils.cpp \
    ui/qvoltage.cpp \
    net/request.cpp \
    net/response.cpp \
    configparser/appconfigparser.cpp


