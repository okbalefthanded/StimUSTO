#ifndef GLUTILS_H
#define GLUTILS_H
//
#include<QVector>
#include<QVector3D>

namespace glUtils {

static const qint8 VERTICES_PER_TRIANGLE = 3;
static const qint8 POINTS_PER_SQUARE = 4;
static const qint8 INDICES_PER_SQUARE = 6;
static const qint8 TRIANGLES_PER_SQUARE = 2;
static const int TUPLESIZE  = 3;
static const int MAX_ELEMENTS = 4;

}

namespace glColors {
static const QVector3D white = QVector3D(1.0f, 1.0f, 1.0f);
static const QVector3D gray = QVector3D(0.25f, 0.25f, 0.25f);
static const QVector3D red = QVector3D(1.0f, 0.0f, 0.0f);
static const QVector3D green = QVector3D(0.0f, 1.0f, 0.0f);
static const QVector3D orange = QVector3D(1.0f, 0.5f, 0.0f);
}

namespace refPoints
{
static const QVector<QVector3D> topPoints =
{
    QVector3D(-0.10f, 0.10f, 1.0f), QVector3D(-0.75f, 0.10f, 1.0f),
    QVector3D(0.55f, 0.10f, 1.0f), QVector3D(-0.10f, 0.75f, 1.0f),
    QVector3D(-0.10f, -0.55f, 1.0f)
};
}


#endif // GLUTILS_H
