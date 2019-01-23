#include "serialcom.h"
#include "extern_data.h"
#include <sys/time.h>
#include <sys/types.h>
//#include <sys/ipc.h>
//#include <sys/ioctl.h>
#include <sys/stat.h>
//#include <sys/select.h>
#include <sys/stat.h>
#include <stdio.h> //标准输入输出定义
#include <fcntl.h> //文件控制定义
#include <errno.h> //错误代码定义
#include <unistd.h> //unix标准函数定义
//#include <termios.h>//定义串口结构termios，并包含<termbits.h>
#include <string.h> //字符串函数定义
#include <math.h>
#include <stdlib.h>
#include <QDebug>
#include <QTime>
serialCOM::serialCOM()
{


    comDevice = new Win_QextSerialPort();//定义串口对象
    comDevice->close();

}

/*************串口配置***************************/
bool serialCOM::opencom(const QString &com_Num,
                        int com_Buad, char com_Testbit,int com_Databit , int com_Stopbit)
{
    bool serial_fd;
    comDevice->close();
//定义串口对象
    comDevice->setPortName(com_Num);//com_Num
//以读写方式打开串口
    serial_fd=comDevice ->open(QIODevice::ReadWrite);
//波特率设置，我们设置为57600
      if(com_Buad == 50)
              comDevice->setBaudRate(BAUD50);//setBaudRate(BAUD50;
      else    if(com_Buad == 75)
              comDevice->setBaudRate(BAUD75);
      else	if(com_Buad == 110)
              comDevice->setBaudRate(BAUD110);
      else	if(com_Buad == 134)
              comDevice->setBaudRate(BAUD134);
      else	if(com_Buad == 150)
              comDevice->setBaudRate(BAUD150);
      else	if(com_Buad == 200)
              comDevice->setBaudRate(BAUD200);
      else	if(com_Buad == 300)
              comDevice->setBaudRate(BAUD300);
      else	if(com_Buad == 600)
              comDevice->setBaudRate(BAUD600);
      else	if(com_Buad == 1200)
             comDevice->setBaudRate(BAUD1200);
      else	if(com_Buad == 1800)
              comDevice->setBaudRate(BAUD1800);
      else	if(com_Buad == 2400)
              comDevice->setBaudRate(BAUD2400);
      else	if(com_Buad == 4800)
              comDevice->setBaudRate(BAUD4800);
      else	if(com_Buad == 9600)
              comDevice->setBaudRate(BAUD9600);
      else	if(com_Buad == 19200)
              comDevice->setBaudRate(BAUD19200);
      else	if(com_Buad == 38400)
              comDevice->setBaudRate(BAUD38400);
      else	if(com_Buad == 57600)
              comDevice->setBaudRate(BAUD57600);
      else	if(com_Buad == 115200)
              comDevice->setBaudRate(BAUD115200);
      else	if(com_Buad == 230400)
              comDevice->setBaudRate(BAUD256000);
      else
              comDevice->setBaudRate(BAUD9600);
    //奇偶校验设置，我们设置为无校验
      if(com_Testbit == 'n'||com_Testbit == 'N') comDevice->setParity(PAR_NONE);
      else if(com_Testbit == 'e'||com_Testbit == 'E')comDevice->setParity(PAR_EVEN);
      else if(com_Testbit == 'o'||com_Testbit == 'O')comDevice->setParity(PAR_ODD);
      else if(com_Testbit == 'm'||com_Testbit == 'M')comDevice->setParity(PAR_MARK);
      else if(com_Testbit == 's'||com_Testbit == 'S')comDevice->setParity(PAR_SPACE);
      else comDevice->setParity(PAR_NONE);
    //数据位设置，我们设置为8位数据位
      if(com_Databit == 5) comDevice->setDataBits(DATA_5);
      else if(com_Databit == 6)comDevice->setDataBits(DATA_6);
      else if(com_Databit == 7)comDevice->setDataBits(DATA_7);
      else if(com_Databit == 8)comDevice->setDataBits(DATA_8);
      else comDevice->setDataBits(DATA_8);
    //停止位设置，我们设置为1位停止位
      if(com_Stopbit == 1)comDevice->setStopBits(STOP_1);
      else if(com_Stopbit == 2)comDevice->setStopBits(STOP_2);
      //else if(com_Stopbit == "1.5") comDevice->setStopBits(STOP_1_5);
      else comDevice->setStopBits(STOP_1);
    //数据流控制设置，我们设置为无数据流控制
    comDevice->setFlowControl(FLOW_OFF);
//    if(com_QueryMode==0)
//    {
//       comDevice->setQueryMode(QextSerialBase::Polling);//Polling
//       comDevice->setTimeout(-1);
//    }
//    else
//    {
//        comDevice->setQueryMode(QextSerialBase::EventDriven);//EventDriven
//        comDevice->setTimeout(100);
//    }
    comDevice->setTimeout(-1);
    comDevice->setQueryMode(QextSerialBase::EventDriven);//EventDriven

    //connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));
    return serial_fd;

}
void serialCOM::closecom()
{
   comDevice->close();
}
QString string_errorinfo;
/*******串口控制调用子程序******************/
QTime t1;
bool serialCOM::send_slot(const char *sendDataBuf,int len)
{
    int nByte = -1;
    char receiveData[0];
    while(comDevice->bytesAvailable())
    {
       comDevice->readAll();
       comDevice->seek(0);
       comDevice->flush();
       usleep(10000);
       emit Error_info("readAll-------");
       //qDebug()<<"readAll-------";
    }

    nByte = comDevice->write(sendDataBuf,len);
    //if(nByte==-1) goto erro_out;
    //qDebug(string)<<"send num="<<nByte<<"data="<<(int)sendDataBuf[0]<<(int)sendDataBuf[1]<<(int)sendDataBuf[2]<<(int)sendDataBuf[3]<<(int)sendDataBuf[6];
    string_errorinfo.clear();
    for(int i=0;i<len;i++)
    {
        string_errorinfo+=tr("%1 ").arg((uchar)sendDataBuf[i],0,16);
    }
    emit Error_info(tr("send num=%1 data=%2")
                        .arg(nByte)
                        .arg(string_errorinfo)
                        );
    //qDebug("sendDataBuf=%d",sendDataBuf[0]);
    //03 02 03
    if((uchar)sendDataBuf[2]==3&&(uchar)sendDataBuf[3]==2&&(uchar)sendDataBuf[4]==3)
    {
        //qDebug()<<removepagenum;
        for(int i=0;i<removepagenum;i++)
        {
          usleep(20000);
        }

    }
    int readnum=3;
    if((uchar)sendDataBuf[2]==3&&(uchar)sendDataBuf[3]==4&&(uchar)sendDataBuf[4]==3)
    {
        readnum=6;
    }
    t1.start();
    for(int i=0;i<readnum;i++)
    {
        t1.restart();
        while((comDevice->read((char *)&receiveData , 1))<=0)
        {
          if((uchar)sendDataBuf[2]==3&&(uchar)sendDataBuf[3]==2&&(uchar)sendDataBuf[4]==3)
          {
              if(t1.elapsed()>=5000) goto erro_out;
          }
          else
          {
              if(t1.elapsed()>=1500) goto erro_out;
          }

          //usleep(300000);
        }
        receiveDataBuf[i]=(unsigned char)(receiveData[0]);
    }
   // nByte=comDevice->read((char *)&receiveDataBuf , 10);
    //if(nByte==-1) goto erro_out;
    //qDebug(string)<<"read num="<<nByte<<"data="<<receiveDataBuf[0]<<receiveDataBuf[1]<<receiveDataBuf[2]<<receiveDataBuf[3];
    string_errorinfo.clear();
    for(int i=0;i<readnum;i++)
    {
        string_errorinfo+=tr("%1 ").arg((uchar)receiveDataBuf[i],0,16);
    }
    emit Error_info(tr("read num=%1 data=%2")
                        .arg(nByte)
                        .arg(string_errorinfo)
                        );
    return true;
 erro_out:
   // qDebug()<<"read erro---------------------------------";
    emit Error_info("read erro---------------------------------");
    return false;
}
