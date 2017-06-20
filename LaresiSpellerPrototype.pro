#-------------------------------------------------
#
# Project created by QtCreator 2016-09-04T03:46:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LaresiSpellerPrototype
TEMPLATE = app


SOURCES += main.cpp\
        configpanel.cpp \
    mvepspeller.cpp \
    flashingspeller.cpp \
    matrixlayout.cpp \
    ovmarkersender.cpp \
    randomflashsequence.cpp \
    motionitem.cpp \
    movingface.cpp \
    ssvep.cpp \
    coloredface.cpp \
    hybridstimulation.cpp \
    ellipselayout.cpp

HEADERS  += configpanel.h \
    mvepspeller.h \
   C:\openvibe-1.2.2-src\openvibe-1.2.2-src\toolkit\include\toolkit\ovtk_stimulations.h \
    flashingspeller.h \
    matrixlayout.h \
    ovmarkersender.h \
    randomflashsequence.h \
    motionitem.h \
    movingface.h \
    ssvep.h \
    coloredface.h \
    hybridstimulation.h \
    ellipselayout.h

INCLUDEPATH += C:\openvibe-1.2.2-src\openvibe-1.2.2-src\toolkit\include\toolkit

FORMS    += configpanel.ui \
    flashingspeller.ui \
    movingface.ui \
    ssvep.ui \
    coloredface.ui \
    hybridstimulation.ui

RESOURCES += \
    asset.qrc

DISTFILES += \
    MyOVBox.py
