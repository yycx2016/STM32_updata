#ifndef FORM_TOOL_H
#define FORM_TOOL_H

#include <QWidget>
#include <QFile>
#include <QTimer>
#include <QTime>
#include <QSocketNotifier>
#include <QLineEdit>

namespace Ui {
class Form_tool;
}

class Form_tool : public QWidget
{
    Q_OBJECT

public:
    explicit Form_tool(QWidget *parent = 0);
    ~Form_tool();

    QSocketNotifier *serial_notifier;



private slots:

    void send_solt_GZZ(QString user_input);
    void auto_sent_interval_solt();
    void updateList(bool flag);
    void readMyCom();//串口接收函数
    int send_GZZ(void);//选择发送指令
    void on_pushButton_send_1_clicked();
    void on_pushButton_send_2_clicked();
    void on_pushButton_send_3_clicked();
    void on_pushButton_send_4_clicked();
    void on_pushButton_send_5_clicked();
    void on_pushButton_send_6_clicked();
    void on_pushButton_send_7_clicked();
    void on_pushButton_send_8_clicked();
    void on_pushButton_send_9_clicked();
    void on_pushButton_send_10_clicked();
    void on_pushButton_send_11_clicked();
    void on_pushButton_send_12_clicked();

    void on_pushButton_savefile_clicked();

    void on_pushButton_saveList_clicked();

    void on_pushButton_cycle_clicked();

    void on_pushButton_clearDisplay_clicked();

    void on_checkBox_16_clicked(bool checked);

    void on_pushButton_mode_clicked();
    void on_lineEdit_startFlag_textChanged(const QString &arg1);

    void on_spinBox_dataNum_valueChanged(int arg1);

    void on_pushButton_CX_clicked();

    void on_pushButton_OldAsk_clicked();

    void on_pushButton_NewAsk_clicked();

    void on_pushButton_V_Min_old_clicked();

    void on_pushButton_V_Min_new_clicked();

    void on_pushButton_V_Max_old_clicked();

    void on_pushButton_V_Max_new_clicked();

    void on_pushButton_V_Accr_old_clicked();

    void on_pushButton_V_Accr_new_clicked();

    void on_pushButton_V_Even_old_clicked();

    void on_pushButton_V_Even_new_clicked();

    void on_pushButton_V_Reset_old_clicked();

    void on_pushButton_V_Reset_new_clicked();

    void on_pushButton_setpSet_clicked();

    void on_pushButton_DirSet_clicked();

    void on_pushButton_Enanble_clicked();

    void on_pushButton_Run_Accr_clicked();

    void on_pushButton_Run_Even_clicked();

    void on_pushButton_Run_Reset_clicked();

    void on_pushButton_Run_Accr_new_clicked();

    void on_pushButton_Run_Even_new_clicked();

    void on_pushButton_Run_Reset_new_clicked();

    void on_pushButton_ReadDG_clicked();

    void on_pushButton_InitDG_clicked();

    void on_pushButton_ReadOP_clicked();

    void on_pushButton_YM_CX_clicked();

    void on_pushButton_YM_Ask_clicked();

    void on_pushButton_YMJC_clicked();

    void on_pushButton_YM_AD_clicked();

    void on_pushButton_YM_DCF_clicked();

    void on_pushButton_YM_ERROR_clicked();

    void on_pushButton_YM_SET_clicked();

    void on_pushButton_YM_STOP_clicked();

    void on_pushButton_YM_Return_clicked();

    void on_pushButton_YM_MotorRun_clicked();

    void on_pushButton_YM_MotorAsk_clicked();

    void on_checkBox_automated_clicked(bool checked);

    void on_pushButton_condition_clicked(bool checked);

    void on_pushButton_open_clicked();

signals:
    void modeChanged();
private:
    Ui::Form_tool *ui;

    bool is16_;

    unsigned char to_sent_data_buf[1];
    unsigned char to_sent_data[1024];
    unsigned char to_recv_data[1024];
    int receive_byte_counter;
    int receive_counter;
    int sent_byte_counter;
    quint8 receivebuf[200];
    int n;
    QString string_recv;
    QTime t_temp;

    QTimer *auto_sent_interval;
    QFile saved;
    QFile file;

    QList <QLineEdit*> LineEdits_auto ;
    QList <QLineEdit*> LineEdits;
};

#endif // FORM_TOOL_H
