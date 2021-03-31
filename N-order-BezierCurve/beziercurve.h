#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(BezierCurvePrivate);

class BezierCurve : public QWidget
{
    Q_OBJECT

public:
    BezierCurve(QWidget *parent = nullptr);
    ~BezierCurve() override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    BezierCurvePrivate *d = nullptr;
};

#endif // BEZIERCURVE_H
