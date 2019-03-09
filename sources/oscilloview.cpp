#include "oscilloview.h"
#include "global.h"
#include "mainwindow.h"
#include "oscillochart.h"

#include <QDebug>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QWindow>
#include <QFileDialog>

OscilloView::OscilloView(OscilloChart *chart, QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent){
    m_oscilloChart = chart;
    //setRubberBand(QChartView::NoRubberBand);
    setMode(Zoom);
    m_isRuling = false;
    m_isDragging = false;
    m_isRubberBanding = false;
    m_GLwidget = new QOpenGLWidget();

    setViewport(m_GLwidget);
    setMouseTracking(true);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    m_trigCursor = new Cursor("T1",chart,1,this);
    m_trigCursor->setAutoHide(true);
    scene()->addItem(m_trigCursor);
    m_lastTrigPos = 127;

    m_yCursor1 = new Cursor("Y1",chart,2,this);
    m_yCursor1->setOrientation(Cursor::Vertical);
    m_yCursor1->setColor(Qt::gray);
    scene()->addItem(m_yCursor1);
    m_yCursor1->setPos(0,0);
    m_lastY1Pos = 0;
}

void OscilloView::setTrigPos(int pos){
    m_lastTrigPos = pos;
    if (scene()) {
        QPointF trigPos = m_oscilloChart->mapToPosition(QPointF(0,pos));
        m_trigCursor->updateCursor();
        m_trigCursor->setPos(m_oscilloChart->plotArea().left(),trigPos.y());
    }
}

void OscilloView::updateY1Pos(qreal pos){
    m_lastY1Pos = pos;
}

Cursor *OscilloView::trigCursor(){
    return m_trigCursor;
}

void OscilloView::setAntialiasing(bool antialias){
    QSurfaceFormat format = QSurfaceFormat();
    if ( antialias ){
        format.setSamples(4);
        setRenderHint(QPainter::Antialiasing);
    }else{
        format.setSamples(1);
        setRenderHint(QPainter::NonCosmeticDefaultPen);
    }
    m_GLwidget->setFormat(format);
}

void OscilloView::save(){

    QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), "", tr("Images (*.png)"));
    if ( filename.isEmpty() )
        return;

    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);

    QWindow window;
    window.setSurfaceType(QWindow::OpenGLSurface);
    window.setFormat(format);
    window.create();

    QOpenGLContext context;
    context.setFormat(format);
    if (!context.create())
        qFatal("Cannot create the requested OpenGL context!");
    context.makeCurrent(&window);

    const QRect drawRect(viewport()->rect());
    const QSize drawRectSize = drawRect.size();

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setSamples(16);
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject fbo(drawRectSize, fboFormat);
    fbo.bind();

    QOpenGLPaintDevice device(drawRectSize);
    QPainter painter;
    painter.begin(&device);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    scene()->render(&painter,drawRect,drawRect);

    painter.end();

    fbo.release();
    fbo.toImage().save(filename, "PNG");
}

void OscilloView::setMode(Mode mode){
    m_mode = mode;
    if ( m_mode == Ruler )
        setCursor(Qt::CrossCursor);
    else
        setCursor(Qt::ArrowCursor);
}

bool OscilloView::viewportEvent(QEvent *event){
    return QGraphicsView::viewportEvent(event);
}

void OscilloView::resizeEvent(QResizeEvent *event){
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        m_oscilloChart->resize(event->size());
        QPointF trigPos = m_oscilloChart->mapToPosition(QPointF(0,m_lastTrigPos));
        m_trigCursor->updateCursor();
        m_trigCursor->setPos(m_oscilloChart->plotArea().left(),trigPos.y());

        QPointF Y1Pos = m_oscilloChart->mapToPosition(QPointF(m_lastY1Pos,0));
        m_yCursor1->updateCursor();
        m_yCursor1->setPos(Y1Pos.x(),m_oscilloChart->plotArea().top());
    }
    QGraphicsView::resizeEvent(event);
}

void OscilloView::mousePressEvent(QMouseEvent *event){
    QGraphicsView::mousePressEvent(event);

    if ( ! m_oscilloChart->plotArea().contains(event->pos()) )
        return;

    if ( event->button() == Qt::LeftButton ){
        if ( m_mode == Ruler ){
            m_isRuling = true;
            m_startPoint = event->pos();
            m_newPoint = m_startPoint;
        }
        if ( m_mode == Zoom ){
            m_isRubberBanding = true;
            m_startPoint = event->pos();
            m_newPoint = m_startPoint;
        }
    }

    if ( event->button() == Qt::MidButton ){
        m_isDragging = true;
        m_startPoint = event->pos();
        m_newPoint = m_startPoint;
        setCursor(Qt::ClosedHandCursor);
    }  
}

void OscilloView::mouseMoveEvent(QMouseEvent *event){   
    QGraphicsView::mouseMoveEvent(event);

    if ( m_isDragging ){
        QPoint delta = event->pos() - m_newPoint;
        m_oscilloChart->scrollBy(-delta.x(), delta.y());
        m_newPoint = event->pos();
        scene()->invalidate();
    }else{
        if ( m_mode == Ruler ){
            m_newPoint = event->pos();
            scene()->invalidate();
        }
        if ( m_mode == Zoom ){
            m_newPoint = event->pos();
            scene()->invalidate();
        }        
    }  
}

