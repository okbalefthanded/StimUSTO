#ifndef SSVEPDIRECTION_H
#define SSVEPDIRECTION_H

//
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QUdpSocket>
#include <QTimer>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QDebug>
#include <QTcpSocket>
//
#include "ssvepstimulation.h"
#include "randomflashsequence.h"
#include "ssvep.h"
#include "logger.h"
#include "glutils.h"
//
class SsvepDirection : public SSVEPstimulation
{
    Q_OBJECT

public:
    SsvepDirection(SSVEP *paradigm, int t_port);

private slots:

    void initElements();
    void initCenters();
    void initCircles();
    void initColors();
    void initIndices();

};



#endif // SSVEPDIRECTION_H
