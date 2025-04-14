#include <QDebug>
#include "erp.h"
#include "utils.h"
#include "randomflashsequence.h"

ERP::ERP() : Paradigm (), m_flashingMode(flashing_mode::SC){}

ERP::ERP(QString mode, QString control,
         QString type, QString comm,
         int dur, quint8 bDur,
         quint8 nrSeq, QString phrase,
         QString ip, QString sType,
         QString fMode, QString lang):
    Paradigm(mode, control, type, comm, dur, bDur, nrSeq, sType, phrase, ip),
    m_flashingMode(fMode), m_language(lang) {}

QVariant ERP::toVariant() const
{
    QVariantMap map;

    // speller settings
    map.insert("language", m_language);
    map.insert("rows", m_rows);
    map.insert("columns", m_cols);
    map.insert("background_color", m_backgroundColor);
    map.insert("character_color", m_charColor);
    map.insert("character_font", m_charFont);
    map.insert("stimuli_files", m_stimuliFilePaths);

    // paradigm settings
    map.insert("experimentMode", m_experimentMode);
    map.insert("controlMode", m_controlMode);
    map.insert("paradigmType", m_type);
    map.insert("stimulationDuration", m_stimulationDuration);
    map.insert("breakDuration", m_breakDuration);
    map.insert("nrSequences", m_nrSequences);
    map.insert("desiredPhrase", m_desiredPhrase);
    map.insert("stimulationType", m_stimulationType);
    map.insert("flashingMode", m_flashingMode);

    // external communication settings
    map.insert("externalComm", m_externalComm);
    map.insert("ip", m_externalAddress);

    return map;
}

void ERP::fromVariant(const QVariant &variant)
{
    int n_elements = 9;
    // QVariant dummyValue;

    QVariantMap map = variant.toMap();

    m_map = QVariantMap(variant.toMap());

    m_experimentMode = map.value("experimentMode").toString().toUpper();
    m_controlMode    = map.value("controlMode").toString().toUpper();

    if(map.value("externalComm").isNull())
    {
        m_externalComm    = external_comm::DISABLED;
        m_externalAddress = "127.0.0.1"; // home sweet home
    }
    else
    {
        m_externalComm    = map.value("externalComm").toString().toUpper();
        m_externalAddress = map.value("ip").toString();
    }
    m_type = map.value("paradigmType").toString().toUpper();
    m_stimulationDuration = map.value("stimulationDuration").toInt();
    m_breakDuration = map.value("breakDuration").toInt();
    m_nrSequences   = map.value("nrSequences").toInt();

    m_desiredPhrase   = map.value("desiredPhrase").toString();
    m_stimulationType = map.value("stimulationType").toString().toUpper();


    if (m_stimulationType == speller_type::AUDITORY)
    {
        n_elements = 5;
    }
    else if (m_stimulationType >= speller_type::SMALL && m_stimulationType <= speller_type::SMALL_CIRCLE)
    {
        n_elements = 6;
    }

    if(m_controlMode == control_mode::ASYNC)
    {
        ++n_elements;
    }


    if(m_desiredPhrase.isEmpty())
    {
        RandomFlashSequence *randomPhrase;

        if (m_experimentMode == operation_mode::CALIBRATION)
        {
            // randomPhrase = new RandomFlashSequence(9, 4);
            randomPhrase = new RandomFlashSequence(n_elements, 4);
            // randomPhrase = new RandomFlashSequence(9, 2);
            // randomPhrase = new RandomFlashSequence(9, 1);

            // RandomFlashSequence *randomPhrase = new RandomFlashSequence(9, 2);
            // RandomFlashSequence *randomPhrase = new RandomFlashSequence(9, 4);
            // RandomFlashSequence *randomPhrase = new RandomFlashSequence(9, 3);
        }
        else
        {
            //  randomPhrase = new RandomFlashSequence(9, 6);
            randomPhrase = new RandomFlashSequence(n_elements, 4);
            //  randomPhrase = new RandomFlashSequence(n_elements, 6);
        }

        m_desiredPhrase = randomPhrase->toString();
    }

    else
    {
        m_desiredPhrase = m_desiredPhrase.toUpper();
    }

    m_flashingMode = map.value("flashingMode").toString().toUpper();

    setRows(map.value("rows"));
    setCols(map.value("columns"));
    setCharColor(map.value("character_color"));
    setCharFont(map.value("character_font"));
    setBackgroundColor(map.value("background_color"));

    if(map.value("language").isNull())
    {
        m_language = speller_language::ENGLISH;
    }
    else
    {
        m_language = map.value("language").toString().toUpper();
    }

    m_stimuliFilePaths = map.value("stimuli_files").toList();
}


QString ERP::stimulationType() const
{
    return m_stimulationType;
}

void ERP::setStimulationType(const QString &t_stimulationType)
{
    m_stimulationType = t_stimulationType;
}

QString ERP::flashingMode() const
{
    return m_flashingMode;
}

void ERP::setFlashingMode(const QString &t_flashingMode)
{
    m_flashingMode = t_flashingMode;
}

QString ERP::language() const
{
    return m_language;
}

void ERP::setLanguage(QString newLanguage)
{
    m_language = newLanguage;
}

int ERP::rows() const
{
    return m_rows;
}

void ERP::setRows(int newRows)
{
    m_rows = newRows;
}

void ERP::setRows(QVariant newRows)
{
    m_rows = newRows.isNull() ? 6 : newRows.toInt();
}

int ERP::cols() const
{
    return m_cols;
}

void ERP::setCols(int newCols)
{
    m_cols = newCols;
}

void ERP::setCols(QVariant newCols)
{
    m_cols = newCols.isNull() ? 6 : newCols.toInt();
}

QVariant ERP::charColor() const
{
    return m_charColor;
}

void ERP::setCharColor(const QVariant &newCharColor)
{
    m_charColor = newCharColor.isNull() ? "gray": newCharColor;
}

QVariant ERP::backgroundColor() const
{
    return m_backgroundColor;
}

void ERP::setBackgroundColor(const QVariant &newBackgroundColor)
{
    m_backgroundColor = newBackgroundColor.isNull() ? "black": newBackgroundColor;
}

QList<QVariant> ERP::stimuliFilePaths() const
{
    return m_stimuliFilePaths;
}

void ERP::setStimuliFilePaths(const QList<QVariant> &newStimuliFilePaths)
{
    if(newStimuliFilePaths.isEmpty())
    {
        m_stimuliFilePaths.append(":/images/bennabi_face_red.png");
    }
    else
    {
        m_stimuliFilePaths = newStimuliFilePaths;
    }
}

QVariant ERP::charFont() const
{
    return m_charFont;
}

void ERP::setCharFont(const QVariant &newCharFont)
{
    m_charFont = newCharFont.isNull() ? 40: newCharFont;
}
