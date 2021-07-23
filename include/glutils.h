#ifndef GLUTILS_H
#define GLUTILS_H
//
#include<QVector>
#include<QVector3D>

namespace glUtils {

static const qint8 VERTICES_PER_TRIANGLE = 3;
static const qint8 POINTS_PER_SQUARE     = 4;
static const qint8 INDICES_PER_SQUARE    = 6;
static const qint8 TRIANGLES_PER_SQUARE  = 2;
static const int TUPLESIZE    = 3;
static const int MAX_ELEMENTS = 4;

}

namespace glColors {
static const QVector3D white  = QVector3D(1.0f, 1.0f, 1.0f);
static const QVector3D gray   = QVector3D(0.25f, 0.25f, 0.25f);
static const QVector3D red    = QVector3D(1.0f, 0.0f, 0.0f);
static const QVector3D green  = QVector3D(0.0f, 1.0f, 0.0f);
static const QVector3D blue   = QVector3D(0.0f, 0.0f, 1.0f);
static const QVector3D yellow = QVector3D(1.0f, 1.0f, 0.0f);
static const QVector3D orange = QVector3D(1.0f, 0.5f, 0.0f);
}

namespace refPoints
{
static const QVector<QVector3D> topPoints =
{

    /*    QVector3D(-0.10f, 0.10f, 1.0f), QVector3D(-0.68f, 0.10f, 1.0f),
                    QVector3D(0.50f, 0.10f, 1.0f), QVector3D(-0.10f, 0.70f, 1.0f),
                    QVector3D(-0.10f, -0.38f, 1.0f)
                        */

    // original points
    // QVector3D(-0.10f, 0.10f, 1.0f), QVector3D(-0.75f, 0.10f, 1.0f),
    QVector3D(-0.10f, 0.10f, 1.0f), QVector3D(-0.75f, 0.10f, 1.0f),

    // QVector3D(0.55f, 0.10f, 1.0f), QVector3D(-0.10f, 0.75f, 1.0f),
                                   // -.1, .7
    QVector3D(0.56f, 0.10f, 1.0f), QVector3D(-0.10f, 0.70f, 1.0f),

    //QVector3D(-0.10f, -0.45f, 1.0f)
    // -0.1,-0.5,
    QVector3D(-0.10f, -0.45f, 1.0f)

    /*
        // closer points- one line
        QVector3D(-0.10f, 0.15f, 1.0f), QVector3D(-0.85f, 0.15f, 1.0f),

        // QVector3D(0.55f, 0.10f, 1.0f), QVector3D(-0.10f, 0.75f, 1.0f),
        QVector3D(-0.35f, 0.15f, 1.0f), QVector3D(0.15f, 0.15f, 1.0f),

        //QVector3D(-0.10f, -0.45f, 1.0f)
        QVector3D(0.65f, 0.15f, 1.0f)
            */

};
}


#endif // GLUTILS_H
