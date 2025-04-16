#include "randomflashsequence.h"
#include <QVector>
#include <QList>
#include <algorithm>
#include <random>
#include <time.h>
#include <QtDebug>
#include "utils.h"
//using namespace std;

RandomFlashSequence::RandomFlashSequence(QObject *parent) : QObject(parent){}

RandomFlashSequence::RandomFlashSequence(int length, int nr_sequences, QString flash_type, int rows, int cols)
{

    if (flash_type == flashing_mode::SC)
    {
        SCSequence(length, nr_sequences);
    }
    else if(flash_type == flashing_mode::RC)

    {
        RCSequence(length, nr_sequences, 2, false, rows, cols);
    }

    else if(flash_type == flashing_mode::RASP)
    {
        RASPSequence(length, nr_sequences, 2, false, rows, cols);
    }
}

RandomFlashSequence::RandomFlashSequence(int length, int nr_sequences, int min_dist, bool repetition)
{
    // srand(time(0));
    // srand(time(0));
    // qDebug()<< time(NULL);
    srand(time(0));
    QVector<int> list(length);
    QVector<int> l(length);

    // std::random_device rd;
    // std::mt19937 g(rd());

    // std::random_device r;
    // std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
    // std::mt19937 eng(seed);

    std::iota(list.begin(), list.end(), 1);
    std::random_shuffle(list.begin(), list.end());
    // std::random_shuffle(list.begin(), list.end(), g); removed in c++17
    // std::shuffle(list.begin(), list.end(), g);
    // std::shuffle(list.begin(), list.end(), eng);

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

void RandomFlashSequence::SCSequence(int length, int nr_sequences, int min_dist, bool repetition)
{
    QVector<int> sequence = initSequence(length, nr_sequences, min_dist, repetition);

    for(int i=0; i< sequenceSet.length(); i++)
    {
        sequenceSet[i] = {sequence[i]};
    }
}

void RandomFlashSequence::RCSequence(int length, int nr_sequences, int min_dist, bool repetition, int rows, int cols)
{
    QVector<int> sequence = initSequence(length, nr_sequences, min_dist, repetition);

    for(int i=0; i< sequenceSet.length(); i++)
    {
        sequenceSet[i] = utils::indexToRowColumn(sequence[i], rows, cols); //FIXME
    }

}

void RandomFlashSequence::RASPSequence(int length, int nr_sequences, int min_dist, bool repetition, int rows, int cols)
{
    srand(time(0));
    QVector<int> sequence = initSequence(length, nr_sequences, min_dist, repetition);
    int elements = rows * cols;
    int min = 1;
    QVector<QVector<int>> grid(rows);
    QVector<int> indices;

    for (int i = min; i <= elements; ++i)
    {
        indices.append(i);
    }

    // Shuffle the possible values randomly
    std::random_shuffle(indices.begin(), indices.end());


    int valueIndex = 0;
    for (int i = 0; i < rows; ++i)
    {
        grid[i].resize(cols);
        for (int j = 0; j < cols; ++j)
        {
            grid[i][j] = indices[valueIndex++];
        }
    }

    for(int i=0; i< sequenceSet.length(); i++)
    {
        if (sequence[i] <= rows)
        {
            sequenceSet[i] = grid[sequence[i] - 1];
        }
        else
        {
            QVector<int> tmp;
            for(int k=0; k<rows; ++k)
            {
                tmp.append(grid[k][sequence[i] - (rows+1)]);
            }
            sequenceSet[i] = tmp;
        }
    }
}


QVector<int> RandomFlashSequence::generateSequence(int length, int nr_sequences, int min_dist, bool repetition)
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

    return list;
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

QVector<int> RandomFlashSequence::initSequence(int length, int nr_sequences, int min_dist, bool repetition)
{
    QVector<int> sequence = generateSequence(length, nr_sequences, min_dist, repetition);
    this->sequence = sequence;
    sequenceSet.resize(sequence.length());
    return sequence;
}


