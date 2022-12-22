#-------------------------------------------------
#
# Project created by QtCreator 2016-09-04T03:46:54
#
#-------------------------------------------------

QT       += core gui network opengl multimedia

CONFIG += c++11 console
# CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StimUSTO
TEMPLATE = app


SOURCES += src\main.cpp\
    src/arabicspeller.cpp \
    src/auditoryspeller.cpp \
    src/chromaspeller.cpp \
    src/externComm.cpp \
    src/hybrid.cpp \
    src/hybridstimulation.cpp \
    src/multistimuli.cpp \
    src/phonekeypad.cpp \
    src/spellersmall.cpp \
    src/ssvepcircle.cpp \
    src/ssvepdirection.cpp \    
    src/ssvepstimulation.cpp \
    src\configpanel.cpp \
    src\matrixlayout.cpp \
    src\ovmarkersender.cpp \
    src\randomflashsequence.cpp \
    src\ellipselayout.cpp \
    src\ssvepgl.cpp \
    src\utils.cpp \
    src\speller.cpp \
    src/flashingspeller.cpp \
    src/facespeller.cpp \
    src/paradigm.cpp \
    src/erp.cpp \
    src/jsonserializer.cpp \
    src/ssvep.cpp \
    src/logger.cpp

HEADERS  += include\configpanel.h \
    include/arabicspeller.h \
    include/auditoryspeller.h \
    include/chromaspeller.h \
    include/externComm.h \
    include/hybrid.h \
    include/hybridstimulation.h \
    include/multistimuli.h \
    include/phonekeypad.h \
    include/spellersmall.h \
    include/ssvepcircle.h \
    include/ssvepdirection.h \    
    include/ssvepstimulation.h \
    include\ovtk_stimulations.h \
    include\matrixlayout.h \
    include\ovmarkersender.h \
    include\randomflashsequence.h \
    include\ellipselayout.h \
    include\ssvepgl.h \
    include\utils.h \
    include\speller.h \
    include\glutils.h \
    include/flashingspeller.h \
    include/facespeller.h \
    include/paradigm.h \
    include/erp.h \
    include/serializable.h \
    include/serializer.h \
    include/jsonserializer.h \
    include/ssvep.h \
    include/logger.h

INCLUDEPATH += include\

FORMS    += src\ui\configpanel.ui \
            src\ui\ssvep.ui \
            src\ui\ssvepsingle.ui \
            src\ui\spellerform.ui

RESOURCES +=  assets\asset.qrc

DISTFILES += \
    MyOVBox.py \
    src\shaders\sh_v.vert \
    src\shaders\sh_f.frag \
    config/default.json \
    config/facespeller_calib.json \
    config/facespeller_online.json \
    README.md \
    src/pyscripts/MyOVBox.py \
    config/ssvep_online_sync.json \
    config/ssvep_calib_sync.json
