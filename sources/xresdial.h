#ifndef XRESDIAL_H
#define XRESDIAL_H

#include <QWidget>
#include <QButtonGroup>

namespace Ui {
class XResDial;
}

class XResDial : public QWidget
{
    Q_OBJECT

public:
    explicit XResDial(QWidget *parent = 0);
    ~XResDial();
    void setCurrentRes(int res);
    int currentRes();

signals:
    void resChanged(int res);

private slots:
    void onBtnClick(int btn);
    void onValueChanged();

private:
    Ui::XResDial *ui;
    QButtonGroup *m_btnGrp;
};

#endif // XRESDIAL_H
