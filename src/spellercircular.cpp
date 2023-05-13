#include "spellercircular.h"
#include "circularlayout.h"

SpellerCircular::SpellerCircular(QWidget *parent, quint16 t_port) : SpellerSmall(t_port)
{
    // qDebug()<< Q_FUNC_INFO;
    setupUi(this);
    this->setProperty("windowTitle", "ERP Small Circular Speller");
    createLayout();
    showWindow();
}

void SpellerCircular::startFlashing()
{
    sendStimulationInfo();
    stimulationColoredFace();
    switchStimulationTimers();
}

void SpellerCircular::pauseFlashing()
{
    int id = m_flashingSequence->sequence[m_currentStimulation];
    QString elemColor = getElemColor(id);

    if(m_ERP->controlMode() == control_mode::ASYNC)
    {
        // do nothing
        if(id > 1)
        {
            this->layout()->itemAt(id-1)->
                    widget()->setStyleSheet("QLabel { color : " + elemColor + "; font: 40pt }");

            this->layout()->itemAt(id-1)->
                    widget()->setProperty("text", QString::number(id-1));
        }
    }

    else
    {

        this->layout()->itemAt(id)->
                widget()->setStyleSheet("QLabel { color : " + elemColor + "; font: 40pt }");

        this->layout()->itemAt(id)->
                widget()->setProperty("text", QString::number(id));
    }

    switchStimulationTimers();
    ++m_currentStimulation;

    trialEnd();
}

void SpellerCircular::highlightTarget()
{
    int currentTarget = getCurrentTarget();
    if(isAsync()){--currentTarget;}
    this->layout()->itemAt(currentTarget)->
            widget()->setStyleSheet("QLabel { color : yellow; font: 60pt }");
}

void SpellerCircular::refreshTarget()
{
    QString elemColor = getElemColor(m_currentTarget);
    int currentTarget = getCurrentTarget();
    if(isAsync()){--currentTarget;}
    this->layout()->itemAt(currentTarget)->
            widget()->setStyleSheet("QLabel { color : "+elemColor+"; font: 40pt }");
}

void SpellerCircular::feedback()
{

    receiveFeedback();

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
        if(isAsync()){--id;}
        this->layout()->itemAt(id)->
                widget()->setStyleSheet("QLabel { color : " + fbkColor + "; font: 60pt }");
    }

    postTrial();
}

void SpellerCircular::postTrial()
{
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
                elemColor = getElemColor(id);
                if(isAsync()){--id;}
                this->layout()->itemAt(id)->
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

void SpellerCircular::stimulationColoredFace()
{
    int currentStim = m_flashingSequence->sequence[m_currentStimulation];

    if(m_ERP->controlMode() == control_mode::ASYNC)
    {
        if (currentStim == 1)
        { return;}
        else
        {
            QPixmap pixmap;

            if ( (currentStim == 2) || (currentStim == 5))
            {
                // pixmap = QPixmap(":/images/bennabi_face_magenta.png");
                pixmap = QPixmap(":/images/bennabi_face_blue.png");
            }

            else if ((currentStim == 3) || (currentStim == 7))
            {
                //     pixmap = QPixmap(":/images/bennabi_face_blue.png");
                pixmap = QPixmap(":/images/bennabi_face_red.png");
            }

            else if ((currentStim == 4) || (currentStim == 6))
            {
                //  pixmap = QPixmap(":/images/bennabi_face_red.png");
                // pixmap = QPixmap(":/images/bennabi_face_blue.png");
                pixmap = QPixmap(":/images/bennabi_face_magenta.png");
            }

            this->layout()->itemAt(currentStim-1)->widget()->setProperty("pixmap", pixmap);
        }
    }

    else
    {
        QPixmap pixmap;

        if ( (currentStim == 1) || (currentStim == 4))
        {
            // pixmap = QPixmap(":/images/bennabi_face_magenta.png");
            pixmap = QPixmap(":/images/bennabi_face_blue.png");
        }

        else if ((currentStim == 2) || (currentStim == 6))
        {
            //     pixmap = QPixmap(":/images/bennabi_face_blue.png");
            pixmap = QPixmap(":/images/bennabi_face_red.png");
        }

        else if ((currentStim == 3) || (currentStim == 5))
        {
            //  pixmap = QPixmap(":/images/bennabi_face_red.png");
            // pixmap = QPixmap(":/images/bennabi_face_blue.png");
            pixmap = QPixmap(":/images/bennabi_face_magenta.png");
        }

     //   this->layout()->itemAt(currentStim-1)->widget()->setProperty("pixmap", pixmap);
        this->layout()->itemAt(currentStim)->widget()->setProperty("pixmap", pixmap);
    }
}

int SpellerCircular::getnElements()
{
    int n_elements = m_nrElements;
    if(!isAsync()){--n_elements;}
    return n_elements;
}

void SpellerCircular::createLayout()
{
    // qDebug()<< Q_FUNC_INFO;
    // speller settings
    m_rows = 2;
    m_cols = 3;
    m_nrElements = (m_rows * m_cols) + 1;

    m_icons = QList<QPixmap>();
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);

    // QGridLayout *layout = new QGridLayout();
    CircularLayout *layout = new CircularLayout();

    m_textRow = new QLabel(this);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignLeft);
    m_textRow->setObjectName("Desired Phrase Row");

    // layout->setHorizontalSpacing(400);
    // layout->setVerticalSpacing(100);
    // layout->setHorizontalSpacing(110); //110 for alignment
    // layout->setVerticalSpacing(5); //40

    m_textRow->setText(m_desiredPhrase);
    m_textRow->hide();

    QString stimName;
    QPixmap pic;
    // QImage iconImage;
    int label_h, label_w;
    // add speller ellements
    // Qt::AlignmentFlag alignment = Qt::AlignCenter;
    QString elemColor = "gray";

    for (int i=0; i<m_nrElements; i++)
    {
        QLabel *element = new QLabel(this);
        label_h = element->height() + 40;
        label_w = element->width()  + 40;

        element->setText(QString::number(i));
        element->setAlignment(Qt::AlignCenter);
        element->setStyleSheet("font: 40pt; color:"+elemColor);

        layout->addWidget(element);
        m_presentedLetters.append(QString::number(i+1));
        if (i > 0)
        {
            stimName = ":/images/" + QString::number(i+1) + ".png"; // directions images
            pic = QPixmap(stimName);
            m_icons.append(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));
        }
    }

    this->setLayout(layout);
}

void SpellerCircular::showFeedback(QString command, bool correct)
{
    qDebug()<< Q_FUNC_INFO << command;
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
        }

        fillFeedBackMap(&map, mapColor, textColor, speed);
        if(isAsync()){--id;}
        this->layout()->itemAt(id)->
                            widget()->setProperty("pixmap", map);

        utils::wait(500);

        // refreshtarget
        if (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            this->layout()->itemAt(id)->
                    widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

            this->layout()->itemAt(id)->
                    widget()->setProperty("text", QString::number(id));
        }
    }

}

SpellerCircular::~SpellerCircular(){}
