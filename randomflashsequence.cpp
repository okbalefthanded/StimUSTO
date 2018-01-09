#include "randomflashsequence.h"
#include <QVector>
#include <algorithm>
#include <time.h>
using namespace std;

RandomFlashSequence::RandomFlashSequence(QObject *parent) : QObject(parent)
{

}
RandomFlashSequence::RandomFlashSequence(int length, int nr_sequences, int min_dist, bool repetition)
{

    srand(time(0));
    QVector<int> list(length);
    QVector<int> l(length);

    iota(list.begin(), list.end(), 1);
    random_shuffle(list.begin(), list.end());
    l = list;

    for (int i=1; i<nr_sequences; i++){
        if (length == 1)
        {
            list.append( l );
        }
        else {
            random_shuffle(l.begin(), l.end());
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

QVector<int> RandomFlashSequence::range(int start, int end)
{
    QVector<int> list(end-start+1);
    iota(list.begin(), list.end(), start);
    return list;
}


