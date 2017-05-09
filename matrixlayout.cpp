#include "matrixlayout.h"

MatrixLayout::MatrixLayout(QObject *parent) : QObject(parent)
{

}

MatrixLayout::MatrixLayout(QPair<int, int> size, int rows, int cols)
{

    // init
    this->size.first = size.first;
    this->size.second = size.second;
    this->rows = rows;
    this->cols = cols;

    int distx, disty;
    int x, y;

    int width = size.first;
    int height = size.second;

    distx = width / (cols - 1);
    disty = height / (rows - 1);

    for (int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
            x = round(distx * j - width / 2);
            y = round(disty * i - height / 2);
            positions.append(qMakePair(x, y));

            }
    }



}

