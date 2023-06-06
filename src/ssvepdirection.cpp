#include <QWindow>
#include <QDebug>
#include <QTime>
#include <QtMath>
#include <Qapplication>
#include <QDateTime>
#include <QDir>
#include <QPainter>
#include <QFont>
#include <array>
//
#include "ssvepdirection.h"
#include "ovtk_stimulations.h"
#include "utils.h"
#include "glutils.h"
//

SsvepDirection::SsvepDirection(SSVEP *paradigm, int t_port): SSVEPstimulation(paradigm, t_port)
{
    initElements();
}

void SsvepDirection::initElements()
{
    // init vectors
    m_vertexPerCircle = glUtils::SIDES_PER_CIRCLE + 2;
    int vectorsSize = (m_ssvep->nrElements() * m_vertexPerCircle) + m_ssvep->nrElements();
    m_vertices.resize(vectorsSize);
    //
    initCenters(); // circles centers points
    initCircles(); // vertices
    initColors();  // vertices' colors
    initIndices(); // vertices indices
    //
    scheduleRedraw();
}

void SsvepDirection::initCenters()
{
    int n_elements = m_ssvep->nrElements();
    QSize screenSize = utils::getScreenSize();
    float radiusx = glUtils::STIM_RADIUS / (screenSize.width() * glUtils::PIXEL_CM);
    float radiusy = glUtils::STIM_RADIUS / (screenSize.height() * glUtils::PIXEL_CM);
    int start = 0;

    m_centerPoints.resize(n_elements);

    for (int j = 0; j<=n_elements; ++j)
    {
        // circles center points
        m_centerPoints[j].setX(refPoints::dcenters[j].x());
        m_centerPoints[j].setY(refPoints::dcenters[j].y());
        m_centerPoints[j].setZ(refPoints::dcenters[j].z());
    }
}

void SsvepDirection::initCircles()
{
    float twicePi = 2.0f * M_PI;
    int start = 0;
    int stop = m_vertexPerCircle;
    int k=0;
    int elements = m_ssvep->nrElements();

    // qDebug()<< Q_FUNC_INFO << elements;

    QSize screenSize = utils::getScreenSize();
    float radiusx = glUtils::RADIUS_CM / (screenSize.width() * glUtils::PIXEL_CM);
    float radiusy = glUtils::RADIUS_CM / (screenSize.height() * glUtils::PIXEL_CM);
    float x, y, z, xx, yy;

    m_centers.resize(m_ssvep->nrElements());

    for (int j = start; j<=elements; ++j)
    {

        x = m_centerPoints[j].x();
        y = m_centerPoints[j].y();
        z = m_centerPoints[j].z();

        setVertex(k, x, y, z);

        // circles vertices
        for ( int i = k+1; i < stop; i++ )
        {
            xx = (x + (radiusx * cos(i * twicePi / glUtils::SIDES_PER_CIRCLE)));
            yy = (y + (radiusy * sin(i * twicePi /glUtils::SIDES_PER_CIRCLE)));
            setVertex(i, xx, yy, z);
        }
        k = stop;
        stop += m_vertexPerCircle;
    }

    k = m_vertices.count() - m_ssvep->nrElements();

    // center points vertices
    int i=start;
    for (int ind=k; ind<m_vertices.count();++ind)
    {
        setVertex(ind, m_centerPoints[i].x(), m_centerPoints[i].y(), m_centerPoints[i].z());
        ++i;
    }

}

void SsvepDirection::initColors()
{
    int vectorsSize = (m_ssvep->nrElements() * m_vertexPerCircle) + m_ssvep->nrElements();

    m_colors.resize(vectorsSize);

    for (int i=0; i<m_colors.count(); i++)
    {
        if (i < m_colors.count() - m_ssvep->nrElements())
        {
            m_colors[i] = glColors::white;
        }
        else
        {
            m_colors[i] = glColors::red;
        }
    }

}

void SsvepDirection::initIndices()
{
    // init indices
    int circleCount = m_ssvep->nrElements();

    m_vindices.resize(3*(m_vertexPerCircle*circleCount-(circleCount*2)));
    m_centerindices.resize(m_ssvep->nrElements());

    int circleIndices = 3*glUtils::SIDES_PER_CIRCLE;
    int k = 0;

    for (int i=0; i<m_vindices.count(); i+=3)
    {
        if((i%3)==0)
        {
            m_vindices[i] = m_vertexPerCircle * (i / circleIndices);
        }

        if( (i % circleIndices) == 0)
        {
            k = m_vindices[i];
        }

        m_vindices[i+1] = k+1;
        m_vindices[i+2] = k+2;
        k++;
    }

    int centerStart = m_vertices.count() - m_ssvep->nrElements();
    for (int i=0; i<m_centerindices.count(); ++i)
    {
        m_centerindices[i] = centerStart + i;
    }

}
