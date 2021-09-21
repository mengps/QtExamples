#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QThread>

#include <thread>

class TimerTask
{
public:
    TimerTask()
    {
        m_thread.start();
    }

    ~TimerTask()
    {
        for (auto &timer: m_timers) {
            if (timer->isActive()) timer->stop();
            timer->deleteLater();
        }

        m_thread.quit();
        m_thread.wait();
    }

    template<typename Task>
    void addTask(Task task, int interval, bool repeat = true)
    {
        QTimer *timer = new QTimer;
        timer->moveToThread(&m_thread);
        timer->setInterval(interval);
        timer->setSingleShot(!repeat);
        QObject::connect(timer, &QTimer::timeout, task);

        if (!repeat)
            QObject::connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
        else
            m_timers.append(timer);

        QMetaObject::invokeMethod(timer, "start");
    }

private:
    QThread m_thread;
    QList<QTimer *> m_timers;
};

void printCurrentThreadId() {
    qDebug() << "TimerTask thread id =" << QThread::currentThreadId();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    std::thread thread([]{
        qDebug() << "std::thread id =" << QThread::currentThreadId();
        QTimer timer;
        timer.setInterval(1000);
        QObject::connect(&timer, &QTimer::timeout, []{
            qDebug() << "std::thread 1000 ms time out";
        });
        QEventLoop eventLoop;
        timer.start();
        eventLoop.exec();
    });

    TimerTask tasks;
    tasks.addTask([]{ qDebug() << "1000 ms time out"; }, 1000, true);
    tasks.addTask([]{ qDebug() << "2000 ms time out"; }, 2000, true);
    tasks.addTask(&printCurrentThreadId, 0, false);

    qDebug() << "main app thread id =" << QThread::currentThreadId();

    while (true) {
        QThread::msleep(1000);
    }
}
