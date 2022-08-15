#-------------------------------------------------
#
# Project created by QtCreator 2018-01-01T20:08:03
#
#-------------------------------------------------

QT       += core gui network xml serialport gamepad websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ButtonMash
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


win32 {
    LIBS += -ldinput8 -ldxguid
    CONFIG += c++11
    SOURCES +=  QGameController/src/gamecontroller/qgamecontroller.cpp
    SOURCES +=  QGameController/src/gamecontroller/qgamecontroller_win.cpp
    SOURCES +=  directinputsource.cpp
    HEADERS +=  directinputsource.h
    QGC_PUBLIC_HEADERS += QGameController/src/gamecontroller/qgamecontroller.h
    QGC_PRIVATE_HEADERS +=  QGameController/src/gamecontroller/qgamecontroller_p.h
    HEADERS += $$QGC_PUBLIC_HEADERS $$QGC_PRIVATE_HEADERS
}

SOURCES += \
        main.cpp \
        skinselector.cpp \
        inputdecoder.cpp \
        telnetconnection.cpp \
        inputdisplay.cpp \
        skinparser.cpp \
        arduinocom.cpp \
        inputprovider.cpp \
        inputsourceselector.cpp \
        qgamepadsource.cpp \
        mapbuttondialog.cpp \
        usb2snessource.cpp \
        usb2snes.cpp \
        snesclassictelnet.cpp \
        configurationdialog.cpp \
        skineditor.cpp \
    localcontroller.cpp \
    localcontrollermanager.cpp

HEADERS += \
        skinselector.h \
        inputdecoder.h \
        telnetconnection.h \
        inputdisplay.h \
        skinparser.h \
        arduinocom.h \
        inputprovider.h \
        inputsourceselector.h \
        qgamepadsource.h \
        mapbuttondialog.h \
        usb2snessource.h \
        usb2snes.h \
        snesclassictelnet.h \
        configurationdialog.h \
        skineditor.h \
        localcontroller.h \
        localcontrollermanager.h

FORMS += \
        skinselector.ui \
        inputdisplay.ui \
        inputsourceselector.ui \
        mapbuttondialog.ui \
        configurationdialog.ui \
        skineditor.ui

RC_FILE += \
    buttonmash.rc

macx: {
	QMAKE_INFO_PLIST = Info.plist
	ICON = Icon128x128.icns
}

RESOURCES += \
    ressources.qrc