void OscilloView::mouseReleaseEvent(QMouseEvent *event){
    QGraphicsView::mouseReleaseEvent(event);

    if ( ! m_oscilloChart->plotArea().contains(event->pos()) )
        return;

    if ( event->button() == Qt::LeftButton ){
        if ( m_mode == Ruler && m_isRuling ){
            m_isRuling = false;
        }
        if ( m_mode == Zoom && m_isRubberBanding ){
             m_isRubberBanding = false;
             QPointF delta = m_startPoint - event->pos();
             if ( delta.manhattanLength() > 4 )
                m_oscilloChart->zoomRect(QRectF(m_startPoint,event->pos()));
        }
    }

    if ( event->button() == Qt::MidButton && m_isDragging ){
        m_isDragging = false;
        if ( m_mode == Ruler )
            setCursor(Qt::CrossCursor);
        else
            setCursor(Qt::ArrowCursor);
    }

    if ( event->button() == Qt::RightButton ){
        m_oscilloChart->zoomOutSlot();
    }
}

void OscilloView::keyPressEvent(QKeyEvent *event){
    switch (event->key()) {
    case Qt::Key_Plus:
        m_oscilloChart->zoomInSlot();
        break;
    case Qt::Key_Minus:
        m_oscilloChart->zoomOutSlot();
        break;
    case Qt::Key_0:
        m_oscilloChart->resetZoomSlot();
        ensureVisible(m_oscilloChart);
        break;
    case Qt::Key_Left:
        m_oscilloChart->scrollBy(-10.0, 0.0);
        break;
    case Qt::Key_Right:
        m_oscilloChart->scrollBy(10, 0);
        break;
    case Qt::Key_Up:
        m_oscilloChart->scrollBy(0, 10);
        break;
    case Qt::Key_Down:
        m_oscilloChart->scrollBy(0, -10);
        break;
    case Qt::Key_5:
        viewport()->rect();
        break;
    case Qt::Key_Escape:
        emit leaveFullScreen();
        break;
    case Qt::Key_Z:
        emit changeMode(0);
        break;
    case Qt::Key_R:
        emit changeMode(1);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void OscilloView::wheelEvent(QWheelEvent *event){
    qreal factor;
    if ( event->delta() > 0 )
        factor = 2.0;
    else
        factor = 0.5;

    QRectF viewportRectInValue = QRectF(m_oscilloChart->plotArea().left(),m_oscilloChart->plotArea().top(),
                                        m_oscilloChart->plotArea().width()/factor,m_oscilloChart->plotArea().height()/factor);
    QPointF mousePos = mapFromGlobal(QCursor::pos());
    viewportRectInValue.moveCenter(mousePos);
    m_oscilloChart->zoomRect(viewportRectInValue);
    QPointF delta = m_oscilloChart->plotArea().center() -mousePos;
    m_oscilloChart->scrollBy(delta.x(),-delta.y());
    m_oscilloChart->zoomUpdated();
}

void OscilloView::drawForeground(QPainter *painter, const QRectF &rect){
    Q_UNUSED(rect)
    QPen pen;
    //pen.setCosmetic(true);
    pen.setColor(Qt::white);
    painter->setPen(pen);

    painter->setClipRect(m_oscilloChart->plotArea());

    if ( m_isRubberBanding ){
        QColor col  =Qt::white;
        col.setAlphaF(0.2);
        painter->setBrush(QBrush(col));
        painter->drawRect(QRect(m_startPoint,m_newPoint));
    }
    if ( m_isRuling ){
        if ( m_oscilloChart->mode() == OscilloChart::Classic ||
             ( (m_oscilloChart->mode() == OscilloChart::Maths) && ( m_oscilloChart->mathMode() == OscilloChart::Add
                 || m_oscilloChart->mathMode() == OscilloChart::Substract
                 || m_oscilloChart->mathMode() == OscilloChart::XY)) ){
                painter->drawLine(m_startPoint,m_newPoint);
        }

        QColor col  =Qt::black;
        col.setAlphaF(0.8);
        painter->fillRect(m_newPoint.x()+10,m_newPoint.y(),90,65,col);
        QPointF startPointValue = m_oscilloChart->mapToValue(m_oscilloChart->mapFromScene(mapToScene(m_startPoint)));
        QPointF newPointValue = m_oscilloChart->mapToValue(m_oscilloChart->mapFromScene(mapToScene(m_newPoint)));
        qreal deltaY = qAbs(newPointValue.y() - startPointValue.y());
        qreal deltaX = qAbs(newPointValue.x() - startPointValue.x());
        qreal valx = m_oscilloChart->valueFromGraph(deltaX,0);

        if ( m_oscilloChart->mode() == OscilloChart::Maths ){
            if ( m_oscilloChart->mathMode() == OscilloChart::Add
                 || m_oscilloChart->mathMode() == OscilloChart::Substract ){
                qreal val3 = m_oscilloChart->valueFromGraph(deltaY,3);
                pen.setColor(Qt::white);
                painter->setPen(pen);
                if ( val3 < 1){
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+15,"     dY: "+QString::number(val3*1000.0,'f',2)+" mV");
                }else{
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+15,"     dY: "+QString::number(val3,'f',2)+" V");
                }

                drawXValues(painter,valx);
            }
            if ( m_oscilloChart->mathMode() == OscilloChart::XY ){
                qreal val3 = m_oscilloChart->valueFromGraph(deltaY,3);
                pen.setColor(m_oscilloChart->canalColor(1));
                painter->setPen(pen);
                if ( val3 < 1){
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+15,"     dY: "+QString::number(val3*1000.0,'f',2)+" mV");
                }else{
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+15,"     dY: "+QString::number(val3,'f',2)+" V");
                }
                val3 = m_oscilloChart->valueFromGraph(deltaX,4);
                pen.setColor(m_oscilloChart->canalColor(2));
                painter->setPen(pen);
                if ( val3 < 1){
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+30,"     dX: "+QString::number(val3*1000.0,'f',2)+" mV");
                }else{
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+30,"     dX: "+QString::number(val3,'f',2)+" V");
                }
            }
            if ( m_oscilloChart->mathMode() == OscilloChart::FFT1 || m_oscilloChart->mathMode() == OscilloChart::FFT2){
                QColor col;
                if ( m_oscilloChart->mathMode() == OscilloChart::FFT1 )
                    col = m_oscilloChart->canalColor(1);
                else
                    col = m_oscilloChart->canalColor(2);
                qreal val3 = m_oscilloChart->valueFromGraph(newPointValue.y(),5);
                pen.setColor(col);
                painter->setPen(pen);
                painter->drawText(m_newPoint.x(),m_newPoint.y()+15,"     Y: "+QString::number(val3/80.0,'f',2));

                val3 = m_oscilloChart->valueFromGraph(newPointValue.x(),6);
                if ( val3 < 1000 )
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+30,"     Y: "+QString::number(val3,'f',2)+" Hz");
                else if ( val3 < 1000000 )
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+30,"     Y: "+QString::number(val3/1000.0,'f',2)+" KHz");
                else
                    painter->drawText(m_newPoint.x(),m_newPoint.y()+30,"     Y: "+QString::number(val3/1000000.0,'f',2)+" MHz");

                pen.setColor(Qt::white);
                pen.setStyle(Qt::DashLine);
                painter->setPen(pen);
                painter->drawLine(m_newPoint.x(),0,m_newPoint.x(),viewport()->rect().bottom());
                painter->drawLine(0,m_newPoint.y(),viewport()->rect().right(),m_newPoint.y());
            }
        }else{
            qreal val1 = m_oscilloChart->valueFromGraph(deltaY,1);
            qreal val2 = m_oscilloChart->valueFromGraph(deltaY,2);
            pen.setColor(m_oscilloChart->canalColor(1));
            painter->setPen(pen);
            if ( val1 < 1){
                painter->drawText(m_newPoint.x(),m_newPoint.y()+15,"     dY1: "+QString::number(val1*1000.0,'f',2)+" mV");
            }else{
                painter->drawText(m_newPoint.x(),m_newPoint.y()+15,"     dY1: "+QString::number(val1,'f',2)+" V");
            }
            pen.setColor(m_oscilloChart->canalColor(2));
            painter->setPen(pen);
            if ( val2 < 1){
                painter->drawText(m_newPoint.x(),m_newPoint.y()+30,"     dY2: "+QString::number(val2*1000.0,'f',2)+" mV");
            }else{
                painter->drawText(m_newPoint.x(),m_newPoint.y()+30,"     dY2: "+QString::number(val2,'f',2)+" V");
            }
            drawXValues(painter,valx);
        }
    }
}

void OscilloView::drawXValues(QPainter *painter, qreal valx){
    QPen pen;
    pen.setColor(Qt::white);
    painter->setPen(pen);
    painter->setPen(pen);
    QString unit = tr("µs");
    qreal valxBackup = valx;

    if ( valx >= 1000 ){
        unit = tr("ms");
        valx = valx / 1000.0;
    }

    if ( valx >= 1000 ){
        unit = tr("s");
        valx = valx / 1000.0;
    }

    painter->drawText(m_newPoint.x(),m_newPoint.y()+45,"     dX:  "+QString::number(valx,'f',2)+" "+unit);

    valxBackup = 1 / ( valxBackup / 1000000.0);
    unit = tr("Hz");

    if ( valxBackup > 1000 ) {
        unit = tr("KHz");
        valxBackup = valxBackup / 1000.0;
    }

    if ( valxBackup > 1000 ){
        unit = tr("MHz");
        valxBackup = valxBackup / 1000.0;
    }

    painter->drawText(m_newPoint.x(),m_newPoint.y()+60,"     f:  "+QString::number(valxBackup,'f',2)+" "+unit);
}
