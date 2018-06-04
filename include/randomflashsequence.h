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
    RandomFlashSequence(int length, int nr_sequences, int min_dist=2, bool repetition=false);

    inline QVector<int> range(int start, int end);
    QVector<int> sequence;

};

#endif // RANDOMFLASHSEQUENCE_H
