#include "spellercircdir.h"
#include "circularlayout.h"

SpellerCircDir::SpellerCircDir(QWidget *parent) : Speller(1)
{
    // qDebug()<< Q_FUNC_INFO;
    setupUi(this);
    this->setProperty("windowTitle", "ERP Small Circular Speller");
    createLayout();
    showWindow();
}

void SpellerCircDir::startFlashing()
{
    sendStimulationInfo();
    stimulationColoredFace();
    switchStimulationTimers();
}

void SpellerCircDir::stimulationColoredFace()
{
    int currentStim = m_flashingSequence->sequence[m_currentStimulation] - 1;
    int mod = currentStim % 3;
    QPixmap pixmap;


    if(currentStim == 0)
    {
        pixmap =  QPixmap(":/images/bennabi_face_orange.png");
    }
    else if( mod == 1)
    {
        pixmap =  QPixmap(":/images/bennabi_face_magenta.png");
    }

    else if(mod == 2)
    {
        pixmap = QPixmap(":/images/bennabi_face_blue.png");
    }

    else if(mod == 0)
    {
        pixmap = QPixmap(":/images/bennabi_face_red.png");
    }

    this->layout()->itemAt(currentStim)->widget()->setProperty("pixmap", pixmap);

}

void SpellerCircDir::feedback()
{
    receiveFeedback();

    m_textRow->setText(m_text);
    int id = m_text[m_text.length()-1].digitValue();

    if (m_presentFeedback)
    {
        QPixmap map = m_icons[id-1];
        if (m_text[m_text.length()-1] != "#")
        {

            if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
            {
                if( Correct())
                {
                    map.fill(m_correctColor);
                    isCorrect = true;
                    ++m_correct;
                }
                else
                {
                    map.fill(m_incorrectColor);
                    isCorrect = false;
                }
            }

            else if (m_ERP->experimentMode() == operation_mode::FREE_MODE)
            {
                map.fill(m_incorrectColor);
            }
        }
        this->layout()->itemAt(id-1)->
                widget()->setProperty("pixmap", map);
    }

    postTrial();
}


void SpellerCircDir::createLayout()
{
    // qDebug()<< Q_FUNC_INFO;
    // speller settings
    m_rows = 2;
    m_cols = 4;
    m_nrElements = (m_rows * m_cols) + 1;

    m_icons = QList<QPixmap>();
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);

    CircularLayout *layout = new CircularLayout();

    m_textRow = new QLabel(this);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignLeft);
    m_textRow->setObjectName("Desired Phrase Row");

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

        stimName = ":/images/circ" + QString::number(i) + ".png"; // directions images
        pic = QPixmap(stimName);

        element->setPixmap(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));
        element->setAlignment(Qt::AlignCenter);

        layout->addWidget(element);
        m_presentedLetters.append(QString::number(i+1));

        m_icons.append(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));
    }

    this->setLayout(layout);
}

SpellerCircDir::~SpellerCircDir(){}
