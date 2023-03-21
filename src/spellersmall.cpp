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
    createLayout();
    showWindow();
}

SpellerSmall::SpellerSmall(int i): Speller(i) {}

void SpellerSmall::startFlashing()
{
    // qDebug()<< Q_FUNC_INFO;

    sendStimulationInfo();

    switch(m_ERP->stimulationType())
    {
    case speller_type::SMALL:
    {
        stimulationColoredFace();
        break;
    }
    case speller_type::SMALL_FLASH:
    {
        flashing();
        break;
    }
    case speller_type::SMALL_FACE :
    {
        stimulationFace();
        break;
    }
    case speller_type::SMALL_IFACE:
    {
        stimulationInvertedFace();
        break;
    }

    case speller_type::SMALL_ICFACE:
    {
        stimulationInvertedColoredFace();
        break;
    }

    }

    switchStimulationTimers();
}

void SpellerSmall::pauseFlashing()
{

    int id = m_flashingSequence->sequence[m_currentStimulation];

    QString elemColor = getElemColor(id);

    // this->layout()->itemAt(id - 1)->
    //        widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
    this->layout()->itemAt(id - 1)->
            widget()->setStyleSheet("QLabel { color : " + elemColor + "; font: 40pt }");

    this->layout()->itemAt(id-1)->
            widget()->setProperty("text", QString::number(id));

    switchStimulationTimers();
    ++m_currentStimulation;

    trialEnd();
}

void SpellerSmall::createLayout()
{
    // qDebug()<< Q_FUNC_INFO;
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
    QString elemColor = "gray";
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
            // element->setStyleSheet("font: 40pt; color:gray");

            element->setAlignment(Qt::AlignCenter);
            m_icons.append(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));

            // QuickHack: FIXME
            if (j ==0)
            {
                alignment = Qt::AlignRight;
                //         elemColor = "magenta";
            }
            else if (j==1)
            {
                alignment = Qt::AlignCenter;
                //       elemColor = "blue";
            }
            else {
                alignment= Qt::AlignLeft;
                //     elemColor = "red";
            }
            element->setStyleSheet("font: 40pt; color:"+elemColor);
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
    QString elemColor = getElemColor(getCurrentTarget());

    this->layout()->itemAt(getCurrentTarget() - 1)->
            widget()->setStyleSheet("QLabel { color : "+elemColor+"; font: 40pt }");
}

void SpellerSmall::flashing()
{
    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation]-1)
            ->widget()
            ->setStyleSheet("QLabel { color : white; font: 60pt }");
}

void SpellerSmall::stimulationFace()
{

    int id = m_flashingSequence->sequence[m_currentStimulation];
    // QPixmap pixmap(":/images/bennabi_face.png");
    QPixmap pixmap(":/images/bennabi_face_red.png");
    this->layout()->itemAt(id-1)->widget()->setProperty("pixmap", pixmap);
}

void SpellerSmall::stimulationColoredFace()
{
    int currentStim = m_flashingSequence->sequence[m_currentStimulation];
    QPixmap pixmap;

    if ( (currentStim % 3) == 1 )
    {
        // pixmap = QPixmap(":/images/bennabi_face_magenta.png");
        pixmap = QPixmap(":/images/bennabi_face_blue.png");
    }

    else if ((currentStim % 3) == 2)
    {
        //     pixmap = QPixmap(":/images/bennabi_face_blue.png");
        pixmap = QPixmap(":/images/bennabi_face_red.png");
    }

    else if ((currentStim % 3) == 0)
    {
        //  pixmap = QPixmap(":/images/bennabi_face_red.png");
        // pixmap = QPixmap(":/images/bennabi_face_blue.png");
        pixmap = QPixmap(":/images/bennabi_face_magenta.png");
    }

    this->layout()->itemAt(currentStim-1)->widget()->setProperty("pixmap", pixmap);
}

void SpellerSmall::stimulationInvertedFace()
{
    int id = m_flashingSequence->sequence[m_currentStimulation];
    // QPixmap pixmap(":/images/bennabi_face_inverted.png");
    // QPixmap pixmap(":/images/bennabi_face_red_inverted.png");
    // QPixmap pixmap(":/images/rsz_whitehouse_small.png");
    QPixmap pixmap(":/images/bennabi_face_red_inverted.png");
    this->layout()->itemAt(id-1)->widget()->setProperty("pixmap", pixmap);
}

