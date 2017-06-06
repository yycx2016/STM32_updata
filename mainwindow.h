#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "Thread_com/MyThread_com.h"
#include "serialcom.h"
#include "form_tool.h"
#include "form_stm32updatatool.h"
#include "form_ymjc.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Form_tool * serialTool;
    Form_stm32UpdataTool *stm32UpdataTool;
    Form_ymjc *QyYmJC;
     QLabel *label_comStatus ;


private slots:
    void updataComStatus();
    void foreachComPort();
    void openSerial_slot();// 打开串口
    void on_openCloseSerialPushButton_clicked();
    void on_pushButton_dis_Help_clicked();

    void on_comboBox_mode_currentIndexChanged(int index);
    void on_tabWidget_currentChanged(int index);
};

#endif // MAINWINDOW_H
