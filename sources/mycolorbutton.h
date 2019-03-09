#ifndef MYCOLORBUTTON_H
#define MYCOLORBUTTON_H

// button wich display a color square show a color dialog to chose a new one
// used in grid dialog

#include <QPushButton>

namespace Ui {
class MYColorButton;
}

class MYColorButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MYColorButton(QWidget *parent = 0);

    QColor color();
    void setColor(QColor color);

signals:
    void colorChanged(QColor newColor);

private slots:
    void showDLG();

private:
    void setCustomPixmap();

    Ui::MYColorButton *ui;
    QColor m_color;
};

#endif // MYCOLORBUTTON_H
