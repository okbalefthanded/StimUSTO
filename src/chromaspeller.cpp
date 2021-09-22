#include <QLayout>
#include <QTimer>
#include <QTime>
#include <QMap>
//
#include "chromaspeller.h"
#include "erp.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//

ChromaSpeller::ChromaSpeller(QWidget *parent) : Speller(0)
{
    // qDebug()<< Q_FUNC_INFO;

    setupUi(this);

    this->setProperty("windowTitle", "ERP Chroma Speller");

    showWindow();

    createLayout();
    initTimers();
    initFeedbackSocket();

    m_state = trial_state::PRE_TRIAL;
    m_highlightColor = Qt::white;
    m_incorrectColor = Qt::gray;

}



void ChromaSpeller::startFlashing()
{
    sendStimulationInfo();

    int id = m_flashingSequence->sequence[m_currentStimulation];
    QPixmap pixmap(":/images/bennabi_face_red_inverted.png");

    m_element = new QLabel();
    m_element->setPixmap(pixmap);
    m_element->setAlignment(Qt::AlignCenter);

    this->layout()->replaceWidget(this->
                                  layout()->
                                  itemAt(id-1)->
                                  widget(),
                                  m_element,
                                  Qt::FindDirectChildrenOnly);

    switchStimulationTimers();

}


void ChromaSpeller::createLayout()
{
    // qDebug()<< Q_FUNC_INFO;
    // speller settings
    m_rows = 3;
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

    layout->setHorizontalSpacing(400);
    layout->setVerticalSpacing(100);

    m_textRow->setText(m_desiredPhrase);

    int k = 1;
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

            // stimName = ":/images/" + m_iconMap.value(k) + ".png"; // directions images
            stimName = ":/images/" + m_iconMap.value(k);

            pic = QPixmap(stimName);

            element->setPixmap(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));
            element->setAlignment(Qt::AlignCenter);

            m_icons.append(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));

            layout->addWidget(element, i, j);

            m_presentedLetters.append(QString::number(k));
            k++;
        }
    }

    this->setLayout(layout);
}