void SpellerSmall::stimulationInvertedColoredFace()
{
    int currentStim = m_flashingSequence->sequence[m_currentStimulation];
    QPixmap pixmap;

    if ( (currentStim % 3) == 1 )
    {
        pixmap = QPixmap(":/images/bennabi_face_magenta_inverted.png");
    }

    else if ((currentStim % 3) == 2)
    {
        pixmap = QPixmap(":/images/bennabi_face_blue_inverted.png");
    }

    else if ((currentStim % 3) == 0)
    {
        pixmap = QPixmap(":/images/bennabi_face_red_inverted.png");
    }

    this->layout()->itemAt(currentStim-1)->widget()->setProperty("pixmap", pixmap);
}

QString SpellerSmall::getElemColor(int t_index)
{
    QString elemColor = "gray";

    if ( (t_index % 3) == 1 )
    {
        elemColor = "magenta";
    }

    else if ((t_index % 3) == 2)
    {
        elemColor = "blue";
    }

    else if ((t_index % 3) == 0)
    {
        elemColor = "red";
    }

    //return elemColor;
    return "gray";
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
    receiveFeedback();

    // qDebug()<< m_text[m_text.length()-1] << " "<< m_desiredPhrase[m_currentLetter - 1];

    m_textRow->setText(m_text);
    int id = m_text[m_text.length()-1].digitValue();
    QString fbkColor = "white";
    if (m_presentFeedback)
    {
        if (m_text[m_text.length()-1] != "#")
        {
            if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
            {
                if( Correct())
                {
                    fbkColor = "green";
                    isCorrect = true;
                    ++m_correct;
                }
                else
                {
                    fbkColor = "white"; //"blue";
                    isCorrect = false;
                }
            }

            else if (m_ERP->experimentMode() == operation_mode::FREE_MODE)
            {
                fbkColor = "red";
            }
        }

        this->layout()->itemAt(id-1)->
                widget()->setStyleSheet("QLabel { color : " + fbkColor + "; font: 60pt }");
    }

    postTrial();
}

void SpellerSmall::showFeedback(QString command, bool correct)
{
    // qDebug()<< Q_FUNC_INFO << command;
    int id = 0;
    QPixmap map;
    if (command[0] != '#')
    {
        id  = command[0].digitValue();
        QString speed = command.at(1);
        QColor mapColor, textColor;
        mapColor  = Qt::blue;
        textColor = Qt::black;
        map = m_icons[id-1];
        // present feedbck : COPY mode
        if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {
            // feedback: green correct selection, highlight the target icon
            //           blue incorrect selection, highlight the selected icon
            if( m_text[m_text.length()-1] == m_desiredPhrase[m_desiredPhrase.length() - 1])
            {
                mapColor = Qt::green;
                isCorrect = true;
                ++m_correct;
            }
            else
            {
                mapColor = Qt::blue;
                isCorrect = false;
            }

            if (correct)
            {
                textColor = Qt::red;
            }
            else
            {
                textColor = Qt::black;
            }

            // fillFeedBackMap(&map, mapColor, textColor, speed);
        }
        // present feedback: FREE mode
        // else if (m_ERP->experimentMode() == operation_mode::FREE_MODE)
        //{
        //    fillFeedBackMap(&map, Qt::blue, Qt::black, speed);
        //}
        //
        fillFeedBackMap(&map, mapColor, textColor, speed);
        this->layout()->itemAt(id-1)->
                            widget()->setProperty("pixmap", map);
        //
        utils::wait(500);

        // refreshtarget
        if (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            this->layout()->itemAt(id-1)->
                    widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

            this->layout()->itemAt(id-1)->
                    widget()->setProperty("text", QString::number(id));
        }
    }
}

void SpellerSmall::postTrial()
{
    // qDebug()<< Q_FUNC_INFO;

    ++m_trials;
    int id = m_text[m_text.length()-1].digitValue();
    QString elemColor = "gray";
    if (m_presentFeedback)
    {
        // utils::wait(100);
        utils::wait(500);

        if (m_text[m_text.length()-1] != "#")
        {
            if (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                    m_ERP->experimentMode() == operation_mode::FREE_MODE)
            {
                // refreshTarget();
                elemColor = getElemColor(id);
                this->layout()->itemAt(id-1)->
                        widget()->setStyleSheet("QLabel { color : "+elemColor+"; font: 40pt }");
            }
            else if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
            {
                refreshTarget();
            }
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

SpellerSmall::~SpellerSmall(){}
