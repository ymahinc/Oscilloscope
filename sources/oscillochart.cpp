#include "oscillochart.h"
#include "mainwindow.h"

#include <QDebug>
#include <QRandomGenerator>
#include "fft.h"
#include "frequencyspectrum.h"

OscilloChart::OscilloChart(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QChart(parent, wFlags){    
    setCanalInverted(1,false);
    setCanalInverted(2,false);

    updateDeltaX1(0);
    updateDeltaX2(0);
    updateDeltaY1(0);
    updateDeltaY2(0);

    m_canal1Color = Qt::black;
    m_canal2Color = Qt::black;

    m_canal1IsVisible = true;
    m_canal2IsVisible = true;

    m_zoomLevel = 1.0;

    m_trigLevel = 0;

    m_deltaX = 0;
    m_deltaY = 0;

    legend()->hide();
    setTheme(QChart::ChartThemeDark);

    m_axisX = new QCategoryAxis;
    m_axisX->setMin(0);
    m_axisX->setMax(300);
    m_axisX->setStartValue(0);
    m_axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    setAxisResolution(m_axisX,1);
    addAxis(m_axisX, Qt::AlignBottom);

    m_axisY1 = new QCategoryAxis;
    m_axisY1->setMin(0);
    m_axisY1->setMax(255);
    m_axisY1->setStartValue(0);
    m_axisY1->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    setAxisResolution(m_axisY1,100);
    addAxis(m_axisY1, Qt::AlignLeft);

    m_axisY2 = new QCategoryAxis;
    m_axisY2->setMin(0);
    m_axisY2->setMax(255);
    m_axisY2->setStartValue(0);
    m_axisY2->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    setAxisResolution(m_axisY2,100);
    addAxis(m_axisY2, Qt::AlignRight);

   createGridTicks();

    m_lineSeries1 =  new QLineSeries(this);
    m_lineSeries2 =  new QLineSeries(this);
    m_lineSeries3 =  new QLineSeries(this);

    m_splineSeries1 =  new QSplineSeries(this);
    m_splineSeries2 =  new QSplineSeries(this);
    m_splineSeries3 =  new QSplineSeries(this);

    m_scatterSeries1 =  new QScatterSeries(this);
    m_scatterSeries1->setMarkerSize(1.0);

    m_scatterSeries2 =  new QScatterSeries(this);
    m_scatterSeries2->setMarkerSize(1.0);

    m_scatterSeries3 =  new QScatterSeries(this);
    m_scatterSeries3->setMarkerSize(1.0);

    setCanalColor(1,Qt::black);
    setCanalColor(2,Qt::black);
    setCanalColor(3,Qt::white);

    m_canal1 = m_lineSeries1;
    m_canal2 = m_lineSeries2;
    m_canal3 = m_lineSeries3;

    setInterpolation(OscilloChart::Line);

    m_mode = Classic;
    setMathMode(Add);
    setMode(Classic);

    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(updateView()));
    m_timer->start(1);
}

void OscilloChart::triggerLevelUpdatedFromView(int val){
    emit triggerLevelUpdated(val);
}

void OscilloChart::updateDeltaX1(int delta){
    m_deltaX1 = delta;
}

void OscilloChart::updateDeltaX2(int delta){
    m_deltaX2 = delta;
}

void OscilloChart::updateDeltaY1(int delta){
    m_deltaY1 = delta;
}

void OscilloChart::updateDeltaY2(int delta){
    m_deltaY2 = delta;
}

void OscilloChart::updateMathMode(int mode){
    setMathMode((MathModes) mode);
}

OscilloChart::Modes OscilloChart::mode(){
    return m_mode;
}

OscilloChart::MathModes OscilloChart::mathMode(){
    return m_mathMode;
}

void OscilloChart::setTimePerDiv(int microsec){
    setAxisResolution(m_axisX,microsec);
}

void OscilloChart::setmVPerDiv(int canal, int mv){
    if ( canal == 1 )
        setAxisResolution(m_axisY1,mv);
    if ( canal == 2 )
        setAxisResolution(m_axisY2,mv);
}

