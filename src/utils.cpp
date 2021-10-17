#include <QtMath>
#include <QDebug>
#include <QTime>
//
#include "utils.h"
//
namespace utils {
// rectangle flickering : ON/OFF
QVector<double> gen_flick(double freq, int refreshRate, float length, quint8 stimMode, double phase)
{

    if(stimMode == frequency_stimulation::ON_OFF)
    {
        return gen_flick_on_off(freq, refreshRate, length);
    }
    else if( stimMode == frequency_stimulation::SIN)
    {
        return gen_flick_sin(freq, refreshRate, length, 0.0);
    }
    else if( stimMode == frequency_stimulation::SIN_FP)
    {
        return gen_flick_sin(freq, refreshRate, length, phase);
    }
}

//
QVector<double> gen_flick_on_off(double freq, int refreshRate, float length)
{
    int samples = refreshRate * (length / 1000);
    //int samples = refreshRate * length;

    int period = refreshRate / freq;QVector<double> temp(period, 1);
    QVector<double> v;

    for(int idx = period / 2; idx<period; idx++)
    {
        temp[idx] = 0;
    }
    v = temp;

    for(int i=1; i< freq * length ;i++)
    {
        v.append(temp);
    }

    v.resize(samples);
    return v;
}


// sampled sinusoidal flickering
// implement the Sampled sinusoidal frequency modulation method
// X. Chen, Z. Chen, S. Gao, and X. Gao, “A high-ITR SSVEP-based BCI speller,”
// Taylor Fr. Brain Comput. InterfacesBrain-Computer Interfaces, vol. 1, no. 3–4, pp. 181–191, 2014.
QVector<double> gen_flick_sin(double freq, int refreshRate, float length, double phase)
{
    //    int samples = refreshRate * int(length);
    // qDebug()<< Q_FUNC_INFO << phase;
    int samples = refreshRate * (length / 1000);
    QVector<double> stim(samples, 0);

    for(int index = 0; index < samples; index++)
    {
        stim[index] = 0.5 * (1 + qSin(2*M_PI*freq*(index/double(refreshRate)) + phase));
    }

    return stim;
}
//
void wait(int millisecondsToWait)
{
    //    qDebug()<< Q_FUNC_INFO;

    // from stackoverflow question:
    // http://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        //        qDebug()<<"waiting..."<<QTime::currentTime();
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100);

    }
}
//
qint8 sign(double value)
{
    return (value > 0) ? 1 : -1;
}

//

QSize getScreenSize()
{
    QList<QScreen *> pluggedScreens = QGuiApplication::screens();

    if(pluggedScreens.size() == 2)
    {
        return pluggedScreens.last()->size();
    }
    else
    {
        return pluggedScreens.first()->size();
    }
}

int screenCount()
{
    return QGuiApplication::screens().size();
}

}


