// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef PRINC_H
#define PRINC_H

#include "ui_princ.h"

class Princ : public QMainWindow, private Ui::Princ
{
    Q_OBJECT

public:
    explicit Princ(QWidget *parent = 0);

public slots:
    void setSliderRadius(double radius);

protected slots:
    void onSliderRadius(int value);
private slots:
    void on_checkBox_clicked();
    void on_pushButton_clicked();
    void on_horizontalSlider_2_valueChanged(int value);
    void on_horizontalSlider_valueChanged(int value);
};

#endif // PRINC_H
