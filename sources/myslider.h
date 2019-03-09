#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>

class MYSlider : public QSlider
{
public:
    MYSlider(QWidget *parent = nullptr);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // MYSLIDER_H
