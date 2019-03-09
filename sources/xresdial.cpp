#include "xresdial.h"
#include "ui_xresdial.h"

XResDial::XResDial(QWidget *parent)
    : QWidget(parent), ui(new Ui::XResDial){
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
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(onValueChanged()));
    connect(ui->unitsComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onValueChanged()));
}

XResDial::~XResDial(){
    delete ui;
}

void XResDial::onValueChanged(){
    emit resChanged(ui->dial->value() + 9 * ui->unitsComboBox->currentIndex());
}

int XResDial::currentRes(){
    return ui->dial->value() + 9 * ui->unitsComboBox->currentIndex();
}

void XResDial::onBtnClick(int btn){
    ui->dial->setValue(btn);
}

void XResDial::setCurrentRes(int res){
    if ( res < 9 ){
        ui->dial->setValue(res);
        ui->unitsComboBox->setCurrentIndex(0);
    }else{
        ui->dial->setValue(res-9);
        ui->unitsComboBox->setCurrentIndex(1);
    }
}
