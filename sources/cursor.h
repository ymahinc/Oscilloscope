#ifndef CURSOR_H
#define CURSOR_H

#include <QGraphicsItem>
#include <QPainter>
class OscilloChart;
class OscilloView;

class Cursor : public QGraphicsItem
{
public:
    enum Orientation{Horizontal, Vertical};

    Cursor(QString text, OscilloChart *chart, int type = 0, OscilloView *view = 0, QGraphicsItem *parent = nullptr);
    QColor color();
    Orientation orientation();
    QString text();
    void setAutoHide(bool autoHide);
    bool autoHide();
    void temporaryEnable(bool enable);

public slots:
    void setColor(QColor color);
    void setOrientation(Orientation orientation);
    void setText(QString text);
    void updateCursor();

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

private:
    OscilloChart *m_chart;
    QColor m_color;
    Orientation m_orientation;
    QString m_text;
    QPainterPath m_path;
    QRectF m_boundingRect;
    int m_type; // 0 classic cursor, 1 trigger cursor
    OscilloView *m_view;
    bool m_autoHide;
    bool m_temporaryShow;
};

#endif // CURSOR_H
