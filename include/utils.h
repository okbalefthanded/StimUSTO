#ifndef UTILS_H
#define UTILS_H

#include <QtMath>
#include <QVector>
#include <QSize>
#include <QGuiApplication>
#include <QScreen>

namespace utils {
//
static QChar letters[6][6] = {
    {'A','B','C','D','E','F'},
    {'G','H','I','J','K','L'},
    {'M','N','O','Q','P','R'},
    {'S','T','U','V','W','X'},
    {'Y','Z','0','1','2','3'},
    {'4','5','6','7','8','9'},
    };

static QString ArabicLetters = "أبتثجحخدذرزسشصضطظغفقكلمنهوي";
// const static QString ArabicLetters = "دخحجثتبأطضصشسزرذنملكقفغظيوه";
const static QString numbers = "0123456789<_>";

// rectangle flickering
QVector<double> gen_flick(double freq, int refreshRate, float length, quint8 stimMode, double phase=0.0);

// rectangle flickering : ON/OFF
QVector<double> gen_flick_on_off(double freq, int refreshRate, float length);

// sampled sinusoidal flickering // joint frequency-phase modulation (JFPM)
QVector<double> gen_flick_sin(double freq, int refreshRate, float length, double phase);

void wait(int millisecondsToWait);

//
qint8 sign(double value);

//
QSize getScreenSize();

int screenCount();

int indexToGridCoordinates(int index, int gridSize);

QVector<int> indexToRowColumn(int index, int rows, int cols);

}
//
namespace operation_mode {
// operation modes (experiment mode)
const QString CALIBRATION  = QStringLiteral("CALIBRATION");
const QString COPY_MODE    = QStringLiteral("COPY");
const QString FREE_MODE    = QStringLiteral("FREE");
const QString SSVEP_SINGLE = "3";
}


namespace paradigm_type {
const QString ERP    = QStringLiteral("ERP");
const QString SSVEP  = QStringLiteral("SSVEP");
const QString HYBRID = QStringLiteral("HYBRID");
const QString DOUBLE_ERP   = QStringLiteral("DOUBLE_ERP");
const QString DOUBLE_SSVEP = QStringLiteral("DOUBLE_SSVEP");
}

//
namespace flashing_mode {
const QString SC   = QStringLiteral("SINGLE CHARACTER"); // single character
const QString RC   = QStringLiteral("ROW COLUMN"); // row-column
const QString RASP = QStringLiteral("RASP"); //
}
namespace speller_type {
// speller types
// stimulation type
const QString FLASH = QStringLiteral("FLASH");
const QString SINGLE = QStringLiteral("SINGLE STIMULUS");
const QString MULTIPLE = QStringLiteral("MULTI STIMULUS");
//
const QString FLASHING_SPELLER = QStringLiteral("FLASHING_SPELLER");
const QString FACES_SPELLER = QStringLiteral("FACES_SPELLER");
const QString INVERTED_FACE = QStringLiteral("INVERTED_FACE");
const QString COLORED_FACE = QStringLiteral("COLORED_FACE");
const QString INVERTED_COLORED_FACE = QStringLiteral("INVERTED_COLORED_FACE");
const QString SSVEP_DIRECTIONS = QStringLiteral("SSVEP_DIRECTIONS");
const QString ARABIC_SPELLER = QStringLiteral("ARABIC_SPELLER");
const QString MULTI_STIM = QStringLiteral("MULTI_STIM");
const QString DUAL_STIM = QStringLiteral("DUAL_STIM");
const QString MISMATCH = QStringLiteral("MISMATCH");
const QString CHROMA = QStringLiteral("CHROMA");
const QString AUDITORY = QStringLiteral("AUDITORY");
const QString SSVEP_CIRCLE = QStringLiteral("SSVEP_CIRCLE");
const QString SMALL = QStringLiteral("SMALL"); // upright colored faces
const QString SMALL_FLASH = QStringLiteral("SMALL_FLASH");
const QString SMALL_FACE = QStringLiteral("SMALL_FACE");
const QString SMALL_IFACE = QStringLiteral("SMALL_IFACE"); // inverted face
const QString SMALL_ICFACE = QStringLiteral("SMALL_ICFACE"); // inverted colored face
const QString SMALL_SR = QStringLiteral("SMALL_SR"); // stim color for each
const QString SMALL_CIRCLE = QStringLiteral("SMALL_CIRCLE"); // cricular layout, needs a refactor
const QString SSVEP_DIRCIRCLE = QStringLiteral("SSVEP_DIRCIRCLE"); // cricular layout, needs a refactor
const QString CIRC_DIR = QStringLiteral("CIRC_DIR"); // 8 directions erp speller in circular layout
const QString SSVEP_GRID = QStringLiteral("SSVEP_GRID"); // ssvep cicrles in a grid layoutspeller
const QString TEST = QStringLiteral("TEST"); // testing the refactor
//const QString MOTION_BAR = QStringLiteral("MOTION_BAR");
//const QString MOTION_FACE = QStringLiteral("MOTION_FACE");
//const QString MOVING_FACE = QStringLiteral("MOVING_FACE");
}

namespace speller_language
{
const QString ENGLISH = QStringLiteral("ENGLISH");
const QString ARABIC  = QStringLiteral("DZARABIC");
// TODO
// Middle East Keyboard
// other languages
}

namespace control_mode {
// control mode : synchronous / asynchronous
const QString SYNC  = QStringLiteral("SYNC");
const QString ASYNC = QStringLiteral("ASYNC");
}

namespace trial_state {
// trial states
const quint8 PRE_TRIAL     = 0;
const quint8 STIMULUS      = 1;
const quint8 POST_STIMULUS = 2;
const quint8 FEEDBACK      = 3;
const quint8 POST_TRIAL    = 4;
}
//
namespace frequency_stimulation {
const quint8 ON_OFF = 0;
const quint8 SIN    = 1; // only frequency
const quint8 SIN_FP = 2; // joint frequency-phase modulation
}

namespace config {
//

const QString ovTCPTagPort = "15631";
const int start_port = 54321;
//
const uint64_t OVTK_StimulationLabel_Base = 0x00008100;
const int REFRESH_RATE = 60;
// const double PHASE = 0.35*M_PI;
const double PHASE     = 0.5 * M_PI; // M_PI;
}

namespace order {
const quint8 ERP_FIRST   = 0;
const quint8 SSVEP_FIRST = 1;
}

namespace external_comm {
const QString DISABLED = QStringLiteral("DISABLED");
const QString ENABLED  = QStringLiteral("ENABLED");
}

namespace machine_state {
const quint8 MOVING = 0;
const quint8 READY  = 1;
}


#endif // UTILS_H
