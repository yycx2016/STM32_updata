#ifndef EXTERN_DATA_H
#define EXTERN_DATA_H

#include <QDebug>
#include <QMessageBox>

#include "mydefine.h"
#include "serialcom.h"

#define Step_Wait 0
#define COM_Program 1
#define Auto_Program 2
//extern
extern volatile bool stopthread_flag;
extern bool com_status;
extern bool start_flags;
extern bool isALLAutoUpdata;

extern unsigned char receiveDataBuf[10];
extern int removepagenum;
extern unsigned char programDataBuf[20];
extern int gCom_QueryMode;

extern serialCOM *serialComTool;

#endif // EXTERN_DATA_H
