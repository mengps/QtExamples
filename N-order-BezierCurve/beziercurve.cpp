#include "beziercurve.h"


#include <QtMath>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

/**
 * @brief createNBezierCurve 生成N阶贝塞尔曲线点
 * @param src 源贝塞尔控制点
 * @param dest 目的贝塞尔曲线点
 * @param precision 生成精度
 */
static void createNBezierCurve(const QList<QPointF> &src, QList<QPointF> &dest, qreal precision)
{
    if (src.size() <= 0) return;

    //清空
    QList<QPointF>().swap(dest);

    for (qreal t = 0; t < 1.0000; t += precision) {
        int size = src.size();
        QVector<qreal> coefficient(size, 0);
        coefficient[0] = 1.000;
        qreal u1 = 1.0 - t;

        for (int j = 1; j <= size - 1; j++) {
            qreal saved = 0.0;
            for (int k = 0; k < j; k++){
                qreal temp = coefficient[k];
                coefficient[k] = saved + u1 * temp;
                saved = t * temp;
            }
            coefficient[j] = saved;
        }

        QPointF resultPoint;
        for (int i = 0; i < size; i++) {
            QPointF point = src.at(i);
            resultPoint = resultPoint + point * coefficient[i];
        }

        dest.append(resultPoint);
    }
}

class BezierCurvePrivate
{
public:
    BezierCurvePrivate() = default;

    //完成控制点构成
    bool m_completed = false;
    bool m_mousePressed = false;
    int m_currentControlPointIndex = -1;
    qreal m_precision = 0.1;
    QList<QPointF> m_controlPoints;
    QList<QPointF> m_bezierCurve;
};

BezierCurve::BezierCurve(QWidget *parent)
    : QWidget(parent)
{
    d = new BezierCurvePrivate;

    QComboBox *comboBox = new QComboBox(this);
    connect(comboBox, QOverload<int>::of(&QComboBox::activated), this, [=](int index) {
        d->m_precision = comboBox->itemText(index).toDouble();
        createNBezierCurve(d->m_controlPoints, d->m_bezierCurve, d->m_precision);
        update();
    });
    comboBox->addItem(QString::number(0.1));
    comboBox->addItem(QString::number(0.01));
    comboBox->addItem(QString::number(0.001));
    QLabel *label = new QLabel("精度:", this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(comboBox);

    QWidget *widget = new QWidget(this);
    widget->resize(150, 50);
    widget->setLayout(layout);

    resize(800, 600);
}

BezierCurve::~BezierCurve()
{
    if (d) delete d;
}

void BezierCurve::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.save();
    painter.setBrush(QBrush(Qt::red));
    QFontMetrics metrics(painter.font());
    for (auto i = 0; i < d->m_controlPoints.size(); i++) {
        painter.setPen(Qt::red);
        painter.drawEllipse(d->m_controlPoints.at(i), 10.0, 10.0);
        painter.setPen(Qt::white);
        QString number = QString::number(i);
        auto rect = metrics.boundingRect(number);
        painter.drawText(d->m_controlPoints.at(i) + QPointF(-rect.width() / 2, rect.height() / 2 - 1.0), number);
    }
    painter.restore();

    if (d->m_controlPoints.size() >= 2) {
        QPainterPath curve;
        curve.moveTo(d->m_bezierCurve.at(0));
        for (auto i = 1; i < d->m_bezierCurve.size(); i++) {;
            curve.lineTo(d->m_bezierCurve.at(i));
        }
        auto pen = painter.pen();
        pen.setColor(Qt::blue);
        pen.setWidth(2.0);
        painter.setPen(pen);
        painter.drawPath(curve);
    }
}

void BezierCurve::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && event->pos().y() > 50) {
        d->m_mousePressed = true;
        if (d->m_completed) {
            d->m_currentControlPointIndex = -1;
            auto pos = event->pos();
            for (auto i = 0; i < d->m_controlPoints.size(); i++) {
                auto point = d->m_controlPoints.at(i);
                //判断是否在控制点上
                if (qAbs(qSqrt(qPow(pos.x() - point.x(), 2) + qPow(pos.y() - point.y(), 2))) < 10.0000) {
                    d->m_currentControlPointIndex = i;
                    break;
                }
            }
        } else {
            d->m_controlPoints.append(event->pos());
            //控制点增加了，需要更新
            createNBezierCurve(d->m_controlPoints, d->m_bezierCurve, d->m_precision);
            update();
        }
    } else if (event->buttons() & Qt::RightButton) {
        d->m_completed = true;
    }
}

void BezierCurve::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton
            && d->m_mousePressed
            && d->m_currentControlPointIndex != -1
            && event->pos().y() > 50) {
        if (d->m_currentControlPointIndex < d->m_controlPoints.size()) {
            d->m_controlPoints[d->m_currentControlPointIndex] = event->pos();
            createNBezierCurve(d->m_controlPoints, d->m_bezierCurve, d->m_precision);
            update();
        }
    }
}

void BezierCurve::mouseReleaseEvent(QMouseEvent *event)
{
    d->m_currentControlPointIndex = -1;
    if (event->buttons() & Qt::LeftButton) {
        d->m_mousePressed = false;
    }
}

void BezierCurve::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        d->m_completed = false;
        QList<QPointF>().swap(d->m_controlPoints);
        QList<QPointF>().swap(d->m_bezierCurve);
        update();
    }
}
