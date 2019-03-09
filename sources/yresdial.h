#ifndef YRESDIAL_H
#define YRESDIAL_H

#include <QWidget>
#include <QButtonGroup>

namespace Ui {
class YResDial;
}

class YResDial : public QWidget
{
    Q_OBJECT

public:
    explicit YResDial(QWidget *parent = 0);
    ~YResDial();
    void setCurrentRes(int res);
    void setColor(QColor color);

signals:
    void resChanged(int res);

private slots:
    void onBtnClick(int btn);

private:
    Ui::YResDial *ui;
    QButtonGroup *m_btnGrp;
};

#endif // YRESDIAL_H
