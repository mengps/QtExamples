#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QTextEdit>
#include <QTimer>

static void initializeDebugEnveriment()
{
    static bool initialized = false;
    static QTextEdit *edit = new QTextEdit;
    static int lineCount = 0;
    static const QString stdoutFileDir = qApp->applicationDirPath() + "/cache";

    static QTimer *watcher = new QTimer(qApp);
    static quint64 fileSize = 0;
    static QFile watchedStdoutFile(stdoutFileDir + "/stdout");

    if (!initialized) {
        qRegisterMetaType<QTextCursor>("QTextCursor");

        auto palette = edit->palette();
        palette.setBrush(QPalette::Highlight, QColor(0, 120, 215));
        edit->setPalette(palette);
        edit->setReadOnly(true);
        edit->setWindowTitle(QStringLiteral("调试窗口"));
        edit->setWindowFlag(Qt::WindowStaysOnTopHint);
        edit->resize(700, 500);
        edit->show();

        if (!QDir().exists(stdoutFileDir)) QDir().mkpath(stdoutFileDir);

        std::freopen((stdoutFileDir + "/stdout").toLocal8Bit().data(), "w", stdout);
        watchedStdoutFile.open(QIODevice::ReadOnly);

        watcher->start(100);
        QObject::connect(watcher, &QTimer::timeout, watcher, []{
            if (watchedStdoutFile.size() != fileSize) {
                fileSize = watchedStdoutFile.size();
                auto watchedMsg = QString::fromLocal8Bit(watchedStdoutFile.readAll());
                if (!watchedMsg.isEmpty()) {
                    auto list = watchedMsg.split('\n');
                    for (auto msg: qAsConst(list)) {
                        msg = msg.trimmed();
                        auto time = QDateTime::currentDateTime().toString("[yyyy-MM-dd-hh:mm:ss:zzz] ");
                        if (!msg.isEmpty()) msg = time + msg;
                        edit->append(msg);
                        if (!edit->textCursor().hasSelection()) edit->moveCursor(QTextCursor::End);
                        if (++lineCount > 50000) {
                            lineCount = 0;
                            edit->clear();
                        }
                    }
                }
            }
        });

        initialized = true;
    }

    static auto myMsgHandler = [](QtMsgType, const QMessageLogContext &, const QString &msg) -> void {
        auto time = QDateTime::currentDateTime().toString("[yyyy-MM-dd-hh:mm:ss:zzz] ");
        edit->append(time + msg);
        if (!edit->textCursor().hasSelection()) edit->moveCursor(QTextCursor::End);
        if (++lineCount > 50000) {
            lineCount = 0;
            edit->clear();
        }
    };

    qInstallMessageHandler(myMsgHandler);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    initializeDebugEnveriment();

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &timer, []{
        static int count = 1;
        qDebug() << "This is Qt Debug message! Count:" << count++;
    });
    timer.start(1000);

    QTimer otherTimer;
    QObject::connect(&otherTimer, &QTimer::timeout, &otherTimer, []{
        static int count = 1;
        printf("This is printf stdout message! Count: %d", count++);
        fflush(stdout);
    });
    otherTimer.start(1000);

    return app.exec();
}
