#include "randomflashsequence.h"
#include <QVector>
#include <QList>
#include <algorithm>
#include <random>
#include <time.h>
#include <QtDebug>
//using namespace std;

RandomFlashSequence::RandomFlashSequence(QObject *parent) : QObject(parent){}

RandomFlashSequence::RandomFlashSequence(int length, int nr_sequences, int min_dist, bool repetition)
{
    // srand(time(0));
    // srand(time(0));
    // qDebug()<< time(NULL);
    srand(time(NULL));
    QVector<int> list(length);
    QVector<int> l(length);

    // std::random_device rd;
    // std::mt19937 g(rd());

    std::random_device r;
    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
    std::mt19937 eng(seed);

    std::iota(list.begin(), list.end(), 1);
    // std::random_shuffle(list.begin(), list.end(), g); removed in c++17
    // std::shuffle(list.begin(), list.end(), g);
    std::shuffle(list.begin(), list.end(), eng);

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
                std::swap(l[0], l[2]);
            }
            else if(list.last()==l[1])
            {
                std::swap(l[1], l[2]);
            }
            else if(list[list.count()-1]==l[0])
            {
                std::swap(l[0], l[1]);
            }
        }
    }
    sequence = list;

}

RandomFlashSequence::RandomFlashSequence(int elements, int nr_sequences, int setCount, int setLength)
{
    // qDebug() << Q_FUNC_INFO;
    srand(time(0));
    QList<QList<int>> sequenceSets;
    QVector<int> l(elements / setLength);
    QVector<int> x(elements / setLength);
    // QVector<int> l(elements);
    // QVector<int> x(elements);
    QList<int> tmp;
    QList<int> tmp2;
    QVector<int> buff;
    QList<int> flash;

    for(int i=1; i<=setCount; i++)
    {
        sequenceSets.append(range(i, i+(elements-setLength), setLength));
    }

    // qDebug()<< Q_FUNC_INFO << sequenceSets;

    std::iota(l.begin(), l.end(), 1);
    std::random_shuffle(l.begin(), l.end());
    x = l;
    buff = x;
    for (int i=0; i<nr_sequences; i++)
    {
        for(int k=0; k<setLength; k++)
        {
            for(int j=0; j<setCount; j++)
            {
                if (tmp.size() == 0)
                {
                    tmp.append( sequenceSets[j].at(l[k]-1) );
                    x.removeAll(l[k]);
                }
                else
                {
                    tmp.append( sequenceSets[j].at(x.takeFirst()-1) );
                }
            }
            // append list of one sequence
            tmp2.append(tmp);
            x.resize(l.size());
            x[0] = l[k+1];
            tmp = buff.toList();
            tmp.removeAll(x[0]);
            std::swap(tmp.first(), tmp.last());
            for(int m=1; m<=x.size(); m++)
            {
                x[m] = tmp[m-1];
            }
            tmp.clear();
            buff = x;
        }
        std::random_shuffle(l.begin(), l.end());
        flash.append(tmp2);
        tmp2.clear();
        x = l;
        buff = x;
    }

    sequence = flash.toVector();
    // qDebug()<< Q_FUNC_INFO << sequence;
}

RandomFlashSequence::RandomFlashSequence(int nr_sequences)
{

    srand(time(0));
    QList<QList<int>> sequenceSets;
    QList<int> flash;
    int k = 0;
    for(int i=1; i<4; i++)
    {
        sequenceSets.append(range(i+k, i+k+2, 2));
        k += 2;
    }
    sequenceSets.append(QList<int>{2,8});
    sequenceSets.append(QList<int>{5,0});


    for(int i = 0; i<nr_sequences;i++)
    {
        std::random_shuffle(sequenceSets.begin(), sequenceSets.end());
        for(int j=0; j<sequenceSets.length();j++)
        {
            flash.append(sequenceSets[j]);
        }

    }

    sequence = flash.toVector();

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


