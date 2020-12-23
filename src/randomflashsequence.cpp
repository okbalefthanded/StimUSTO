#include "randomflashsequence.h"
#include <QVector>
#include <QList>
#include <algorithm>
#include <time.h>
//using namespace std;

RandomFlashSequence::RandomFlashSequence(QObject *parent) : QObject(parent)
{

}
RandomFlashSequence::RandomFlashSequence(int length, int nr_sequences, int min_dist, bool repetition)
{

    srand(time(0));
    QVector<int> list(length);
    QVector<int> l(length);

    std::iota(list.begin(), list.end(), 1);
    std::random_shuffle(list.begin(), list.end());
    l = list;

    for (int i=1; i<nr_sequences; i++)
    {
        if (length == 1)
        {
            list.append( l );
        }
        else {
            std::random_shuffle(l.begin(), l.end());
            list.append( l );

            if(list.last()==l.first())
            {

                std::swap(l[0],l[2]);

            }
            else if(list.last()==l[1])
            {

                std::swap(l[1],l[2]);

            }
            else if(list[list.count()-1]==l[0])
            {

                std::swap(l[0],l[1]);

            }
        }
    }
    sequence = list;

}

RandomFlashSequence::RandomFlashSequence(int elements, int nr_sequences, int setCount, int setLength)
{
    srand(time(0));
    QList<QList<int>> sequenceSets;

    QVector<int> l(elements);

    for(int i=1; i<=setCount; i++)
    {
        sequenceSets.append(range(i, i+(elements-setLength), setLength));
    }

    //std::iota(list.begin(), list.end(), 1);

    for (int i=0; i<nr_sequences; i++)
    {

    }
}

QVector<int> RandomFlashSequence::range(int start, int end)
{
    QVector<int> list(end-start+1);
    std::iota(list.begin(), list.end(), start);
    return list;
}

QList<int> RandomFlashSequence::range(int start, int end, int step)
{
    QList<int> list;
    for (int i = start; i <= end; i += step)
        list.push_back(i);
    return list;
}

QString RandomFlashSequence::toString()
{
    QString seqStr;

    for (int i=0;i<sequence.length();i++)
    {
        seqStr += QString::number(sequence[i]);
    }

    return seqStr;
}

QVector<int> RandomFlashSequence::toSequence(QString seqStr)
{
    QVector<int> intSeq(seqStr.length());

    for (int i=0;i<seqStr.length();i++)
    {
        intSeq[i] = seqStr.at(i).digitValue();
    }

    return intSeq;
}


