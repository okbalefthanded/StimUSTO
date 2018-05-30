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
    ~MatrixLayout();
    QList<QPair<int, int>> positions;
signals:

public slots:

private:
    QPair<int, int> size;

    int rows;
    int cols;
};

#endif // MATRIXLAYOUT_H