void OscilloChart::setCanalInverted(int canal, bool inverted){
    if ( canal == 1 )
        m_canal1IsInverted = !inverted;
    if ( canal == 2 )
        m_canal2IsInverted = !inverted;
}

void OscilloChart::setMode(Modes mode){
    m_mode = mode;
    if ( m_mode == Maths ){
        m_canal1->setVisible(false);
        m_canal2->setVisible(false);
        m_canal3->setVisible(true);
        setMathMode(m_mathMode);
    }else{
        m_canal1->setVisible(m_canal1IsVisible);
        m_canal2->setVisible(m_canal2IsVisible);
        m_canal3->setVisible(false);
        m_axisY2->setVisible(true);
        m_axisY1->setLinePenColor(m_canal1Color);
        m_axisY1->setLabelsColor(m_canal1Color);
        m_axisX->setLinePenColor(Qt::white);
        m_axisX->setLabelsColor(Qt::white);
        setAxisResolution(m_axisY1,m_Y1Res);
        setAxisResolution(m_axisY2,m_Y2Res);
        setAxisResolution(m_axisX,m_XRes);
    }
}

void OscilloChart::setMathMode(MathModes mathMode){
    m_mathMode = mathMode;
    if ( m_mathMode == Add  || m_mathMode == Substract ){
        m_axisY2->setVisible(false);
        m_axisY1->setLinePenColor(Qt::white);
        m_axisY1->setLabelsColor(Qt::white);
        if ( m_Y2Res > m_Y1Res )
            setAxisResolution(m_axisY1,m_Y2Res);
        m_axisX->setLinePenColor(Qt::white);
        m_axisX->setLabelsColor(Qt::white);
        setAxisResolution(m_axisX,m_XRes);
        m_canal3->setColor(Qt::white);
    }
    if ( m_mathMode == XY ){
        m_axisY2->setVisible(false);
        m_axisY1->setLinePenColor(m_canal1Color);
        m_axisY1->setLabelsColor(m_canal1Color);
        m_axisX->setLinePenColor(m_canal2Color);
        m_axisX->setLabelsColor(m_canal2Color);
        setAxisResolution(m_axisY1,m_Y1Res);
        setAxisResolution(m_axisX,m_Y2Res);
        m_canal3->setColor(Qt::white);
    }
    if ( m_mathMode == FFT1 || m_mathMode == FFT2 ){
        m_axisY2->setVisible(false);
        if ( m_mathMode == FFT1 ){
            m_axisY1->setLinePenColor(m_canal1Color);
            m_axisY1->setLabelsColor(m_canal1Color);
            m_canal3->setColor(m_canal1Color);
        }else{
            m_axisY1->setLinePenColor(m_canal2Color);
            m_axisY1->setLabelsColor(m_canal2Color);
            m_canal3->setColor(m_canal2Color);
        }

        m_axisX->setLinePenColor(Qt::white);
        m_axisX->setLabelsColor(Qt::white);
        setAxisResolution(m_axisY1,10);
        setAxisResolution(m_axisX,m_XRes);
    }
    emit mathModeUpdated();
}

void OscilloChart::createGridTicks(){
    m_gridAxis =  new QLineSeries(this);
    m_gridAxis->setColor(Qt::white);
    QVector<QPointF> xData;
    for (int i = 0; i < 10; i ++){
        for(int j = 0; j < 5; j++ ){
            xData << QPointF(i*30+j*6,127.5) << QPointF(i*30+(j+1)*6,127.5)
                  << QPointF(i*30+(j+1)*6,125) << QPointF(i*30+(j+1)*6,130)
                  << QPointF(i*30+(j+1)*6,127.5);
        }
        if ( i < 9 ){
            xData << QPointF(i*30,127.5) << QPointF((i+1)*30,127.5)
              << QPointF((i+1)*30,122.5) << QPointF((i+1)*30,132.5) << QPointF((i+1)*30,127.5);
        }
    }
    xData << QPointF(150,127.5) << QPointF(150,0);
    for (int i = 0; i < 8; i ++){
        for(int j = 0; j < 5; j++ ){
            xData << QPointF(150,i*31.875+j*6.375) << QPointF(150,i*31.875+(j+1)*6.375)
                  << QPointF(148,i*31.875+(j+1)*6.375) << QPointF(152,i*31.875+(j+1)*6.375)
                  << QPointF(150,i*31.875+(j+1)*6.375);
        }
        if ( i < 7 ){
            xData << QPointF(150,i*31.875) << QPointF(150,(i+1)*31.875)
              << QPointF(146,(i+1)*31.875) << QPointF(154,(i+1)*31.875) << QPointF(150,(i+1)*31.875);
        }
    }
    m_gridAxis->replace(xData);
    /*QPen p;
    p.setColor(Qt::white);
    p.setCosmetic(true);
    m_gridAxis->setPen(p);*/
    addSeries(m_gridAxis);
    m_gridAxis->attachAxis(m_axisY1);
    m_gridAxis->attachAxis(m_axisX);
}

