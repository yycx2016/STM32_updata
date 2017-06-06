#ifndef FORM_YMJC_H
#define FORM_YMJC_H

#include <QWidget>
#include "qextserial/win_qextserialport.h"
#include <QTimer>
#include <QTime>
#include <QScrollBar>
#include <qmath.h>
#include "drawadvaluecurve.h"
#include <QtWidgets/QLabel>

namespace Ui {
class Form_ymjc;
}

class Form_ymjc : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form_ymjc(QWidget *parent = 0);
    ~Form_ymjc();
private:
    Ui::Form_ymjc *ui;
    QTimer *timer_getValue;
    DrawADValueCurve *ADValueCurve;
    QTime overtime;
    QLabel *Tabel[12];
    int n;
signals:
    void modeChanged();
    void sigADChanged();

private slots:
    void on_pushButton_start_clicked();
    void on_pushButton_close_clicked();
    void on_pushButton_clicked();
    void on_checkBox_CH1_clicked(bool checked);
    void on_checkBox_CH2_clicked(bool checked);
    void on_checkBox_CH3_clicked(bool checked);
    void on_checkBox_CH4_clicked(bool checked);

    void readMyCom();
    void timer_getValue_slot();


};

#endif // FORM_YMJC_H
