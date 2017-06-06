#ifndef FORM_STM32UPDATATOOL_H
#define FORM_STM32UPDATATOOL_H

#include <QWidget>
#include "Thread_com/MyThread_com.h"
#include "serialcom.h"
#include <QProgressBar>
#include <QFileSystemWatcher>

namespace Ui {
class Form_stm32UpdataTool;
}

class Form_stm32UpdataTool : public QWidget
{
    Q_OBJECT

public:
    explicit Form_stm32UpdataTool(QWidget *parent = 0);
    QProgressBar   *progressBar;
    ~Form_stm32UpdataTool();

private slots:
    void timer_ctl_slot();

    void infoshow_slot(QString cominfo);

    void on_pushButton_openfile_clicked();

    void on_pushButton_ID_clicked();

    void on_pushButton_remove_clicked();

    void on_pushButton_addr_clicked();

    void on_pushButton_page_size_clicked();

    void on_pushButton_page_clicked();

    void on_pushButton_clean_clicked();

    void on_pushButton_programaddr_clicked();

    void on_pushButton_updata_clicked();

    void on_pushButton_user_clicked();

    void on_pushButton_AutoUpdata_clicked();

    void on_pushButton_IDUpdata_clicked();

    void on_pushButton_clicked();

    void on_pushButton_mode_clicked();
public slots:
    void showMessage(QString path);

signals:
    void modeChanged();

private:
    Ui::Form_stm32UpdataTool *ui;

    bool isFileOpen;

    int char_count;
    unsigned char to_sent_data[100];
    int cnt_ID;


    QTime t_last;

    MyThread1*mythread_com;
    QTimer *timer_ctl;
    //serialCOM *myserialcom;
    QByteArray inBlock;

    QFileSystemWatcher fsWatcher;
    void updataFile(QString fileName);
};

#endif // FORM_STM32UPDATATOOL_H
