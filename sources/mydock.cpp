#include "mydock.h"

#include <QDebug>

MYDock::MYDock(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent,flags) {
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connect(this,SIGNAL(topLevelChanged(bool)),this,SLOT(onDockedChanged(bool)));
}

void MYDock::keyPressEvent(QKeyEvent *event){
    if ( event->key() == Qt::Key_Escape )
        emit leaveFullScreen();
    if ( event->key() == Qt::Key_Z )
        emit changeMode(0);
    if ( event->key() == Qt::Key_R )
        emit changeMode(1);
    QDockWidget::keyPressEvent(event);
}

void MYDock::onDockedChanged(bool floating){
    if ( floating )
        setWindowOpacity(0.8);
    else
        setWindowOpacity(1.0);
}
