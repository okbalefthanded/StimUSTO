#ifndef UTILS_H
#define UTILS_H

#include <QVector>
#include <QSize>
#include <QGuiApplication>
#include <QScreen>

namespace utils {

// rectangle flickering : ON/OFF
QVector<int> gen_flick(double freq, int refreshRate, float length);

// sampled sinusoidal flickering
QVector<double> gen_flick_sin(double freq, int refreshRate, int length);

//
qint8 sign(double value);

//
QSize getScreenSize();

}

#endif // UTILS_H
