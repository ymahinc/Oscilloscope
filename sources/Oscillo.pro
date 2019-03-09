#-------------------------------------------------
#
# Project created by QtCreator 2018-06-22T18:05:05
#
#-------------------------------------------------

QT       += core gui serialport charts qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Oscillo
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    settingsdlg.cpp \
    oscillochart.cpp \
    mycolorbutton.cpp \
    oscilloview.cpp \
    portthread.cpp \
    yresdial.cpp \
    xresdial.cpp \
    mydock.cpp \
    mydial.cpp \
    fft.cpp \
    frequencyspectrum.cpp \
    myslider.cpp \
    cursor.cpp

HEADERS += \
        mainwindow.h \
    settingsdlg.h \
    global.h \
    oscillochart.h \
    mycolorbutton.h \
    oscilloview.h \
    portthread.h \
    yresdial.h \
    xresdial.h \
    mydock.h \
    mydial.h \
    fft.h \
    frequencyspectrum.h \
    myslider.h \
    cursor.h

FORMS += \
        mainwindow.ui \
    settingsdlg.ui \
    yresdial.ui \
    xresdial.ui

RESOURCES += \
    oscillo.qrc

DISTFILES += \
    todo

RC_ICONS = sine.ico
