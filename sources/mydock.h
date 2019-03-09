#ifndef MYDOCK_H
#define MYDOCK_H

#include <QDockWidget>
#include <QKeyEvent>

class MYDock : public QDockWidget
{
    Q_OBJECT

public:
    MYDock(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

signals:
    void leaveFullScreen();
    void changeMode(int mode);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onDockedChanged(bool floating);
};

#endif // MYDOCK_H