void OscilloChart::setAxisResolution(QCategoryAxis *axis, int value){
    // remove old labels
    int labelsCount = axis->count() - 1;
    for (int i = labelsCount; i >= 0 ; i--)
        axis->remove(axis->categoriesLabels().at(i));

    // recreate axis values
    if ( axis == m_axisX )
        setXAxisResolution(value);
    else
        setYAxisResolution(axis,value);
}

void OscilloChart::setXAxisResolution(int value){
    // X axis is time axis
    if ( (m_mode == Classic) || ( (m_mode == Maths) && (m_mathMode == Add) ) ||
         ( (m_mode == Maths) && (m_mathMode == Substract) )){
        m_XRes = value;
        QString unit;
        qreal divider = 1.0;
        for (int i = 0; i < 11; i++){
            if ( i*value >= 1000000 ){
                unit = tr("s");
                divider = 1000000.0;
            }else if ( i*value >= 1000 ){
                unit = tr("ms");
                divider = 1000.0;
            }else{
                unit = tr("µs");
                divider = 1.0;
            }
            m_axisX->append(QString("%1 %2").arg(i*value/divider).arg(unit), i*300/10.0);
        }
    }

    if ( (m_mode == Maths) && (m_mathMode == XY) ){
        QString unit;
        qreal divider = 1.0;
        qreal startVal = value * -4.0;
        for (int i = 0; i < 9; i++){
            if ( ((startVal+i*value) >= 1000) || ((startVal+i*value) <= -1000) ){
                unit = tr("V");
                divider = 1000.0;
            }else{
                unit = tr("mV");
                divider = 1.0;
            }
            m_axisX->append(QString("%1 %2").arg((startVal+i*value)/divider).arg(unit), i*300/8.0);
        }
    }

    if ( ((m_mode == Maths) && (m_mathMode == FFT1)) || ((m_mode == Maths) && (m_mathMode == FFT2)) ){
        m_XRes = value;
        QString unit;
        qreal tempValue = 1.0;
        for (int i = 0; i < 11; i++){
            unit = tr("MHz");
            tempValue = 30.0 / m_XRes * 50000.0;
            qreal divider = 1000000.0;
            if ( i*tempValue < 1000000 ){
                unit = tr("KHz");
                divider = 1000.0;
            }
            if ( i*tempValue < 1000 ){
                unit = tr("Hz");
                divider = 1.0;
            }
            m_axisX->append(QString("%1 %2").arg(i*tempValue/divider).arg(unit), i*300/10.0);
        }
    }
}

void OscilloChart::setYAxisResolution(QCategoryAxis *axis, int value){
    if ( m_mode == Classic ){
        if ( axis == m_axisY1 )
            m_Y1Res = value;
        if ( axis == m_axisY2 )
            m_Y2Res = value;
    }

    if ( (axis == m_axisY2) || (m_mode == Classic) || ((m_mode == Maths)&&(m_mathMode==XY)&&(axis == m_axisY1))
          || ((m_mode == Maths)&&(m_mathMode==Add)&&(axis == m_axisY1)) ||
         ((m_mode == Maths)&&(m_mathMode==Substract)&&(axis == m_axisY1))){
        QString unit;
        qreal divider = 1.0;
        qreal startVal = value * -4.0;
        for (int i = 0; i < 9; i++){
            if ( ((startVal+i*value) >= 1000) || ((startVal+i*value) <= -1000) ){
                unit = tr("V");
                divider = 1000.0;
            }else{
                unit = tr("mV");
                divider = 1.0;
            }
            axis->append(QString("%1 %2").arg((startVal+i*value)/divider).arg(unit), i*255/8.0);
        }
    }

    if ( ((m_mode == Maths) && (m_mathMode == FFT1)) || ((m_mode == Maths) && (m_mathMode == FFT2)) ){
        //QString unit = tr("");
        QString emptyStr = "";
        for (int i = 0; i < 9; i++){
            //axis->append(QString("%1 %2").arg(i*value).arg(unit), i*255/8.0);
            axis->append(emptyStr, i*255/8.0);
            emptyStr.append(" ");
        }
    }
}

