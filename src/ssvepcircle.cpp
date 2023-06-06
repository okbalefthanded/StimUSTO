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
#include "ssvepcircle.h"
#include "ovtk_stimulations.h"
#include "utils.h"
#include "glutils.h"
//

SsvepCircle::SsvepCircle(SSVEP *paradigm, int t_port): SSVEPstimulation(paradigm, t_port)
{
    initElements();
}

void SsvepCircle::initElements()
{
    // init vectors
    // int m_vertexPerCircle = glUtils::SIDES_PER_CIRCLE + 2;
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

void SsvepCircle::initCenters()
{
    int n_elements = m_ssvep->nrElements();
    QSize screenSize = utils::getScreenSize();
    float radiusx = glUtils::STIM_RADIUS / (screenSize.width() * glUtils::PIXEL_CM);
    float radiusy = glUtils::STIM_RADIUS / (screenSize.height() * glUtils::PIXEL_CM);
    int start = 0;

    m_centerPoints.resize(n_elements);

    // circular layout, experimental
    /*
    if(m_ssvep->controlMode() == control_mode::ASYNC)
    {
        m_centerPoints[0] = QVector3D(0.0f, 0.0f, 1.0f);
        --n_elements;
        start = 1;
    }


    for(int i=start; i<n_elements; ++i)
    {
         m_centerPoints[i].setX(radiusx * cos(i));
         m_centerPoints[i].setY(radiusy * sin(i));
         m_centerPoints[i].setZ(1.0f);
    }

    */
    for (int j = 0; j<=n_elements; ++j)
    {

        if(m_ssvep->stimulationType() == speller_type::SSVEP_CIRCLE)
        {
            // circles center points
            m_centerPoints[j].setX(refPoints::centers[j].x());
            m_centerPoints[j].setY(refPoints::centers[j].y());
            m_centerPoints[j].setZ(refPoints::centers[j].z());
        }
        else if(m_ssvep->stimulationType() == speller_type::SSVEP_GRID)
        {
            m_centerPoints[j].setX(refPoints::grid_centers[j].x());
            m_centerPoints[j].setY(refPoints::grid_centers[j].y());
            m_centerPoints[j].setZ(refPoints::grid_centers[j].z());
        }
    }

}

void SsvepCircle::initCircles()
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

    if(m_ssvep->controlMode() == control_mode::SYNC)
    {
        // start = 1;
        start = 0;
    }
    else
    {
        --elements;
    }


    // qDebug()<< Q_FUNC_INFO <<"after" << elements;
    m_centers.resize(m_ssvep->nrElements());

    for (int j = start; j<elements; ++j)
    {
        // circles center points
        // x = refPoints::centers[j].x();
        // y = refPoints::centers[j].y();
        // z = refPoints::centers[j].z();
        x = m_centerPoints[j].x();
        y = m_centerPoints[j].y();
        z = m_centerPoints[j].z();

        setVertex(k, x, y, z);

        // circles vertices
        for ( int i = k+1; i < stop; i++ )
        {

            // xx = (x + (glUtils::RADIUS * cos(i * twicePi / glUtils::SIDES_PER_CIRCLE)));
            // yy = (y + (glUtils::RADIUS * sin(i * twicePi /glUtils::SIDES_PER_CIRCLE)));
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
        //  setVertex(ind, refPoints::centers[i].x(), refPoints::centers[i].y(), refPoints::centers[i].z());
        setVertex(ind, m_centerPoints[i].x(), m_centerPoints[i].y(), m_centerPoints[i].z());
        ++i;
    }

    // qDebug()<< Q_FUNC_INFO << m_ssvep->nrElements() << m_vertices.count();
}

void SsvepCircle::initColors()
{
    int vectorsSize = (m_ssvep->nrElements() * m_vertexPerCircle) + m_ssvep->nrElements();

    // qDebug()<< "vectorsize"<< vectorsSize;

    m_colors.resize(vectorsSize);

    if(m_ssvep->controlMode() == control_mode::SYNC)
    {
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
    else
    {
        // init colors
        // center idle stim
        for (int i=0; i<m_vertexPerCircle; ++i)
        {
            m_colors[i] = glColors::gray;
        }

        for (int i=m_vertexPerCircle; i<m_colors.count(); i++)
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
    // qDebug()<< Q_FUNC_INFO << m_colors.count(); // << m_colors;
}

void SsvepCircle::initIndices()
{
    // init indices
    int circleCount = m_ssvep->nrElements();

    //qDebug()<<"circlecount" << circleCount;

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

    //    qDebug()<< Q_FUNC_INFO << m_vertices.count() << m_vindices.count() << m_centerindices.count();
    //    qDebug()<< Q_FUNC_INFO << m_vindices.count() << m_centerindices;
    //    qDebug()<< Q_FUNC_INFO << m_vindices.count() << m_vindices;
}





