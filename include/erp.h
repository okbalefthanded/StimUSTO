#ifndef ERP_H
#define ERP_H
//
#include "paradigm.h"
//
class ERP : public Paradigm
{

public:
    explicit ERP();
    explicit ERP(QString mode, QString control,
                 QString type, QString comm,
                 int dur, quint8 bDur,
                 quint8 nrSeq, QString phrase,
                 QString ip, QString sType,
                 QString fMode, QString lang);

    QVariant toVariant() const override;
    void fromVariant(const QVariant& variant) override;

    QString stimulationType() const;
    void setStimulationType(const QString &t_stimulationType);

    QString flashingMode() const;
    void setFlashingMode(const QString &t_flashingMode);

    QString language() const;
    void setLanguage(QString newLanguage);

    int rows() const;
    void setRows(int newRows);
    void setRows(QVariant newRows);

    int cols() const;
    void setCols(int newCols);
    void setCols(QVariant newCols);

    QVariant charColor() const;
    void setCharColor(const QVariant &newCharColor);

    QVariant backgroundColor() const;
    void setBackgroundColor(const QVariant &newBackgroundColor);

    QList<QVariant> stimuliFilePaths() const;
    void setStimuliFilePaths(const QList<QVariant> &newStimuliFilePaths);

    QVariant charFont() const;
    void setCharFont(const QVariant &newCharFont);

private:
    QString m_flashingMode;
    QString m_language;
    int m_rows;
    int m_cols;
    QVariant m_charColor;
    QVariant m_charFont;
    QVariant m_backgroundColor;
    QList<QVariant> m_stimuliFilePaths;

};
#endif // ERP_H