qreal OscilloChart::valueFromGraph(qreal value, int axis){
    qreal val = 0;
    if ( axis == 0 ) // X Axis return value in µs
        val = value*m_XRes*10.0/300.0;
    if ( axis == 1 ) // Y1 Axis
        val = value*m_Y1Res*8.0/1000.0/255.0;
    if ( axis == 2 ) // Y2 Axis
        val = value*m_Y2Res*8.0/1000.0/255.0;
    if ( axis == 3 || axis == 4 ){ // Maths Y Axis 4 is for x values in XY mode
        if ( m_mathMode == Add || m_mathMode == Substract ){
            if ( m_Y1Res > m_Y2Res )
                val = value*m_Y1Res*8.0/1000.0/255.0;
            else
                val = value*m_Y2Res*8.0/1000.0/255.0;
        }
        if ( m_mathMode == XY ){
            if ( axis == 3)
                val = value*m_Y1Res*8.0/1000.0/255.0;
            else
                val = value*m_Y2Res*8.0/1000.0/300.0;
        }
    }
    if ( axis == 5 || axis == 6 ){ // Maths X and Y Axis for values in FFT mode
        if ( axis == 5)
            val = value*80.0/255.0;
        else
            val = 50000.0*value/m_XRes;
    }
    return val;
}

QColor OscilloChart::canalColor(int canal){
    if ( canal == 1 )
        return m_canal1Color;
    if ( canal == 2 )
        return m_canal2Color;
    return Qt::black;
}

void OscilloChart::invertCanal1(bool invert){
    setCanalInverted(1,invert);
}

void OscilloChart::invertCanal2(bool invert){
    setCanalInverted(2,invert);
}

int OscilloChart::Y1Res(){
    return m_Y1Res;
}

int OscilloChart::Y2Res(){
    return m_Y2Res;
}

void OscilloChart::setTriggerLevel(int level){
    m_trigLevel = level;
    scene()->update();
}

int OscilloChart::triggerLevel(){
    return m_trigLevel;
}

void OscilloChart::zoomInSlot(){
    m_zoomLevel *= 2.0;
    if ( m_zoomLevel <= 32 )
        zoomIn();
    else
        m_zoomLevel = 32;
    emit zoomUpdated();
}

void OscilloChart::zoomOutSlot(){
    m_zoomLevel *= 0.5;
    if ( m_zoomLevel > 1 )
        zoomOut();
    else
        resetZoomSlot();
    emit zoomUpdated();
}

void OscilloChart::resetZoomSlot(){
    zoomReset();
    m_zoomLevel = 1.0;
    emit zoomUpdated();
}

void OscilloChart::zoomRect(QRectF rect){
    qreal xZoom = plotArea().width() / rect.normalized().width();
    qreal yZoom = plotArea().height() / rect.normalized().height();
    qreal zoom = qMax(xZoom,yZoom);
    m_zoomLevel *= zoom;
    if ( m_zoomLevel <= 1 ){
        resetZoomSlot();
    }else{
        if ( m_zoomLevel > 32 ){
            qreal factor = m_zoomLevel /32.0;
            rect.setWidth(rect.width()*factor);
            rect.setHeight(rect.height()*factor);
            m_zoomLevel /= factor;
        }
        zoomIn(rect.normalized());
    }
    emit zoomUpdated();
}

