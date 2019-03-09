#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>
#include <QtSerialPort/QSerialPortInfo>

#include "global.h"

namespace Ui {
class SettingsDLG;
}

class SettingsDLG : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDLG(Settings Settings, QWidget *parent = 0);
    ~SettingsDLG();
    Settings newSettings();

private slots:
    void updateLabel();

private:
    Ui::SettingsDLG *ui;

    void loadCombo();
    Settings m_settings;
};

#endif // SETTINGSDLG_H
