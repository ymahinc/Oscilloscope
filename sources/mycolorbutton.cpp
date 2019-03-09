#include "mycolorbutton.h"

#include <QColorDialog>

MYColorButton::MYColorButton(QWidget *parent) :
    QPushButton(parent){
    m_color = Qt::black;
    connect(this,SIGNAL(clicked(bool)),this,SLOT(showDLG()));
    setCustomPixmap();
}

QColor MYColorButton::color(){
    return m_color;
}

void MYColorButton::setColor(QColor color){
    m_color = color;
    setCustomPixmap();
}

void MYColorButton::showDLG(){
    QColorDialog colorDLG(m_color,this);
    if ( colorDLG.exec() == QDialog::Accepted ){
        QColor newColor = colorDLG.currentColor();
        if ( m_color != newColor ){
            m_color = newColor;
            setCustomPixmap();
            emit colorChanged(m_color);
        }
    }
}

void MYColorButton::setCustomPixmap(){
    QPixmap pixmap(32, 16);
    pixmap.fill(m_color);
    this->setIcon(pixmap);
    setIconSize(pixmap.rect().size());
}