void OscilloChart::scrollBy(qreal x, qreal y){
    if ( m_zoomLevel > 1 ){
        QPointF topLeft = mapToValue(QPointF(plotArea().topLeft()));
        QPointF bottomRight = mapToValue(QPointF(plotArea().bottomRight()));
        qreal dX = x;
        qreal dY = y;

        if ( (topLeft.x() + x) < 0 ){
            dX = topLeft.x() * -1.0;
        }
        if ( (topLeft.y() + y) > 255 ){
            dY = 255 - topLeft.y();
        }
        if ( (bottomRight.x() + x) > 300 ){
            dX = 300 - bottomRight.x();
        }
        if ( (bottomRight.y() + y) < 0 ){
            dY = (bottomRight.y()+y) * -1.0;
        }

        scroll(dX,dY);
    }

    //qDebug() << y << dY << bottomRight.y() << mapToValue(QPointF(plotArea().bottomRight())).y();
}

void OscilloChart::setCanalColor(int canal, QColor color){
    QPen p;
    p.setCosmetic(true);
    p.setColor(color);
    if ( canal == 1 ){
        m_canal1Color = color;
        m_lineSeries1->setPen(p);
        m_splineSeries1->setPen(p);
        m_scatterSeries1->setPen(p);
        m_axisY1->setLabelsColor(color);
        m_axisY1->setLinePenColor(color);
    }
    if ( canal == 2 ){
        m_canal2Color = color;
        m_lineSeries2->setPen(p);
        m_splineSeries2->setPen(p);
        m_scatterSeries2->setPen(p);
        m_axisY2->setLabelsColor(color);
        m_axisY2->setLinePenColor(color);
    }
    if ( canal == 3 ){
        m_lineSeries3->setPen(p);
        m_splineSeries3->setPen(p);
        m_scatterSeries3->setPen(p);
    }
}

void  OscilloChart::setCanalVisible(int canal, bool visible){
    if ( m_mode == Classic ){
        if ( canal == 1 ){
            m_canal1->setVisible(visible);
            m_canal1IsVisible = visible;
        }
        if ( canal == 2 ){
            m_canal2->setVisible(visible);
            m_canal2IsVisible = visible;
        }
    }
}

void OscilloChart::setInterpolation(OscilloChart::Interpolation mode){
    if ( series().contains(m_canal1) )
        removeSeries(m_canal1);
    if ( series().contains(m_canal2) )
        removeSeries(m_canal2);
    if ( series().contains(m_canal3) )
        removeSeries(m_canal3);

    if ( mode == OscilloChart::Dot ){
        m_canal1 = m_scatterSeries1;
        m_canal2 = m_scatterSeries2;
        m_canal3 = m_scatterSeries3;
    }
    if ( mode == OscilloChart::Line ){
        m_canal1 = m_lineSeries1;
        m_canal2 = m_lineSeries2;
        m_canal3 = m_lineSeries3;
    }
    if ( mode == OscilloChart::Sin ){
        m_canal1 = m_splineSeries1;
        m_canal2 = m_splineSeries2;
        m_canal3 = m_splineSeries3;
    }
    addSeries(m_canal1);
    addSeries(m_canal2);
    addSeries(m_canal3);

    m_canal1->attachAxis(m_axisY1);
    m_canal1->attachAxis(m_axisX);
    m_canal2->attachAxis(m_axisY2);
    m_canal2->attachAxis(m_axisX);
    m_canal3->attachAxis(m_axisY1);
    m_canal3->attachAxis(m_axisX);
}

void OscilloChart::updateData(QByteArray data,int canal){
    /*for (int i = 0 ; i < 1024; i ++) // A virer
        data[i] = data.at(i) + QRandomGenerator::global()->generateDouble()*10;*/

    if ( canal == 1 )
        m_data1.clear();
    if ( canal == 2 )
        m_data2.clear();

    for(int i=0; i < 1024; i++){
        if ( canal == 1 )
            m_data1 << QPointF(i,(uchar)data.at(i));
        if ( canal == 2 )
            m_data2 << QPointF(i,(uchar)data.at(i));
    }
}

