#include "spellercircular.h"
#include "circularlayout.h"

SpellerCircular::SpellerCircular(QWidget *parent) : SpellerSmall(1)
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

void SpellerCircular::stimulationColoredFace()
{
    int currentStim = m_flashingSequence->sequence[m_currentStimulation];

    // qDebug()<< Q_FUNC_INFO << currentStim;

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

    this->layout()->itemAt(currentStim-1)->widget()->setProperty("pixmap", pixmap);
}

void SpellerCircular::createLayout()
{
    // qDebug()<< Q_FUNC_INFO;
    // speller settings
    m_rows = 2;
    m_cols = 3;
    m_nrElements = m_rows * m_cols;

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
    QImage iconImage;
    int label_h, label_w;
    // add speller ellements
    // Qt::AlignmentFlag alignment = Qt::AlignCenter;
    QString elemColor = "gray";

    for (int i=0; i<m_nrElements; i++)
    {
        QLabel *element = new QLabel(this);
        label_h = element->height() + 40;
        label_w = element->width() + 40;
        stimName = ":/images/" + QString::number(i+1) + ".png"; // directions images
        pic = QPixmap(stimName);
        m_icons.append(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));

        element->setText(QString::number(i+1));
        element->setAlignment(Qt::AlignCenter);
        element->setStyleSheet("font: 40pt; color:"+elemColor);

        layout->addWidget(element);
        m_presentedLetters.append(QString::number(i+1));
    }

    this->setLayout(layout);
}

SpellerCircular::~SpellerCircular(){}
