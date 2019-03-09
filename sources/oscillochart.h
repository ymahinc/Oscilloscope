#ifndef OSCILLOCHART_H
#define OSCILLOCHART_H

#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QCategoryAxis>
#include <QTimer>
#include <QElapsedTimer>

class MainWindow;

QT_CHARTS_USE_NAMESPACE

class OscilloChart : public QChart
{
    Q_OBJECT

public:
    enum Interpolation{Line, Sin, Dot};
    enum Modes{Classic, Maths};
    enum MathModes{Add,Substract,XY,FFT1,FFT2};

    OscilloChart(QGraphicsItem *parent = Q_NULLPTR, Qt::WindowFlags wFlags = Qt::WindowFlags());
    void zoomRect(QRectF rect);
    void scrollBy(qreal x, qreal y);
    void setMode(Modes mode);
    void setMathMode(MathModes mathMode);
    Modes mode();
    MathModes mathMode();
    int Y1Res();
    int Y2Res();
    qreal valueFromGraph(qreal value, int axis);
    QColor canalColor(int canal);
    int triggerLevel();
    void triggerLevelUpdatedFromView(int val);

public slots:
    void updateData(QByteArray data,int canal);
    void setCanalColor(int canal, QColor color);
    void setCanalVisible(int canal, bool visible);
    void setInterpolation(Interpolation mode);
    void setTimePerDiv(int microsec);
    void setmVPerDiv(int canal, int mv);
    void setTriggerLevel(int level);
    void zoomInSlot();
    void zoomOutSlot();
    void resetZoomSlot();
    void invertCanal1(bool invert);
    void invertCanal2(bool invert);
    void updateMathMode(int mode);
    void updateDeltaX1(int delta);
    void updateDeltaX2(int delta);
    void updateDeltaY1(int delta);
    void updateDeltaY2(int delta);
    void updateView();

signals:
    void mathModeUpdated();
    void zoomUpdated();
    void triggerLevelUpdated(int value);

private:
    void createGridTicks();
    void setAxisResolution(QCategoryAxis *axis, int value);
    void setXAxisResolution(int value);
    void setYAxisResolution(QCategoryAxis *axis, int value);
    void setCanalInverted(int canal, bool inverted);
    void updateDataAddFunction();
    void updateDataSubstFunction();
    void updateDataXYFunction();
    void updateDataFFTFunction();

    QXYSeries *m_canal1;
    QXYSeries *m_canal2;
    QXYSeries *m_canal3;
    QLineSeries *m_gridAxis;
    QLineSeries *m_lineSeries1;
    QLineSeries *m_lineSeries2;
    QLineSeries *m_lineSeries3;
    QSplineSeries *m_splineSeries1;
    QSplineSeries *m_splineSeries2;
    QSplineSeries *m_splineSeries3;
    QScatterSeries *m_scatterSeries1;
    QScatterSeries *m_scatterSeries2;
    QScatterSeries *m_scatterSeries3;
    QCategoryAxis *m_axisX;
    QCategoryAxis *m_axisY1;
    QCategoryAxis *m_axisY2;
    qreal m_zoomLevel;
    QVector<QPointF> m_data1;
    QVector<QPointF> m_data2;
    QVector<QPointF> m_data3;
    int m_Y1Res; // mv per div
    int m_Y2Res; // mv per div
    int m_XRes; // µs per div
    qreal m_deltaX; // to know X scrolling value
    qreal m_deltaY; // to know Y scrolling value
    int m_deltaX1; // delta X canal1 in samples
    int m_deltaX2; // delta X canal2 in samples
    int m_deltaY1; // Y pos for trace canal 1
    int m_deltaY2; // Y pos for trace canal 2
    int m_trigLevel;

    Modes m_mode;
    MathModes m_mathMode;
    bool m_canal1IsInverted = true;
    bool m_canal2IsInverted = true;
    bool m_canal1IsVisible;
    bool m_canal2IsVisible;
    QColor m_canal1Color;
    QColor m_canal2Color;
    QTimer *m_timer;
};

#endif // OSCILLOCHART_H
