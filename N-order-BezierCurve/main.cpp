#include "beziercurve.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    BezierCurve bezierWidget;
    bezierWidget.show();

    return app.exec();
}
