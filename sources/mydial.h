#ifndef MYDIAL_H
#define MYDIAL_H

#include <QDial>
#include <QWheelEvent>

class MYDial : public QDial
{
    Q_OBJECT

public:
    MYDial(QWidget *parent = nullptr);
    void setColor(QColor color);

signals:
    void mousePressed();
    void mouseReleased();

protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QColor m_color;
};

#endif // MYDIAL_H
