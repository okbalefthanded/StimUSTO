#-------------------------------------------------
#
# Project created by QtCreator 2016-09-04T03:46:54
#
#-------------------------------------------------

QT       += core gui network opengl

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LaresiSpellerPrototype
TEMPLATE = app


SOURCES += src\main.cpp\
        src\configpanel.cpp \
    src\mvepspeller.cpp \
    src\flashingspeller.cpp \
    src\matrixlayout.cpp \
    src\ovmarkersender.cpp \
    src\randomflashsequence.cpp \
    src\motionitem.cpp \
    src\movingface.cpp \
    src\ssvep.cpp \
    src\coloredface.cpp \
    src\hybridstimulation.cpp \
    src\ellipselayout.cpp \
    src\hybridgridstimulation.cpp \
    src\ssvepgl.cpp \
    src\utils.cpp \
    src\speller.cpp \
    src/test.cpp

HEADERS  += include\configpanel.h \
    include\mvepspeller.h \
    include\ovtk_stimulations.h \
    include\flashingspeller.h \
    include\matrixlayout.h \
    include\ovmarkersender.h \
    include\randomflashsequence.h \
    include\motionitem.h \
    include\movingface.h \
    include\ssvep.h \
    include\coloredface.h \
    include\hybridstimulation.h \
    include\ellipselayout.h \
    include\hybridgridstimulation.h \
    include\ssvepgl.h \
    include\utils.h \
    include\speller.h \
    include\test.h

INCLUDEPATH += include\

FORMS    += src\ui\configpanel.ui \
    src\ui\flashingspeller.ui \
    src\ui\movingface.ui \
    src\ui\mvepspeller.ui \
    src\ui\ssvep.ui \
    src\ui\coloredface.ui \
    src\ui\hybridstimulation.ui \
    src\ui\hybridgridstimulation.ui \
    src\ui\ssvepsingle.ui \
    src\ui\speller.ui

RESOURCES +=  assets\asset.qrc

DISTFILES += \
    MyOVBox.py \
    shaders/sh_v.vert \
    shaders/sh_f.frag \
    .gitignore
