#include <QLayout>
#include <QTimer>
#include <QTime>
//
#include "facespeller.h"
#include "erp.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//
void FaceSpeller::startFlashing()
{

    sendStimulationInfo();

    switch(m_ERP->stimulationType())
    {
    case speller_type::FACES_SPELLER:
    {
        stimulationFace();
        break;
    }
    case speller_type::INVERTED_FACE:
    {
        stimulationInvertedFace();
        break;
    }
    case speller_type::COLORED_FACE :
    {
        stimulationColoredFace();
        break;
    }
    case speller_type::INVERTED_COLORED_FACE:
    {
        stimulationInvertedColoredFace();
        break;
    }
    }

    switchStimulationTimers();
}


void FaceSpeller::stimulationFace()
{
    // qDebug()<< Q_FUNC_INFO;

    this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
            widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face.png)");


    /*
    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("image: url(:/images/yin_yang_small_croped.png)");
            */

    /*this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("image: url(:/images/whitehouse.png)");
            */

}

void FaceSpeller::stimulationColoredFace()
{
    switch (m_flashingSequence->sequence[m_currentStimulation])
    {
    case 1:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_blue.png)");
        break;
    }
    case 2:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_green.png)");
        break;
    }
    case 3:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_red.png)");
        break;
    }
    case 4:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_brown.png)");
        break;
    }
    case 5:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_cyan.png)");
        break;
    }
    case 6:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face.png)");
        break;
    }
    case 7:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_yellow.png)");
        break;
    }
    case 8:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_orange.png)");
        break;
    }
    case 9:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_magenta.png)");
        break;
    }

    }

}

void FaceSpeller::stimulationInvertedFace()
{
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");

    /*
    qDebug()<< Q_FUNC_INFO;

    QPalette pl = this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()->palette();

    QPixmap picMap(":/images/bennabi_face_inverted.png");

    pl.setBrush(QPalette::Base, QBrush(picMap));

    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->palette();
    */

}

void FaceSpeller::stimulationInvertedColoredFace()
{

    int currentStim = m_flashingSequence->sequence[m_currentStimulation];

    //qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    if(currentStim <= 3)
    {
        //        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
        //                widget()->setStyleSheet("image: url(:/images/bennabi_face_red_inverted.png)");

        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_red_inverted.png)");


    }

    else if (currentStim <= 6)
    {
        //        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
        //                widget()->setStyleSheet("image: url(:/images/bennabi_face_green_inverted.png)");
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_green_inverted.png)");

    }
    else if (currentStim <= 9)
    {
        //      this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
        //              widget()->setStyleSheet("image: url(:/images/bennabi_face_blue_inverted.png)");
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_blue_inverted.png)");

    }
    /*
    switch (m_flashingSequence->sequence[m_currentStimulation])
    {
    case 1:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_blue_inverted.png)");
        break;
    }
    case 2:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_green_inverted.png)");
        break;
    }
    case 3:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_red_inverted.png)");
        break;
    }
    case 4:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_brown_inverted.png)");
        break;
    }
    case 5:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_cyan_inverted.png)");
        break;
    }
    case 6:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");
        break;
    }
    case 7:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_yellow_inverted.png)");
        break;
    }
    case 8:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_orange_inverted.png)");
        break;
    }
    case 9:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_magenta_inverted.png)");
        break;
    }


    }
    */
}
