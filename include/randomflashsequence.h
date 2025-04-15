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
    RandomFlashSequence(int length, int nr_sequences, QString flash_type, int rows, int cols);
    RandomFlashSequence(int length, int nr_sequences, int min_dist=2, bool repetition=false); // single flash
    RandomFlashSequence(int elements, int nr_sequences, int setCount, int setLength); // parallel flash per set
    RandomFlashSequence(int nr_sequences); // dual stimulation

    //
    void SCSequence(int length, int nr_sequences, int min_dist=2, bool repetition=false);
    void RCSequence(int length, int nr_sequences, int min_dist=2, bool repetition=false, int rows=6, int cols=6);
    void RASPSequence(int length, int nr_sequences, int min_dist=2, bool repetition=false, int rows=6, int cols=6);

    //
    QVector<int> generateSequence(int length, int nr_sequences, int min_dist=2, bool repetition=false);
    inline QVector<int> range(int start, int end);
    inline QList<int> range(int start, int end, int step);
    QString toString();
    static QVector<int> toSequence(QString seqStr);
    QVector<int> sequence; // used for MarkerTag
    QVector<QVector <int>> sequenceSet; // used for element stimulation

protected:
    QVector<int> initSequence(int length, int nr_sequences, int min_dist=2, bool repetition=false);

};

#endif // RANDOMFLASHSEQUENCE_H