void OscilloChart::updateView(){
    if ( m_data1.size() != 1024 || m_data2.size() != 1024 ) // no data available
        return;

    if ( m_mode == Classic ){
        QVector<QPointF> tmp;
        int windowStart = 361 + m_deltaX1;
        for(int i=windowStart; i < (windowStart+300); i++){
            if ( m_canal1IsInverted )
                tmp.append(QPointF(i-windowStart,m_deltaY1+255-m_data1.at(i).y()));
            else
                tmp.append(QPointF(i-windowStart,m_deltaY1+m_data1.at(i).y()));
        }
        m_canal1->replace(tmp);

        tmp.clear();
        windowStart = 361 + m_deltaX2;
        for(int i=windowStart; i < (windowStart+300); i++){
            if ( m_canal2IsInverted )
                tmp.append(QPointF(i-windowStart,m_deltaY2+255-m_data2.at(i).y()));
            else
                tmp.append(QPointF(i-windowStart,m_deltaY2+m_data2.at(i).y()));
        }
        m_canal2->replace(tmp);
    }

    if ( m_mode == Maths ){
        if ( m_mathMode == Add )
            updateDataAddFunction();
        if ( m_mathMode == Substract )
            updateDataSubstFunction();
        if ( m_mathMode == XY )
            updateDataXYFunction();
        if ( m_mathMode == FFT1 || m_mathMode == FFT2 )
            updateDataFFTFunction();
    }
}

void OscilloChart::updateDataAddFunction(){
    m_data3.clear();
    for(int i=361; i < 662; i++){
        double val1mv = ( m_data1.at(i).y() - 128 ) / 32 * m_Y1Res;
        double val2mv = ( m_data2.at(i).y() - 128 ) / 32 * m_Y2Res;
        double newValRaw;
        if ( m_Y1Res > m_Y2Res ){
            double newVal = val1mv + val2mv;
            newValRaw = 32 * newVal / m_Y1Res + 128;
        }else{
            double newVal = val2mv + val1mv;
            newValRaw = 32 * newVal / m_Y2Res + 128;
        }
        m_data3 << QPointF(i-361, newValRaw);
    }
    m_canal3->replace(m_data3);
}

void OscilloChart::updateDataSubstFunction(){
    m_data3.clear();
    for(int i=361; i < 662; i++){
        double val1mv = ( m_data1.at(i).y() - 128 ) / 32 * m_Y1Res;
        double val2mv = ( m_data2.at(i).y() - 128 ) / 32 * m_Y2Res;
        double newVal;
        double newValRaw;
        if ( m_Y1Res > m_Y2Res ){
            newVal = val1mv - val2mv;
            newValRaw = 32 * newVal / m_Y1Res + 128;
        }else{
            newVal = val2mv - val1mv;
            newValRaw = 32 * newVal / m_Y2Res + 128;
        }
        m_data3 << QPointF(i-361, newValRaw);
    }
    m_canal3->replace(m_data3);
}

void OscilloChart::updateDataXYFunction(){
    m_data3.clear();
    qreal ratio = 300.0/255.0;
    for(int i=0; i < 1024; i++)
        m_data3 << QPointF(m_data2.at(i).y()*ratio, m_data1.at(i).y());
    m_canal3->replace(m_data3);
}

void OscilloChart::updateDataFFTFunction(){
    m_data3.clear();
    FFT fft;
    Complex array[1024];

    for(int i=0; i < m_data1.size(); i++){
        std::complex<float> val;
        if ( m_mathMode == FFT1 )
            val.real((float)(m_data1[i].y()/255));
        else
            val.real((float)(m_data2[i].y()/255));
        val.imag(0.0);

        array[i] = val;
    }

    CArray datafft(array, 1024);
    fft.forwardfft(datafft);

    const FrequencySpectrum *fs = fft.getFrequency(datafft, 30.0 / m_XRes * 1000000.0);

    int ptr = 0;
    double maxF = 30.0 / m_XRes * 1000000.0 / 2.0;
    for (int i = 0; i < 512; i++){
        m_data3 << QPointF(fs->at(i).frequency*300/maxF, fs->at(i).amplitude*255.0);
        ptr += 2;
    }

    m_canal3->replace(m_data3);
}
