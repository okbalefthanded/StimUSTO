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
    case speller_type::MISMATCH:
        stimulationMismatchFace();
        break;
    }

    switchStimulationTimers();
}

void FaceSpeller::stimulationFace()
{
    // qDebug()<< Q_FUNC_INFO;
    int id = m_flashingSequence->sequence[m_currentStimulation];
    QPixmap pixmap(":/images/bennabi_face.png");
    m_element = new QLabel();
    m_element->setPixmap(pixmap);
    m_element->setAlignment(Qt::AlignCenter);

    this->layout()->replaceWidget(this->
                                  layout()->
                                  itemAt(id)->
                                  widget(),
                                  m_element,
                                  Qt::FindDirectChildrenOnly);
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

    int id = m_flashingSequence->sequence[m_currentStimulation];
    // QPixmap pixmap(":/images/bennabi_face_inverted.png");
    // QPixmap pixmap(":/images/bennabi_face_red_inverted.png");
    // QPixmap pixmap(":/images/rsz_whitehouse_small.png");
    m_element = new QLabel();
    // m_element->setPixmap(pixmap);
    m_element->setPixmap(*m_stimuli);
    m_element->setAlignment(Qt::AlignCenter);

    this->layout()->replaceWidget(this->
                                  layout()->
                                  itemAt(id-1)->
                                  widget(),
                                  m_element,
                                  Qt::FindDirectChildrenOnly);

}

void FaceSpeller::stimulationInvertedColoredFace()
{

    int currentStim = m_flashingSequence->sequence[m_currentStimulation];
    // qDebug()<< Q_FUNC_INFO << currentStim;

    QPixmap pixmap;
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);

    //qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    if(currentStim <= 3)
    {
        // pixmap = QPixmap(":/images/bennabi_face_red_inverted.png");
        // pixmap = QPixmap(":/images/bennabi_face_magenta_inverted.png");
        pixmap = m_multStimuli[0];
    }

    else if (currentStim <= 6)
    {
        // pixmap = QPixmap(":/images/bennabi_face_blue_inverted.png");
        pixmap = m_multStimuli[1];
    }
    else if (currentStim <= 9)
    {
        // pixmap = QPixmap(":/images/bennabi_face_magenta_inverted.png");
        // pixmap = QPixmap(":/images/bennabi_face_red_inverted.png");
        pixmap = m_multStimuli[2];
    }

    m_element->setPixmap(pixmap);
    this->layout()->replaceWidget(this->
                                  layout()->
                                  itemAt(currentStim-1)->
                                  widget(),
                                  m_element,
                                  Qt::FindDirectChildrenOnly);
}

void FaceSpeller::stimulationMismatchFace()
{
    qDebug()<< Q_FUNC_INFO;

    int id = m_flashingSequence->sequence[m_currentStimulation];

    QPixmap pixmap(":/images/bennabi_face_red_inverted.png");
    QPixmap pixmap_mismatch(":/images/bennabi_face.png");

    m_element = new QLabel();
    m_element->setPixmap(pixmap);
    m_element->setAlignment(Qt::AlignCenter);

    this->layout()->replaceWidget(this->
                                  layout()->
                                  itemAt(id-1)->
                                  widget(),
                                  m_element,
                                  Qt::FindDirectChildrenOnly);


    for (int i=0;i<9;i++)
    {
        if(i != (id-1))
        {

            m_element = new QLabel();
            m_element->setPixmap(pixmap_mismatch);
            m_element->setAlignment(Qt::AlignCenter);


            this->layout()->replaceWidget(this->
                                          layout()->
                                          itemAt(i)->
                                          widget(),
                                          m_element,
                                          Qt::FindDirectChildrenOnly);
        }

    }


}
