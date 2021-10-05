#include <QLayout>
#include <QTimer>
#include <QTime>
#include <QMap>
#include <QSound>
#include <QDebug>
//
#include "auditoryspeller.h"
#include "erp.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//

AuditorySpeller:: AuditorySpeller(QWidget *parent) : Speller(0)
{
    // qDebug()<< Q_FUNC_INFO;

    setupUi(this);

    this->setProperty("windowTitle", "ERP Auditory Speller");

    showWindow();

    createLayout();
    initTimers();
    initFeedbackSocket();

    m_state = trial_state::PRE_TRIAL;
}

void AuditorySpeller::startFlashing()
{
    sendStimulationInfo();

    // play audio sounds
    switch (m_flashingSequence->sequence[m_currentStimulation])
    {
    case 1:
    {
        QSound::play(":/audio/audio/duck.wav");
        break;
    }
    case 2:
    {
        QSound::play(":/audio/audio/singing_bird.wav");
        break;
    }
    case 3:
    {
        QSound::play(":/audio/audio/frog.wav");
        break;
    }
    case 4:
    {
        QSound::play(":/audio/audio/seagull.wav");
        break;
    }
    case 5:
    {
        QSound::play(":/audio/audio/dove.wav");
        break;
    }
    }

    switchStimulationTimers();
}

void AuditorySpeller::pauseFlashing()
{
    switchStimulationTimers();
    ++m_currentStimulation;
    trialEnd();
}

void AuditorySpeller::highlightTarget()
{
    // qDebug() << Q_FUNC_INFO;

    int currentTarget = getCurrentTarget();

    this->layout()->itemAt(currentTarget)->
            widget()->setStyleSheet("QLabel { color : red; font: 60pt }");

   // this->layout()->itemAt(m_currentTarget)->
   //         widget()->setStyleSheet("QLabel { color : red; font: 60pt }");
}

void AuditorySpeller::refreshTarget()
{

       this->layout()->itemAt(m_currentTarget)->
               widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
    // qDebug()<< Q_FUNC_INFO << m_currentTarget;
    // qDebug()<< Q_FUNC_INFO << "[current target] "<< m_currentTarget <<"[map] "<< m_currentTarget-1;

}


void AuditorySpeller::feedback()
{
    //qDebug() << Q_FUNC_INFO;

    receiveFeedback();

    m_textRow->setText(m_text);

    if (m_text[m_text.length()-1] != "#")
    {

        if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {

            if( m_text[m_text.length()-1] == m_desiredPhrase[m_currentLetter - 1])
            {
                this->layout()->itemAt(m_currentTarget)->
                        widget()->setStyleSheet("QLabel { color : green; font: 40pt }");

                isCorrect = true;
                ++m_correct;
            }
            else
            {
                this->layout()->itemAt(m_currentTarget)->
                        widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");

                isCorrect = false;
            }
        }

        else if (m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            this->layout()->itemAt(0)->
                    widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");
        }
    }

    postTrial();
}

void AuditorySpeller::postTrial()
{
   // qDebug()<< Q_FUNC_INFO;

    ++m_trials;

    utils::wait(250);

    if (m_text[m_text.length()-1] != "#")
    {
        if (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            // refreshTarget();

        }
        else if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            refreshTarget();
        }
    }
    //
    // Send and Recieve feedback to/from Robot if external communication is enabled
    externalCommunication();

    m_currentStimulation = 0;
    m_state = trial_state::PRE_TRIAL;
    //
    if (m_currentLetter >= m_desiredPhrase.length() &&
            m_desiredPhrase.length() != 1 &&
            (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
             m_ERP->experimentMode() == operation_mode::CALIBRATION)
            )
    {
        m_correct = (m_correct / m_desiredPhrase.length()) * 100;
        qDebug()<< "Accuracy on ERP session: " << m_correct;
        qDebug()<< "Experiment End, closing speller";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        utils::wait(2000);
        emit(slotTerminated());
        this->close();
    }
    else if(m_desiredPhrase.length() <= 1)
    {
        // qDebug() << "[POST TRIAL 1]" << Q_FUNC_INFO;
        m_currentLetter = 0;
        emit(slotTerminated());
        return;
    }
    else
    {
        startTrial();
    }

}



void AuditorySpeller::createLayout()
{
    // qDebug()<< Q_FUNC_INFO;
    // speller settings
    m_rows = 1;
    m_cols = 5;
    m_nrElements = m_rows * m_cols;

    m_icons = QList<QPixmap>();
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);

    QGridLayout *layout = new QGridLayout();

    m_textRow = new QLabel(this);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignLeft);
    m_textRow->setObjectName("Desired Phrase Row");

    layout->setHorizontalSpacing(400);
    layout->setVerticalSpacing(100);

    m_textRow->setText(m_desiredPhrase);

    int k = 0;
    QString stimName;
    QPixmap pic;
    QImage iconImage;
    int label_h, label_w;
    // add speller ellements
    for(int i=1; i<m_rows+1; i++)
    {
        for(int j=0; j<m_cols; j++)
        {
            QLabel *element = new QLabel(this);
            label_h = element->height() + 40;
            label_w = element->width() + 40;

            element->setText(QString::number(k));
            element->setStyleSheet("font: 40pt; color:gray");
            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element, i, j);

            m_presentedLetters.append(QString::number(k));
            k++;
        }
    }

    this->setLayout(layout);

}
