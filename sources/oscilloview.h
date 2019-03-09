#ifndef OSCILLOVIEW_H
#define OSCILLOVIEW_H

#include <QtCharts/QChartView>
#include <QGraphicsView>
#include <QOpenGLWidget>

#include "cursor.h"

class OscilloChart;

QT_CHARTS_USE_NAMESPACE

class OscilloView : public QGraphicsView
{
    Q_OBJECT

public:
    enum Mode{Zoom, Ruler};

    OscilloView(OscilloChart *chart, QWidget *parent = 0);
    void setMode(Mode mode);
    void save();
    void setAntialiasing(bool antialias);
    void setTrigPos(int pos);
    void updateY1Pos(qreal pos);
    Cursor *trigCursor();

signals:
    void leaveFullScreen();
    void changeMode(int mode);

protected:
    bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    virtual void drawForeground(QPainter *painter, const QRectF &rect);
    virtual void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawXValues(QPainter *painter, qreal valx);

    OscilloChart *m_oscilloChart;
    Mode m_mode;
    bool m_isRuling;
    bool m_isDragging;
    bool m_isRubberBanding;
    QPoint m_startPoint;
    QPoint m_newPoint;
    QOpenGLWidget *m_GLwidget;
    Cursor *m_trigCursor;
    int m_lastTrigPos;
    Cursor *m_yCursor1;
    qreal m_lastY1Pos;
};

#endif // OSCILLOVIEW_H
