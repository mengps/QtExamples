#include "inputmethodeventcatch.h"

#include <QApplication>
#include <QDebug>
#include <QTextEdit>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextEdit edit;
    InputMethodEventCatch editCatch;
    QObject::connect(&editCatch, &InputMethodEventCatch::inputMethodEventPressed, &editCatch, [](int code, const QString &key){
        qDebug() << "inputMethodEventPressed" << Qt::Key(code) << key;
    });
    QObject::connect(&editCatch, &InputMethodEventCatch::inputMethodEventCommitted, &editCatch, [](const QString &commitString){
        qDebug() << "inputMethodEventCommitted" << commitString;
    });
    editCatch.setTarget(&edit);
    edit.show();

    return app.exec();
}
