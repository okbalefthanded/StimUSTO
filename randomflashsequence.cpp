#include "randomflashsequence.h"
#include <QVector>
#include <algorithm>
#include <time.h>
using namespace std;

RandomFlashSequence::RandomFlashSequence(QObject *parent) : QObject(parent)
{

}
RandomFlashSequence::RandomFlashSequence(int length,int nr_sequences, int min_dist, bool repetition)
{

    srand(time(0));
    QVector<int> list(length);
    QVector<int> l(length);

    iota(list.begin(), list.end(), 1);
    random_shuffle(list.begin(), list.end());
    l = list;

    for (int i=1; i<nr_sequences; i++){
        random_shuffle(l.begin(), l.end());
        list.append( l );
    }
    sequence = list;

}

QVector<int> RandomFlashSequence::range(int start, int end)
{
    QVector<int> list(end-start+1);
    iota(list.begin(), list.end(), start);
    return list;
}


