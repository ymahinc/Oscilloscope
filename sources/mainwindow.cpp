#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "global.h"

#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QOpenGLWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    ui->actionZoom->setData(0);
    ui->actionRuler->setData(1);

    m_toolActionGroup = new QActionGroup(this);
    m_toolActionGroup->setExclusive(true);
    m_toolActionGroup->addAction(ui->actionZoom);
    m_toolActionGroup->addAction(ui->actionRuler);

    ui->actionNormal->setData(0);
    ui->actionSingleShot->setData(1);

    m_modeActionGroup = new QActionGroup(this);
    m_modeActionGroup->setExclusive(true);
    m_modeActionGroup->addAction(ui->actionNormal);
    m_modeActionGroup->addAction(ui->actionSingleShot);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setText(tr("Not connected"));
    ui->statusBar->addWidget(m_statusLabel);

    m_oscilloChart = new OscilloChart();

    m_chartView = new OscilloView(m_oscilloChart);
    m_chartView->scene()->addItem(m_oscilloChart);
    m_oscilloChart->setZValue(10);
    setCentralWidget(m_chartView);

    ui->YRes1Dial->setCurrentRes(8);
    m_Y1Res = 8;
    ui->YRes2Dial->setCurrentRes(8);
    m_Y1Res = 8;
    ui->xResDial->setCurrentRes(0);

    setUiEnable(false);

    connect(ui->spinBox,SIGNAL(valueChanged(int)),ui->triggerDial,SLOT(setValue(int)));

    connect(ui->actionConfigure,SIGNAL(triggered(bool)),this,SLOT(showConfigureDLG()));
    connect(ui->canal1GroupBox,SIGNAL(toggled(bool)),this,SLOT(toggleCanal1(bool)));
    connect(ui->canal2GroupBox,SIGNAL(toggled(bool)),this,SLOT(toggleCanal2(bool)));
    connect(ui->YRes1Dial,SIGNAL(resChanged(int)),this,SLOT(updateY1Res(int)));
    connect(ui->YRes2Dial,SIGNAL(resChanged(int)),this,SLOT(updateY2Res(int)));
    connect(ui->chan1ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChan1Coupling()));
    connect(ui->chan2ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChan2Coupling()));
    connect(ui->triggerDial,SIGNAL(valueChanged(int)),this,SLOT(updateTrigValue()));
    connect(ui->xResDial,SIGNAL(resChanged(int)),this,SLOT(updateTimeDiv()));
    connect(ui->actionZoom_In,SIGNAL(triggered(bool)),m_oscilloChart,SLOT(zoomInSlot()));
    connect(ui->actionZoom_out,SIGNAL(triggered(bool)),m_oscilloChart,SLOT(zoomOutSlot()));
    connect(ui->actionZoom_reset,SIGNAL(triggered(bool)),m_oscilloChart,SLOT(resetZoomSlot()));
    connect(m_toolActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(updateMode(QAction*)));
    connect(ui->invertTrigCheckBox,SIGNAL(toggled(bool)),this,SLOT(invertTrig(bool)));
    connect(ui->triggerCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTrigMode()));
    connect(ui->triggerCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTrigColor()));
    connect(&m_thread,SIGNAL(result(bool,QString,int)),this,SLOT(onCommandResult(bool,QString,int)));
    connect(&m_thread,SIGNAL(newDataAvailable(QByteArray,int)),m_oscilloChart,SLOT(updateData(QByteArray,int)));
    connect(&m_thread,SIGNAL(newTrigStateAvailable(bool)),this,SLOT(updateTrigState(bool)));
    connect(ui->mathsGroupBox,SIGNAL(toggled(bool)),this,SLOT(onMathFunction()));
    connect(ui->actionSave_current_graph,SIGNAL(triggered(bool)),this,SLOT(snapShot()));
    connect(ui->actionFullScreen,SIGNAL(triggered(bool)),this,SLOT(fullScreen()));
    connect(m_chartView,SIGNAL(leaveFullScreen()),this,SLOT(leaveFullScreen()));
    connect(ui->dockWidget,SIGNAL(leaveFullScreen()),this,SLOT(leaveFullScreen()));
    connect(ui->mathsComboBox,SIGNAL(currentIndexChanged(int)),m_oscilloChart,SLOT(updateMathMode(int)));
    connect(ui->invertCanal1CheckBox,SIGNAL(toggled(bool)),m_oscilloChart,SLOT(invertCanal1(bool)));
    connect(ui->invertCanal2CheckBox,SIGNAL(toggled(bool)),m_oscilloChart,SLOT(invertCanal2(bool)));
    connect(m_oscilloChart,SIGNAL(mathModeUpdated()),this,SLOT(onMathModeUpdated()));
    connect(m_oscilloChart,SIGNAL(zoomUpdated()),this,SLOT(updateTrigCursor()));
    connect(m_chartView,SIGNAL(changeMode(int)),this,SLOT(setMode(int)));
    connect(ui->dockWidget,SIGNAL(changeMode(int)),this,SLOT(setMode(int)));
    connect(ui->X1Slider,SIGNAL(valueChanged(int)),m_oscilloChart,SLOT(updateDeltaX1(int)));
    connect(ui->X2Slider,SIGNAL(valueChanged(int)),m_oscilloChart,SLOT(updateDeltaX2(int)));
    connect(ui->Y1Slider,SIGNAL(valueChanged(int)),m_oscilloChart,SLOT(updateDeltaY1(int)));
    connect(ui->Y2Slider,SIGNAL(valueChanged(int)),m_oscilloChart,SLOT(updateDeltaY2(int)));
    connect(m_modeActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(onModeUpdated(QAction*)));
    connect(ui->triggerDial,SIGNAL(valueChanged(int)),this,SLOT(updateTrigCursor()));
    connect(ui->triggerDial,SIGNAL(mousePressed()),this,SLOT(showTrigCur()));
    connect(ui->triggerDial,SIGNAL(mouseReleased()),this,SLOT(hideTrigCur()));
    connect(m_oscilloChart,SIGNAL(triggerLevelUpdated(int)),this,SLOT(updateTrigLevelFromView(int)));

    readSettings();

    connectToOscillo();

    m_chartView->setFocus();

    ui->infosTableWidget->resizeColumnToContents(0);
}

