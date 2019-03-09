#include "cursor.h"

#include "oscillochart.h"
#include "oscilloview.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

Cursor::Cursor(QString text, OscilloChart *chart, int type, OscilloView *view, QGraphicsItem *parent)
    :  QGraphicsItem (parent), m_chart(chart), m_text(text){
    m_view = view;
    m_type = type;
    setZValue(11);
    m_color = Qt::red;
    m_orientation = Cursor::Horizontal;
    m_boundingRect = QRectF();
    setFlag(ItemIsMovable,true);
    setFlag(ItemSendsGeometryChanges,true);
    m_autoHide = false;
    m_temporaryShow = false;
}

void Cursor::setAutoHide(bool autoHide){
    m_autoHide = autoHide;
    updateCursor();
}

bool Cursor::autoHide(){
    return m_autoHide;
}

void Cursor::temporaryEnable(bool enable){
    m_temporaryShow = enable;
    updateCursor();
}

QVariant Cursor::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value){

    if (change == ItemPositionChange && scene()) {
        QPointF newPos;
        if ( m_orientation == Cursor::Horizontal ){
            qreal yPos = value.toPointF().y();
            if ( yPos < m_chart->plotArea().top() )
                yPos = m_chart->plotArea().top();
            if ( yPos > m_chart->plotArea().bottom() )
                yPos = m_chart->plotArea().bottom();
            newPos = QPointF(m_chart->plotArea().left(),yPos);
            if ( m_type == 1 ){
                QPointF trigPos = m_chart->mapToValue(newPos);
                m_chart->triggerLevelUpdatedFromView(trigPos.toPoint().y());
            }
            return newPos;
        }
        if ( m_orientation == Cursor::Vertical ){
            qreal xPos = value.toPointF().x();
            if ( xPos < m_chart->plotArea().left() )
                xPos = m_chart->plotArea().left();
            if ( xPos > m_chart->plotArea().right() )
                xPos = m_chart->plotArea().right();
            newPos = QPointF(xPos,m_chart->plotArea().top());
            if( m_type == 2 )
                m_view->updateY1Pos(m_chart->mapToValue(pos()).x());
            return newPos;
        }
    }

    return QGraphicsItem::itemChange(change,value);
}

void Cursor::setText(QString text){
    m_text = text;
}

void Cursor::updateCursor(){
    prepareGeometryChange();
    m_path = QPainterPath();
    if ( m_orientation == Cursor::Horizontal ){
        m_path.moveTo(-10,-5);
        m_path.lineTo(0,0);
        m_path.lineTo(-10,5);
        m_path.lineTo(-10,-5);
        m_path.moveTo(0,0);
        if ( !m_autoHide || (m_autoHide && m_temporaryShow) )
            m_path.lineTo(m_chart->plotArea().width(),0);
        m_boundingRect = m_path.boundingRect();
    }
    if ( m_orientation == Cursor::Vertical ){
        m_path.moveTo(-5,-10);
        m_path.lineTo(0,0);
        m_path.lineTo(5,-10);
        m_path.lineTo(-5,-10);
        m_path.moveTo(0,0);
        if ( !m_autoHide || (m_autoHide && m_temporaryShow) )
            m_path.lineTo(0,m_chart->plotArea().height());
        m_boundingRect = m_path.boundingRect();
    }
}

QString Cursor::text(){
    return m_text;
}

void Cursor::setOrientation(Cursor::Orientation orientation){
    m_orientation = orientation;
}

Cursor::Orientation Cursor::orientation(){
    return m_orientation;
}

void Cursor::setColor(QColor color){
    m_color = color;
}

QColor Cursor::color(){
    return m_color;
}

QRectF Cursor::boundingRect() const{
    return m_boundingRect;
}

QPainterPath Cursor::shape() const {
    return m_path;
}

void Cursor::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    if ( m_type == 1 ){
        m_chart->triggerLevelUpdatedFromView(127);
        QPointF midPoint = m_chart->mapToPosition(QPointF(0,127));
        setPos(QPointF(m_chart->plotArea().left(),midPoint.y()));
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void Cursor::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if ( m_autoHide ){
        m_temporaryShow = true;
        updateCursor();
    }
    QGraphicsItem::mousePressEvent(event);
}

void Cursor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if ( m_autoHide ){
        m_temporaryShow = false;
        updateCursor();
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void Cursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QPen p;
    p.setCosmetic(true);
    p.setColor(m_color);

    painter->setPen(p);
    painter->setBrush(QBrush(m_color));

    painter->drawPath(m_path);
}
