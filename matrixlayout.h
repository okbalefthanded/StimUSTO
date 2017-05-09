#ifndef MATRIXLAYOUT_H
#define MATRIXLAYOUT_H

#include <QObject>
#include <QPair>


class MatrixLayout : public QObject
{
    Q_OBJECT
public:
    explicit MatrixLayout(QObject *parent = 0);
    MatrixLayout(QPair<int, int> size, int rows, int cols);

signals:

public slots:

private:
    QPair<int, int> size;
    QList<QPair<int, int>> positions;
    int rows;
    int cols;
};

#endif // MATRIXLAYOUT_H
