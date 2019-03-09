#include "myslider.h"

MYSlider::MYSlider(QWidget *parent)
    : QSlider(parent){

}

void MYSlider::mouseDoubleClickEvent(QMouseEvent *event){
    Q_UNUSED(event);
    setValue(0);
}
