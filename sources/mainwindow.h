#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QActionGroup>

#include "settingsdlg.h"
//#include "oscillo.h"
#include "oscillochart.h"
#include "oscilloview.h"
#include "portthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Settings const settings();

protected:
    void closeEvent(QCloseEvent *event);

private slots:   
    void setMode(int mode);
    void showConfigureDLG();
    void toggleCanal1(bool show);
    void toggleCanal2(bool show);
    void updateY1Res(int res);
    void updateY2Res(int res);
    void updateChan1Coupling();
    void updateChan2Coupling();
    void updateTrigValue();
    void updateTrigColor();
    void updateTimeDiv();
    void updateTrigCursor();
    void updateTrigState(bool triggered);
    void updateMode(QAction *action);
    void invertTrig(bool invert);
    void updateTrigMode();
    void updateParams();
    void setUiEnable(bool enable);
    void onCommandResult(bool success, QString error, int type);
    void getY1Res();
    void getY2Res();
    void getXRes();
    void getTrigMod();
    void getInvTrig();
    void onMathFunction();
    void snapShot();
    void fullScreen();
    void leaveFullScreen();
    void onMathModeUpdated();
    void onModeUpdated(QAction *action);
    void updateTrigLevelFromView(int val);
    void showTrigCur();
    void hideTrigCur();

private:
    Ui::MainWindow *ui;

    OscilloChart *m_oscilloChart;
    OscilloView *m_chartView ;
    QActionGroup *m_toolActionGroup;
    QActionGroup *m_modeActionGroup;
    PortThread m_thread;
    QQueue<Command> m_queue;
    int m_Y1Res = 0;
    int m_Y2Res = 0;
    int m_XRes = 0;
    int m_trig = 0;
    bool m_invTrig = false;
    Settings m_settings;
    QLabel *m_statusLabel;
    QByteArray m_geometry;

    void writeSettings();
    void readSettings();
    void connectToOscillo();   
    void updateView();
    void setMvPerDiv(int canal, int Yres);
};

#endif // MAINWINDOW_H