void MainWindow::showTrigCur(){
    m_chartView->trigCursor()->temporaryEnable(true);
}

void MainWindow::hideTrigCur(){
    m_chartView->trigCursor()->temporaryEnable(false);
}

void MainWindow::updateTrigState(bool triggered){
    ui->triggeredAction->setEnabled(triggered);
}

void MainWindow::onModeUpdated(QAction *action){

    switch(action->data().toInt()){
    case 0 :
        ui->actionStart->setEnabled(false);
        ui->actionStop->setEnabled(false);
        break;
    case 1 :
        ui->actionStart->setEnabled(true);
        break;
    }
}

void MainWindow::onMathModeUpdated(){
    if ( m_oscilloChart->mathMode() == OscilloChart::XY )
        ui->timeGroupBox->setEnabled(false);
    else
        ui->timeGroupBox->setEnabled(true);
}

void MainWindow::fullScreen(){
    m_geometry = saveGeometry();
    ui->dockWidget->setWindowTitle(tr("Press esc to exit full screen"));
    ui->menuBar->setVisible(false);
    ui->mainToolBar->setVisible(false);
    ui->statusBar->setVisible(false);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setWindowState(Qt::WindowFullScreen);
    showFullScreen();
    m_oscilloChart->setFocus();
}

void MainWindow::leaveFullScreen(){
    restoreGeometry(m_geometry);
    ui->dockWidget->setWindowTitle("");
    ui->menuBar->setVisible(true);
    ui->mainToolBar->setVisible(true);
    ui->statusBar->setVisible(true);
    setWindowFlags(Qt::Window);
    setWindowState(Qt::WindowActive);
    showNormal();
}

