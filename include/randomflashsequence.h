#ifndef RANDOMFLASHSEQUENCE_H
#define RANDOMFLASHSEQUENCE_H

#include <QObject>
#include <QList>
#include <QVector>
#include <numeric>

class RandomFlashSequence : public QObject
{
    Q_OBJECT
public:
    explicit RandomFlashSequence(QObject *parent = 0);
    RandomFlashSequence(int length, int nr_sequences, int min_dist=2, bool repetition=false); // single flash
    RandomFlashSequence(int elements, int nr_sequences, int setCount, int setLength); // parallel flash per set
    RandomFlashSequence(int nr_sequences); // dual stimulation

    inline QVector<int> range(int start, int end);
    inline QList<int> range(int start, int end, int step);
    QString toString();
    static QVector<int> toSequence(QString seqStr);
    QVector<int> sequence;

};

#endif // RANDOMFLASHSEQUENCE_H
