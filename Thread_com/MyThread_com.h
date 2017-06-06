#ifndef MYTHEAD1_H
#define MYTHEAD1_H
#include <QtGui>
#include "serialcom.h"

#define Step_ChooseID       0
#define Step_Unprotect      1
#define Step_EraseAddr      2
#define Step_ErasePageSize  3
#define Step_ErasePageNum   4
#define Step_Erase          5
#define Step_ProgramAddr    6
#define Step_Program        7
#define Step_ToUse          8

class MyThread1 : public QThread
{
        Q_OBJECT
public:
   MyThread1();

   int stepRun;

protected:
       void run();
private:
    QByteArray ba_in;
    int Step_num;

signals:
    void Error_info(const QString&);
    void progressBar_info(int progressBarvalue);

public slots:
    void set_data(int data);
    void set_ByteArraydata(QByteArray data);

private slots:
     void ctl_serial_slot();
     void Erroeinfosend(QString string_error);
    };

#endif // MYTHEAD1_H