void MainWindow::snapShot(){
    m_chartView->save();
}

void MainWindow::onMathFunction(){
    ui->canal1GroupBox->setEnabled(!ui->mathsGroupBox->isChecked());
    ui->canal2GroupBox->setEnabled(!ui->mathsGroupBox->isChecked());
    if ( ui->mathsGroupBox->isChecked() ){
        m_oscilloChart->setMode(OscilloChart::Maths);
    }else{
        m_oscilloChart->setMode(OscilloChart::Classic);
    }
}

void MainWindow::updateParams(){
    getY1Res();
    getY2Res();
    getXRes();
    getTrigMod();
    getInvTrig();
}

void MainWindow::setUiEnable(bool enable){
    enable = true; // A SUPPR!!
    m_chartView->setEnabled(enable);
    ui->dockWidget->setEnabled(enable);
    ui->menuTools->setEnabled(enable);
    ui->menuMode->setEnabled(enable);
    ui->menuView->setEnabled(enable);
    m_toolActionGroup->setEnabled(enable);
    ui->actionFullScreen->setEnabled(enable);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::updateMode(QAction *action){
    switch(action->data().toInt()){
    case 0 :
        m_chartView->setMode(OscilloView::Zoom);
        break;
    case 1 :
        m_chartView->setMode(OscilloView::Ruler);        
        break;
    }
}

void MainWindow::setMode(int mode){
    if ( mode == 0 ){
        ui->actionZoom->setChecked(true);
        m_chartView->setMode(OscilloView::Zoom);
    }else{
        ui->actionRuler->setChecked(true);
        m_chartView->setMode(OscilloView::Ruler);
    }
}

void MainWindow::updateTrigMode(){
    m_trig = ui->triggerCombo->currentIndex();
    Command cmd { "TRIGMOD\n",QStringList("WAIT\r\n")};
    Command cmd2 { QString("%1\n").arg(m_trig+1),QStringList("OK\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_queue.enqueue(cmd2);
    m_thread.transaction(1000,m_queue,99);

    if ( m_trig == 3 ){
        ui->invertTrigCheckBox->setEnabled(false);
        ui->triggerDial->setEnabled(false);
        ui->triggerDial->setColor(QColor());
        ui->invertTrigCheckBox->setChecked(false);
        m_chartView->trigCursor()->setVisible(false);
    }else{
        ui->invertTrigCheckBox->setEnabled(true);
        ui->triggerDial->setEnabled(true);
        if ( m_trig == 0 )
            ui->triggerDial->setColor(m_settings.col1);
        if ( m_trig == 1 )
            ui->triggerDial->setColor(m_settings.col2);
        if ( m_trig == 2 )
            ui->triggerDial->setColor(Qt::gray);
        m_chartView->trigCursor()->setVisible(true);
    }
}

void MainWindow::getY1Res(){
    QStringList expectdReplies;
    expectdReplies << "1\r\n"<<"2\r\n"<<"3\r\n"<<"4\r\n"<<"5\r\n"<<"6\r\n"<<"7\r\n"<<"8\r\n"<<"9\r\n";

    Command cmd { "GETYRES1\n",expectdReplies};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_thread.transaction(1000,m_queue,2);
}

void MainWindow::getY2Res(){
    QStringList expectdReplies;
    expectdReplies << "1\r\n"<<"2\r\n"<<"3\r\n"<<"4\r\n"<<"5\r\n"<<"6\r\n"<<"7\r\n"<<"8\r\n"<<"9\r\n";

    Command cmd { "GETYRES2\n",expectdReplies};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_thread.transaction(1000,m_queue,3);
}

void MainWindow::getXRes(){
    QStringList expectdReplies;
    expectdReplies << "1\r\n"<<"2\r\n"<<"3\r\n"<<"4\r\n"<<"5\r\n"<<"6\r\n"<<"7\r\n"<<"8\r\n"<<"9\r\n"
                   <<"10\r\n"<<"11\r\n"<<"12\r\n"<<"13\r\n"<<"14\r\n"<<"15\r\n"<<"16\r\n"<<"17\r\n"
                   <<"18\r\n";

    Command cmd { "GETXRES\n",expectdReplies};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_thread.transaction(1000,m_queue,4);
}

void MainWindow::getTrigMod(){
    QStringList expectdReplies;
    expectdReplies << "1\r\n"<<"2\r\n"<<"3\r\n"<<"4\r\n"<<"5\r\n";

    Command cmd { "GETTRMOD\n",expectdReplies};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_thread.transaction(1000,m_queue,5);
}

void MainWindow::getInvTrig(){
    QStringList expectdReplies;
    expectdReplies << "0\r\n"<<"1\r\n";

    Command cmd { "GETITR\n",expectdReplies};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_thread.transaction(1000,m_queue,6);
}

void MainWindow::updateTimeDiv(){
    m_XRes = ui->xResDial->currentRes();
    Command cmd { "XRES\n",QStringList("WAIT\r\n")};
    Command cmd2 {QString("%1\n").arg(m_XRes+1),QStringList("OK\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_queue.enqueue(cmd2);
    m_thread.transaction(1000,m_queue,99);

    switch(m_XRes){
    case 0:
        m_oscilloChart->setTimePerDiv(1);
        break;
    case 1:
        m_oscilloChart->setTimePerDiv(2);
        break;
    case 2:
        m_oscilloChart->setTimePerDiv(5);
        break;
    case 3:
        m_oscilloChart->setTimePerDiv(10);
        break;
    case 4:
        m_oscilloChart->setTimePerDiv(20);
        break;
    case 5:
        m_oscilloChart->setTimePerDiv(50);
        break;
    case 6:
        m_oscilloChart->setTimePerDiv(100);
        break;
    case 7:
        m_oscilloChart->setTimePerDiv(200);
        break;
    case 8:
        m_oscilloChart->setTimePerDiv(500);
        break;
    case 9:
        m_oscilloChart->setTimePerDiv(1000);
        break;
    case 10:
        m_oscilloChart->setTimePerDiv(2000);
        break;
    case 11:
        m_oscilloChart->setTimePerDiv(5000);
        break;
    case 12:
        m_oscilloChart->setTimePerDiv(10000);
        break;
    case 13:
        m_oscilloChart->setTimePerDiv(20000);
        break;
    case 14:
        m_oscilloChart->setTimePerDiv(50000);
        break;
    case 15:
        m_oscilloChart->setTimePerDiv(100000);
        break;
    case 16:
        m_oscilloChart->setTimePerDiv(200000);
        break;
    case 17:
        m_oscilloChart->setTimePerDiv(500000);
        break;
    }
}

void MainWindow::updateY1Res(int res){
    m_Y1Res = res;

    Command cmd { "YRES\n",QStringList("WAIT\r\n")};
    Command cmd2 {QString("%1\n").arg(1),QStringList("WAIT\r\n")};
    Command cmd3 {QString("%1\n").arg(m_Y1Res+1),QStringList("OK\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_queue.enqueue(cmd2);
    m_queue.enqueue(cmd3);
    m_thread.transaction(1000,m_queue,99);

    setMvPerDiv(1, m_Y1Res);
}

void MainWindow::updateY2Res(int res){
    m_Y2Res = res;

    Command cmd { "YRES\n",QStringList("WAIT\r\n")};
    Command cmd2 {QString("%1\n").arg(2),QStringList("WAIT\r\n")};
    Command cmd3 {QString("%1\n").arg(m_Y2Res+1),QStringList("OK\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_queue.enqueue(cmd2);
    m_queue.enqueue(cmd3);
    m_thread.transaction(1000,m_queue,99);

    setMvPerDiv(2, m_Y2Res);
}

void MainWindow::updateChan1Coupling(){
    Command cmd { "COUP\n",QStringList("WAIT\r\n")};
    Command cmd2 {QString("%1\n").arg(1),QStringList("WAIT\r\n")};
    Command cmd3 {QString("%1\n").arg(ui->chan1ComboBox->currentText()),QStringList("OK\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_queue.enqueue(cmd2);
    m_queue.enqueue(cmd3);
    m_thread.transaction(1000,m_queue,99);
}

void MainWindow::updateChan2Coupling(){
    Command cmd { "COUP\n",QStringList("WAIT\r\n")};
    Command cmd2 {QString("%1\n").arg(2),QStringList("WAIT\r\n")};
    Command cmd3 {QString("%1\n").arg(ui->chan2ComboBox->currentText()),QStringList("OK\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_queue.enqueue(cmd2);
    m_queue.enqueue(cmd3);
    m_thread.transaction(1000,m_queue,99);
}

void MainWindow::updateTrigLevelFromView(int val){
    disconnect(ui->triggerDial,SIGNAL(valueChanged(int)),this,SLOT(updateTrigCursor()));
    ui->triggerDial->setValue(val);
    connect(ui->triggerDial,SIGNAL(valueChanged(int)),this,SLOT(updateTrigCursor()));
}

void MainWindow::updateTrigValue(){
    Command cmd { "TRIGV\n",QStringList("WAIT\r\n")};
    Command cmd2 {QString("%1\n").arg(ui->triggerDial->value()),QStringList("OK\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_queue.enqueue(cmd2);
    m_thread.transaction(1000,m_queue,99);
}

void MainWindow::updateTrigCursor(){
    m_chartView->setTrigPos(ui->triggerDial->value());
    m_chartView->update();
}

void MainWindow::updateTrigColor(){
    switch(ui->triggerCombo->currentIndex()){
        case 0:
            m_chartView->trigCursor()->setColor(m_settings.col1);
            break;
        case 1:
            m_chartView->trigCursor()->setColor(m_settings.col2);
            break;
        case 2:
            m_chartView->trigCursor()->setColor(Qt::gray);
            break;
        default:
            m_chartView->trigCursor()->setColor(Qt::gray);
            break;
    }
    m_chartView->scene()->invalidate();
}

void MainWindow::setMvPerDiv(int canal, int Yres){
    switch(Yres){
    case 0:
        m_oscilloChart->setmVPerDiv(canal,100);
        break;
    case 1:
        m_oscilloChart->setmVPerDiv(canal,200);
        break;
    case 2:
        m_oscilloChart->setmVPerDiv(canal,500);
        break;
    case 3:
        m_oscilloChart->setmVPerDiv(canal,1000);
        break;
    case 4:
        m_oscilloChart->setmVPerDiv(canal,2000);
        break;
    case 5:
        m_oscilloChart->setmVPerDiv(canal,5000);
        break;
    case 6:
        m_oscilloChart->setmVPerDiv(canal,10000);
        break;
    case 7:
        m_oscilloChart->setmVPerDiv(canal,20000);
        break;
    case 8:
        m_oscilloChart->setmVPerDiv(canal,50000);
        break;
    }
}

void MainWindow::invertTrig(bool invert){
    m_invTrig = invert;
    Command cmd { "INVTRIG\n",QStringList("WAIT\r\n")};
    Command cmd2 { QString("%1\n").arg(((int)invert)+1),QStringList("OK\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);
    m_queue.enqueue(cmd2);

    m_thread.transaction(1000,m_queue,99);
}

void MainWindow::toggleCanal1(bool show){
    m_oscilloChart->setCanalVisible(1,show);
}

void MainWindow::toggleCanal2(bool show){
    m_oscilloChart->setCanalVisible(2,show);
}

void MainWindow::showConfigureDLG(){
    SettingsDLG *dlg = new SettingsDLG(m_settings,this);
    if ( dlg->exec() == QDialog::Accepted ){
        m_settings = dlg->newSettings();
        updateView();
        connectToOscillo();
    }
}

void MainWindow::updateView(){
    ui->YRes1Dial->setColor(m_settings.col1);
    ui->YRes2Dial->setColor(m_settings.col2);
    ui->triggerDial->setColor(m_settings.col1);
    m_oscilloChart->setCanalColor(1,m_settings.col1);
    m_oscilloChart->setCanalColor(2,m_settings.col2);
    m_oscilloChart->setInterpolation(m_settings.interpolation);
    if ( m_settings.antiAliasing )
        m_chartView->setAntialiasing(true); //m_chartView->setRenderHint(QPainter::Antialiasing);
    else
        m_chartView->setAntialiasing(false);//m_chartView->setRenderHint(QPainter::NonCosmeticDefaultPen);
    m_chartView->trigCursor()->setColor(m_settings.col1);
}

void MainWindow::writeSettings(){
    QSettings settings("oscillo.ini", QSettings::IniFormat);
    settings.setValue("port", m_settings.port);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("widgets", saveState());
    settings.setValue("antialiasing", m_settings.antiAliasing);
    settings.setValue("color1", m_settings.col1);
    settings.setValue("color2", m_settings.col2);
    settings.setValue("interpolation", (int)m_settings.interpolation);
}

Settings const MainWindow::settings(){
    return m_settings;
}

void MainWindow::readSettings(){
    QSettings options("oscillo.ini", QSettings::IniFormat);
    m_settings.port = options.value("port").toString();
    m_settings.antiAliasing = options.value("antialiasing",false).toBool();
    m_settings.col1 = QColor(options.value("color1",QColor(Qt::white).name()).toString());
    m_settings.col2 = QColor(options.value("color2",QColor(Qt::green).name()).toString());
    m_settings.interpolation = (OscilloChart::Interpolation)options.value("interpolation",0).toInt();

    updateView();

    restoreGeometry(options.value("geometry").toByteArray());
    restoreState(options.value("widgets").toByteArray());

}

void MainWindow::closeEvent(QCloseEvent *event){
    Q_UNUSED(event)
    writeSettings();
}

void MainWindow::connectToOscillo(){
    m_thread.setPort(m_settings.port);
    Command cmd { "HELLO\n",QStringList("HELLO\r\n")};
    m_queue.clear();
    m_queue.enqueue(cmd);

    m_thread.transaction(1000,m_queue,1);

    updateParams();
}

void MainWindow::onCommandResult(bool success, QString error, int type){
    if ( !success )
        m_statusLabel->setText(error);
    switch ( type ){
        case 0:  // error not linked to a command
        break;
        case 1:  // result from connect command
            if ( success )
                m_statusLabel->setText(tr("Connected on %1").arg(m_settings.port));
            setUiEnable(success);
            break;
        case 2:  // Y1Res get value
            if ( success ){
                m_Y1Res = error.toInt() - 1;
                ui->YRes1Dial->setCurrentRes(m_Y1Res);
            }
            break;
        case 3:  // Y2Res get value
            if ( success ){
                m_Y2Res = error.toInt() - 1;
                ui->YRes2Dial->setCurrentRes(m_Y2Res);
            }
            break;
        case 4:  // XRes get value
            if ( success ){
                m_XRes = error.toInt() - 1;
                ui->xResDial->setCurrentRes(m_XRes);
            }
            break;
        case 5:  // TRIGMOD get value
            if ( success ){
                m_trig = error.toInt() - 1;
                ui->triggerCombo->setCurrentIndex(m_trig);
            }
            break;
        case 6:  // INVTRIG get value
            if ( success ){
                m_invTrig = (bool)error.toInt();
                ui->invertTrigCheckBox->setChecked(m_invTrig);
            }
            break;
        case 99:  // result from command with no user feedback
            break;
    }
}
