#ifndef INPUTMETHODEVENTCATCH_H
#define INPUTMETHODEVENTCATCH_H

#include <QObject>

class InputMethodEventCatch : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* target READ target WRITE setTarget NOTIFY targetChanged)

public:
    explicit InputMethodEventCatch(QObject *parent = nullptr);
    ~InputMethodEventCatch();

    QObject *target();
    void setTarget(QObject *target);

    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void targetChanged();
    void inputMethodEventPressed(int code, const QString &key);
    void inputMethodEventCommitted(const QString &commitString);

private:
    QObject *m_target = nullptr;
};

#endif // INPUTMETHODEVENTCATCH_H
