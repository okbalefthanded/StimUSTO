#include "utils.h"

namespace utils {
// rectangle flickering : ON/OFF
QVector<int> gen_flick(double freq, int refreshRate, float length)
{
    //    int samples = refreshRate * (length / 1000);
    int samples = refreshRate * length;
    int period = refreshRate / freq;
    QVector<int> temp(period, 1);
    QVector<int> v;

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

// TODO
// sampled sinusoidal flickering
QVector<double> gen_flick_sin(double freq, int refreshRate, int length)
{

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

     //
}
}


