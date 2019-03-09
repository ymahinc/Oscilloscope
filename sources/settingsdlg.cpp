#include "settingsdlg.h"
#include "ui_settingsdlg.h"

#include "global.h"

SettingsDLG::SettingsDLG(Settings Settings, QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDLG){
    ui->setupUi(this);

    m_settings = Settings;

    ui->antiAliasingCheckBox->setChecked(m_settings.antiAliasing);
    ui->canal1ColorPushButton->setColor(m_settings.col1);
    ui->canal2ColorPushButton->setColor(m_settings.col2);
    ui->extrapolationComboBox->setCurrentIndex(m_settings.interpolation);

    connect(ui->portComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateLabel()));

    loadCombo();
}

SettingsDLG::~SettingsDLG(){
    delete ui;
}

void SettingsDLG::loadCombo(){
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos){
        ui->portComboBox->addItem(info.portName(),info.description());
        if ( info.portName() == m_settings.port )
            ui->portComboBox->setCurrentIndex(ui->portComboBox->count()-1);
    }
}

void SettingsDLG::updateLabel(){
    QString description = ui->portComboBox->currentData().toString();
    ui->descriptionLabel->setText(description);
}

Settings SettingsDLG::newSettings(){
    m_settings.port = ui->portComboBox->currentText();
    m_settings.antiAliasing = ui->antiAliasingCheckBox->isChecked();
    m_settings.col1 = ui->canal1ColorPushButton->color();
    m_settings.col2 = ui->canal2ColorPushButton->color();
    m_settings.interpolation = (OscilloChart::Interpolation)ui->extrapolationComboBox->currentIndex();
    return m_settings;
}
