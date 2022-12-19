//
#include <QWindow>
#include <QMessageBox>
#include <QGenericMatrix>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QHBoxLayout>
#include <QThread>
#include <QTime>
#include <QPropertyAnimation>
#include <QDateTime>
#include <QPixmap>
#include <QDir>
#include <QPainter>
//
#include "speller.h"
#include "configpanel.h"
#include "ovtk_stimulations.h"
#include "randomflashsequence.h"
#include "utils.h"
#include "spellersmall.h"

SpellerSmall::SpellerSmall(QWidget *parent) : Speller(0)
  //ui(new Ui::Speller)
{
    // qDebug()<< Q_FUNC_INFO;
    // ui->setupUi(this);
    setupUi(this);
    this->setProperty("windowTitle", "ERP Small Speller");
    // this->setStyleSheet("background-color:white");
    // this->show();

    showWindow();
    createLayout();
    initTimers();
    initFeedbackSocket();

    m_state = trial_state::PRE_TRIAL;
    m_highlight = QPixmap(":/images/bennabi_face_red_inverted.png");
    QLabel label;
    int label_h = label.height() + 40;
    int label_w = label.width() + 40;
    m_highlight.scaled(label_w, label_h, Qt::KeepAspectRatio);
    m_highlight.fill(m_highlightColor);
    // this->setWindowFlag(Qt::Popup);
}


void SpellerSmall::startFlashing()
{
      // qDebug()<< Q_FUNC_INFO;

    sendStimulationInfo();

    int currentStim = m_flashingSequence->sequence[m_currentStimulation];
    QPixmap pixmap;
    /*
    if(currentStim <= 3)
    {
        pixmap = QPixmap(":/images/bennabi_face_blue.png");
    }

    else if (currentStim <= 6)
    {
        pixmap = QPixmap(":/images/bennabi_face_red.png");
    }*/

    if ( (currentStim % 3) == 1 )
    {
        pixmap = QPixmap(":/images/bennabi_face_magenta.png");
    }

    else if ((currentStim % 3) == 2)
    {
        pixmap = QPixmap(":/images/bennabi_face_blue.png");
    }

    else if ((currentStim % 3) == 0)
    {
        pixmap = QPixmap(":/images/bennabi_face_red.png");
    }

    this->layout()->itemAt(currentStim-1)->widget()->setProperty("pixmap", pixmap);

    switchStimulationTimers();
}


void SpellerSmall::pauseFlashing()
{

    int id = m_flashingSequence->sequence[m_currentStimulation];
    this->layout()->itemAt(id-1)->
            widget()->setProperty("text", QString::number(id));

    switchStimulationTimers();
    ++m_currentStimulation;

    trialEnd();
}


void SpellerSmall::createLayout()
{
    //  qDebug()<< Q_FUNC_INFO;
    // speller settings
    m_rows = 2;
    m_cols = 3;
    m_nrElements = m_rows * m_cols;

    m_icons = QList<QPixmap>();
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);

    QGridLayout *layout = new QGridLayout();

    m_textRow = new QLabel(this);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignLeft);
    m_textRow->setObjectName("Desired Phrase Row");

    // layout->setHorizontalSpacing(400);
    // layout->setVerticalSpacing(100);
    layout->setHorizontalSpacing(110); //110 for alignment
    // layout->setVerticalSpacing(5); //40

    m_textRow->setText(m_desiredPhrase);
    m_textRow->hide();

    int k = 1;
    QString stimName;
    QPixmap pic;
    QImage iconImage;
    int label_h, label_w;
    // add speller ellements
    Qt::AlignmentFlag alignment = Qt::AlignCenter;
    for(int i=1; i<m_rows+1; i++)
    {
        for(int j=0; j<m_cols; j++)
        {
            QLabel *element = new QLabel(this);

            label_h = element->height() + 40;
            label_w = element->width() + 40;

            stimName = ":/images/" + QString::number(k) + ".png"; // directions images
            pic = QPixmap(stimName);

            element->setText(QString::number(k));
            element->setStyleSheet("font: 40pt; color:gray");

            element->setAlignment(Qt::AlignCenter);
            m_icons.append(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));

            // QuickHack: FIXME
            if (j ==0)
            {
                alignment = Qt::AlignRight;
            }
            else if (j==1)
            {
                alignment = Qt::AlignCenter;
            }
            else {
                alignment= Qt::AlignLeft;
            }

            layout->addWidget(element, i, j, alignment);

            m_presentedLetters.append(QString::number(k));
            k++;
            // presented_letters.append(letters[i-1][j]);
        }
    }

    this->setLayout(layout);
}


void SpellerSmall::highlightTarget()
{
    // qDebug() << Q_FUNC_INFO;

    int currentTarget = getCurrentTarget() - 1;

    this->layout()->itemAt(currentTarget)->
            widget()->setStyleSheet("QLabel { color : yellow; font: 60pt }");
}

void SpellerSmall::refreshTarget()
{
    this->layout()->itemAt(getCurrentTarget() - 1)->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
}

void SpellerSmall::preTrial()
{
    // qDebug()<< Q_FUNC_INFO;

    if(m_trials == 0)
    {
        sendMarker(OVTK_StimulationId_ExperimentStart);
        // setting a pre-trail duration longer for calibration phase
        // than Copy_mode/Free mode phases
        if (m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            m_preTrialTimer->setInterval(1000);
        }
        else
        {
            m_preTrialTimer->setInterval(500);
        }
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

void SpellerSmall::feedback()
{
    // qDebug() << Q_FUNC_INFO;

    receiveFeedback();

    //   qDebug()<< m_text[m_text.length()-1] << " "<< m_desiredPhrase[m_currentLetter - 1];

    m_textRow->setText(m_text);
    int id = m_text[m_text.length()-1].digitValue();
    QString fbkColor = "white";
    if (m_presentFeedback)
    {
        if (m_text[m_text.length()-1] != "#")
        {

            if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
            {

                if( m_text[m_text.length()-1] == m_desiredPhrase[m_currentLetter - 1])
                {
                    fbkColor = "green";
                    isCorrect = true;
                    ++m_correct;
                }
                else
                {
                    fbkColor = "blue";
                    isCorrect = false;
                }
            }

            else if (m_ERP->experimentMode() == operation_mode::FREE_MODE)
            {
                // this->layout()->itemAt(0)->
                //         widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");
                fbkColor = "red";
            }
        }

        this->layout()->itemAt(id-1)->
                widget()->setStyleSheet("QLabel { color : " + fbkColor + "; font: 40pt }");
    }
    postTrial();
}

void SpellerSmall::postTrial()
{
    //   qDebug()<< Q_FUNC_INFO;

    ++m_trials;
    int id = m_text[m_text.length()-1].digitValue();
    if (m_presentFeedback)
    {
        utils::wait(100);

        if (m_text[m_text.length()-1] != "#")
        {
            if (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                    m_ERP->experimentMode() == operation_mode::FREE_MODE)
            {
                // refreshTarget();
                this->layout()->itemAt(id-1)->
                        widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
            }
            else if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
            {
                refreshTarget();
            }
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

SpellerSmall::~SpellerSmall()
{

}
