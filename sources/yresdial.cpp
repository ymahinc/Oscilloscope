#include "yresdial.h"
#include "ui_yresdial.h"

YResDial::YResDial(QWidget *parent)
    : QWidget(parent), ui(new Ui::YResDial){
    ui->setupUi(this);
    m_btnGrp = new QButtonGroup(this);
    m_btnGrp->addButton(ui->res1Button,0);
    m_btnGrp->addButton(ui->res2Button,1);
    m_btnGrp->addButton(ui->res3Button,2);
    m_btnGrp->addButton(ui->res4Button,3);
    m_btnGrp->addButton(ui->res5Button,4);
    m_btnGrp->addButton(ui->res6Button,5);
    m_btnGrp->addButton(ui->res7Button,6);
    m_btnGrp->addButton(ui->res8Button,7);
    m_btnGrp->addButton(ui->res9Button,8);
    connect(m_btnGrp,SIGNAL(buttonClicked(int)),this,SLOT(onBtnClick(int)));
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SIGNAL(resChanged(int)));
}

void YResDial::setColor(QColor color){
    ui->dial->setColor(color);
}

void YResDial::onBtnClick(int btn){
    ui->dial->setValue(btn);
}

YResDial::~YResDial(){
    delete ui;
}

void YResDial::setCurrentRes(int res){
    ui->dial->setValue(res);
}
