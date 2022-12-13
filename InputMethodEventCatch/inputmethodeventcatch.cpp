#include "inputmethodeventcatch.h"

#include <QInputMethodEvent>

static int key2code(const QChar &key)
{
    switch (key.toLatin1())
    {
    case 'q': case 'Q': return Qt::Key_Q;
    case 'w': case 'W': return Qt::Key_W;
    case 'e': case 'E': return Qt::Key_E;
    case 'r': case 'R': return Qt::Key_R;
    case 't': case 'T': return Qt::Key_T;
    case 'y': case 'Y': return Qt::Key_Y;
    case 'u': case 'U': return Qt::Key_U;
    case 'i': case 'I': return Qt::Key_I;
    case 'o': case 'O': return Qt::Key_O;
    case 'p': case 'P': return Qt::Key_P;
    case 'a': case 'A': return Qt::Key_A;
    case 's': case 'S': return Qt::Key_S;
    case 'd': case 'D': return Qt::Key_D;
    case 'f': case 'F': return Qt::Key_F;
    case 'g': case 'G': return Qt::Key_G;
    case 'h': case 'H': return Qt::Key_H;
    case 'j': case 'J': return Qt::Key_J;
    case 'k': case 'K': return Qt::Key_K;
    case 'l': case 'L': return Qt::Key_L;
    case 'z': case 'Z': return Qt::Key_Z;
    case 'x': case 'X': return Qt::Key_X;
    case 'c': case 'C': return Qt::Key_C;
    case 'v': case 'V': return Qt::Key_V;
    case 'b': case 'B': return Qt::Key_B;
    case 'n': case 'N': return Qt::Key_N;
    case 'm': case 'M': return Qt::Key_M;
    }

    return Qt::Key_unknown;
}

InputMethodEventCatch::InputMethodEventCatch(QObject *parent)
    : QObject{parent}
{

}

InputMethodEventCatch::~InputMethodEventCatch()
{
    if (m_target) m_target->removeEventFilter(this);
}

QObject *InputMethodEventCatch::target()
{
    return m_target;
}

void InputMethodEventCatch::setTarget(QObject *target)
{
    if (!target) return;

    if (m_target != target) {
        if (m_target) m_target->removeEventFilter(this);
        target->installEventFilter(this);
        m_target = target;
        emit targetChanged();
    }
}

bool InputMethodEventCatch::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::InputMethod) {
        QInputMethodEvent *input = static_cast<QInputMethodEvent *>(event);
        if (input->preeditString().isEmpty()) {
            emit inputMethodEventCommitted(input->commitString());
        } else {
            QString key;
            for (const auto &attr: input->attributes()) {
                if (attr.type == QInputMethodEvent::AttributeType::Cursor && attr.start > 0) {
                    key = input->preeditString().mid(attr.start - 1, attr.length);
                    break;
                }
            }
            if (key.size() == 1)
                emit inputMethodEventPressed(key2code(*key.begin()), key);
        }
    }
    return QObject::eventFilter(obj, event);
}
