//
#include <QLayout>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QWindow>
#include <QtWidgets>
//
#include "arabicspeller.h"
#include "erp.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//
ArabicSpeller::ArabicSpeller(QWidget *parent) : Speller(12345)
{
    qDebug()<< Q_FUNC_INFO;

    setupUi(this);

    this->setProperty("windowTitle", "ERP Arabic Speller");

    showWindow();

    createLayout();
    initTimers();
    initFeedbackSocket();

    m_state = trial_state::PRE_TRIAL;
}

void ArabicSpeller::startFlashing()
{
    // qDebug()<< Q_FUNC_INFO;
    sendStimulationInfo();

    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");

    switchStimulationTimers();
}

void ArabicSpeller::pauseFlashing()
{
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

    switchStimulationTimers();
    ++m_currentStimulation;

    trialEnd();

}

void ArabicSpeller::createLayout()
{

    qDebug()<< Q_FUNC_INFO;
    // speller settings
    // Arabic speller
    m_rows = 5;
    m_cols = 8;
    m_nrElements = m_rows * m_cols;

    m_icons = QList<QPixmap>();
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);
    QGridLayout *layout = new QGridLayout();

    m_textRow = new QLabel(this);
    m_textRow->setText(m_desiredPhrase);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignLeft);
    //    textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_textRow, 0, 0, 1, 0);

    int k = 0, nbr=0;
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
            label_h = element->height();
            label_w = element->width();

            if(k < utils::ArabicLetters.length())
            {
                element->setText(utils::ArabicLetters.at(k));
                m_presentedLetters.append(utils::ArabicLetters.at(k));
            }
            else
            {
                element->setText(utils::numbers.at(nbr));
                m_presentedLetters.append(utils::numbers.at(nbr));
                nbr++;
            }

            element->setStyleSheet("font: 40pt; color:gray");
            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element, i, j);

            k++;

        }
    }

    this->setLayout(layout);

}

void ArabicSpeller::highlightTarget()
{
    qDebug() << Q_FUNC_INFO;

    int currentTarget = getCurrentTarget();

    this->layout()->itemAt(currentTarget)->
            widget()->setStyleSheet("QLabel { color : red; font: 60pt }");

   // this->layout()->itemAt(m_currentTarget)->
   //         widget()->setStyleSheet("QLabel { color : red; font: 60pt }");
}

void ArabicSpeller::refreshTarget()
{
    this->layout()->itemAt(m_currentTarget)->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
}

void ArabicSpeller::preTrial()
{
    qDebug()<< Q_FUNC_INFO;

    if(m_trials == 0)
    {
        sendMarker(OVTK_StimulationId_ExperimentStart);
    }

    if (m_preTrialCount == 0)
    {
        sendMarker(OVTK_StimulationId_TrialStart);
        m_flashingSequence = new RandomFlashSequence(m_nrElements, m_ERP->nrSequences());

        if (m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            highlightTarget();
            m_text += m_desiredPhrase[m_currentLetter];
            m_textRow->setText(m_text);
        }
        else if(m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {
            highlightTarget();
        }
        else if(m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            utils::wait(500);
        }
    }

    // startPreTrial();

    m_preTrialTimer->start();
    ++m_preTrialCount;

    endPreTrial();

}

void ArabicSpeller::feedback()
{
    qDebug() << Q_FUNC_INFO;

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

void ArabicSpeller::postTrial()
{
    qDebug()<< Q_FUNC_INFO;

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
    // externalCommunication();
    if(m_ERP->externalComm() == external_comm::ENABLED)
    {
       m_externComm->communicate(QString(m_text[m_text.length()-1]));
    }

    postTrialEnd();

}

ArabicSpeller::~ArabicSpeller()
{

}
