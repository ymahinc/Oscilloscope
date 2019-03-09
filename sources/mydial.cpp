#include "mydial.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

MYDial::MYDial(QWidget *parent)
    : QDial(parent){
    m_color = QColor();
}

void MYDial::mousePressEvent(QMouseEvent *event){
    emit mousePressed();
    QWidget::mousePressEvent(event);
}

void MYDial::mouseReleaseEvent(QMouseEvent *event){
    emit mouseReleased();
    QWidget::mouseReleaseEvent(event);
}

void MYDial::wheelEvent(QWheelEvent *event){
    event->ignore();
}

void MYDial::paintEvent(QPaintEvent *event){
    if ( m_color.isValid() ){
        QPainter painter(this);
        QPen p(m_color);
        p.setWidth(3);
        painter.setPen(p);
        QRectF r = rect();
        r.adjust(0,0,0.5,0.5);
        qreal w = width()/10.0;
        qreal h = height()/10.0;
        r.adjust(w,h,-w,-h);
        painter.drawEllipse(r);
    }
    QDial::paintEvent(event);
}

void MYDial::setColor(QColor color){
    m_color = color;
    update();
}
