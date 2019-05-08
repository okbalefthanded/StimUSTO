//
#include <QtDebug>
//
#include "hybridstimulation.h"
#include "facespeller.h"
#include "flashingspeller.h"
//
HybridStimulation::HybridStimulation(ERP *erp, SSVEP *ssvep)
{
    initERPspeller(erp);
    initSSVEP(ssvep);
}


void HybridStimulation::hybridPreTrial()
{

}

void HybridStimulation::hybridStartTrial()
{
    qDebug() << Q_FUNC_INFO << "[TRIAL START]";

    if(m_hybridState == trial_state::PRE_TRIAL)
    {
       // ERPspeller->preTrial();
    }

}



void HybridStimulation::initERPspeller(ERP *erp)
{
    int spellerType = erp->stimulationType();

    switch (spellerType)
    {
    case speller_type::FLASHING_SPELLER:
    {
        //FlashingSpeller *flashSpeller = new FlashingSpeller();
        //flashSpeller->initSpeller(erp);
        ERPspeller = new FlashingSpeller();
        ERPspeller->initSpeller(erp);
        break;
    }
        // case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
    case speller_type::FACES_SPELLER:
    case speller_type::INVERTED_FACE:
    case speller_type::COLORED_FACE:
    case speller_type::INVERTED_COLORED_FACE:
    {
        // FaceSpeller *faceSpeller = new FaceSpeller();
        // faceSpeller->initSpeller(erp);
        ERPspeller = new FaceSpeller();
        ERPspeller->initSpeller(erp);
        break;
    }
    }
}

void HybridStimulation::initSSVEP(SSVEP *ssvep)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    // format.setVersion(3,3);
    format.setVersion(3,0); // ANGLE supports ES 3.0, higher versions raise exceptions

    //SsvepGL *ssvepStimulation = new SsvepGL(*ssvep);
    ssvepStimulation = new SsvepGL(*ssvep);
    ssvepStimulation->setFormat(format);

    if(QGuiApplication::screens().size() == 2)
    {
        ssvepStimulation->resize(utils::getScreenSize());
    }
    else
    {
        ssvepStimulation->resize(QSize(1366, 768)); //temporaty size;
    }
    ssvepStimulation->hide();
}
