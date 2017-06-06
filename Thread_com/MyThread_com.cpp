//MyThread1.h
#include "MyThread_com.h"
#include "extern_data.h"

#include <QDebug>

/*******************线程*****************************************/
/************************线程 串口 流程控制****************************************/
/****************************************/
MyThread1::MyThread1()
{
    Step_num=0;
    start_flags=0;
    stepRun=0;
    connect(serialComTool,SIGNAL(Error_info(QString)),this,SLOT(Erroeinfosend(QString)));
}
void MyThread1::run()
{
    //stopthread_flag=false;
    while(!stopthread_flag)
    {
        if(start_flags)
        {
          ctl_serial_slot();
          //msleep(100);
        }
        else
        {
            msleep(500);
        }
    }
}
void MyThread1::set_data(int data)//更新运行序号
{
    stepRun=data;
    Step_num=0;
}
void MyThread1::set_ByteArraydata(QByteArray data)
{
    ba_in=data;
}
void MyThread1::Erroeinfosend(QString string_error)
{
    emit Error_info(string_error);
}

/**********串口控制*******************/
void MyThread1::ctl_serial_slot()
{
    //int i=0;
    //char sendDataBuf[7]={0x7e,0x00,0x00,0x00,0x00,0x00,0x18};
    switch(stepRun)
    {
    case Step_Wait: msleep(300);break;
    case COM_Program://1
    {
        unsigned char databuffer[4]={0};
        unsigned char data_ID[10];
        quint64 data_savenum=0;
        quint64 data_addr_all=0;
        quint64 block_num=0;
        int sum=0;

        QDataStream setout(&ba_in,QIODevice::ReadOnly);
        setout.device()->seek(0);
        setout>>data_savenum;
        setout>>data_addr_all;
        setout>>block_num;

        for(uint i=0;i<(data_savenum-data_savenum%4);i+=4)
        {
            if(!start_flags)return;
            setout>>databuffer[0];
            setout>>databuffer[1];
            setout>>databuffer[2];
            setout>>databuffer[3];

            sum=0;
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x07;
            data_ID[3]=0x03;
            data_ID[4]=0x03;
            data_ID[5]=databuffer[0];
            data_ID[6]=databuffer[1];
            data_ID[7]=databuffer[2];
            data_ID[8]=databuffer[3];
            for(int j=2;j<9;j++)
            {
                sum+=data_ID[j];
            }
            data_ID[9]=256-(sum&0xff);

            if(serialComTool->send_slot((const char *)&data_ID,10))
            {
                emit progressBar_info((i*100)/data_savenum);
            }
            else
            {
                Erroeinfosend(tr("升级失败！通信错误！"));
                stepRun=0;
                start_flags=false;
                return;
            }
            //msleep(10);
        }
        if(data_savenum%4!=0)
        {
            if(!start_flags)return;
            for(uint i=0;i<(data_savenum%4);i++)
            {
                setout>>databuffer[i];
            }
            for(uint i=data_savenum%4;i<4;i++)
            {
              databuffer[i]=0xff;
            }
            sum=0;
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x07;
            data_ID[3]=0x03;
            data_ID[4]=0x03;
            data_ID[5]=databuffer[0];
            data_ID[6]=databuffer[1];
            data_ID[7]=databuffer[2];
            data_ID[8]=databuffer[3];
            for(int j=2;j<9;j++)
            {
                sum+=data_ID[j];
            }
            data_ID[9]=256-(sum&0xff);
            if(serialComTool->send_slot((const char *)&data_ID,10))
            {
                emit progressBar_info(100);
                Erroeinfosend(tr("升级成功！"));
            }
            else
            {
                Erroeinfosend(tr("升级失败！通信错误！"));
                stepRun=0;
                start_flags=false;
                break;
            }
        }
        else
        {
            emit progressBar_info(100);
            Erroeinfosend(tr("升级成功！"));
        }
        stepRun=Step_Wait;
        start_flags=false;
    }break;
    case Auto_Program://一键自动升级2
    {
        switch(Step_num)
        {
        case Step_ChooseID://选择芯片
        {
            unsigned char data_ID[7];
            int sum=0;
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x04;
            data_ID[3]=0x00;
            data_ID[4]=0x01;
            data_ID[5]=programDataBuf[0];
            for(int i=2;i<6;i++)
            {
                sum+=data_ID[i];
            }
            data_ID[6]=256-(sum&0xff);
            if(serialComTool->send_slot((const char *)&data_ID,7))
            {
                Erroeinfosend(tr("选择芯片,成功！"));
                Step_num=Step_Unprotect;
                msleep(200);
            }
            else
            {
                Erroeinfosend(tr("选择芯片失败！通信错误！"));
                stepRun=Step_Wait;
                start_flags=false;
                isALLAutoUpdata=false;
                return;
            }
        }break;
        case Step_Unprotect://解除保护
        {
            unsigned char data_ID[6];
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x03;
            data_ID[3]=0x01;
            data_ID[4]=0x00;
            data_ID[5]=0xfc;
            if(serialComTool->send_slot((const char *)&data_ID,6))
            {
                Erroeinfosend(tr("解除保护,成功！"));
                Step_num=Step_EraseAddr;
            }
            else
            {
                Erroeinfosend(tr("解除保护失败！通信错误！"));
                stepRun=Step_Wait;
                start_flags=false;
                isALLAutoUpdata=false;
                return;
            }
        }break;
        case Step_EraseAddr://擦除首地址
        {
            unsigned char data_ID[10];
            int sum=0;
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x07;
            data_ID[3]=0x02;
            data_ID[4]=0x00;
            data_ID[5]=programDataBuf[1];
            data_ID[6]=programDataBuf[2];
            data_ID[7]=programDataBuf[3];
            data_ID[8]=programDataBuf[4];
            for(int i=2;i<9;i++)
            {
                sum+=data_ID[i];
            }
            data_ID[9]=256-(sum&0xff);
            if(serialComTool->send_slot((const char *)&data_ID,10))
            {
                Erroeinfosend(tr("擦除首地址,成功！"));
                Step_num=Step_ErasePageSize;
                msleep(100);
            }
            else
            {
                Erroeinfosend(tr("擦除首地址,失败！通信错误！"));
                stepRun=Step_Wait;
                start_flags=false;
                isALLAutoUpdata=false;
                return;
            }
        }break;
        case Step_ErasePageSize://擦除页大小
        {
            unsigned char data_ID[10];
            int sum=0;
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x07;
            data_ID[3]=0x02;
            data_ID[4]=0x01;
            data_ID[5]=programDataBuf[5];
            data_ID[6]=programDataBuf[6];
            data_ID[7]=programDataBuf[7];
            data_ID[8]=programDataBuf[8];
            for(int i=2;i<9;i++)
            {
                sum+=data_ID[i];
            }
            data_ID[9]=256-(sum&0xff);
            if(serialComTool->send_slot((const char *)&data_ID,10))
            {
                Erroeinfosend(tr("擦除页大小,成功！"));
                Step_num=Step_ErasePageNum;
                msleep(100);
            }
            else
            {
                Erroeinfosend(tr("擦除页大小,失败！通信错误！"));
                stepRun=Step_Wait;
                start_flags=false;
                isALLAutoUpdata=false;
                return;
            }
        }break;
        case Step_ErasePageNum://擦除总页数
        {
            unsigned char data_ID[10];
            int sum=0;
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x07;
            data_ID[3]=0x02;
            data_ID[4]=0x02;
            data_ID[5]=programDataBuf[9];
            data_ID[6]=programDataBuf[10];
            data_ID[7]=programDataBuf[11];
            data_ID[8]=programDataBuf[12];
            for(int i=2;i<9;i++)
            {
                sum+=data_ID[i];
            }
            data_ID[9]=256-(sum&0xff);
            if(serialComTool->send_slot((const char *)&data_ID,10))
            {
                Erroeinfosend(tr("擦除总页数,成功！"));
                Step_num=Step_Erase;
                msleep(100);
            }
            else
            {
                Erroeinfosend(tr("擦除总页数,失败！通信错误！"));
                stepRun=Step_Wait;
                start_flags=false;
                isALLAutoUpdata=false;
                return;
            }
        }break;
        case Step_Erase://擦除flash
        {
            unsigned char data_ID[6];
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x03;
            data_ID[3]=0x02;
            data_ID[4]=0x03;
            data_ID[5]=0xf8;
            if(serialComTool->send_slot((const char *)&data_ID,6))
            {
                Erroeinfosend(tr("擦除flash,成功！"));
                Step_num=Step_ProgramAddr;
                msleep(100);
            }
            else
            {
                Erroeinfosend(tr("擦除flash,失败！通信错误！"));
                stepRun=Step_Wait;
                start_flags=false;
                isALLAutoUpdata=false;
                return;
            }
        }break;
        case Step_ProgramAddr://设置Flash编程起始地址
        {
            unsigned char data_ID[10];
            int sum=0;
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x07;
            data_ID[3]=0x03;
            data_ID[4]=0x00;
            data_ID[5]=programDataBuf[13];
            data_ID[6]=programDataBuf[14];
            data_ID[7]=programDataBuf[15];
            data_ID[8]=programDataBuf[16];
            for(int i=2;i<9;i++)
            {
                sum+=data_ID[i];
            }
            data_ID[9]=256-(sum&0xff);
            if(serialComTool->send_slot((const char *)&data_ID,10))
            {
                Erroeinfosend(tr("编程首地址,成功！"));
                Step_num=Step_Program;
                msleep(100);
            }
            else
            {
                Erroeinfosend(tr("编程首地址,失败！通信错误！"));
                stepRun=Step_Wait;
                start_flags=false;
                isALLAutoUpdata=false;
                return;
            }
        }break;
        case Step_Program://写入
        {
            unsigned char databuffer[4]={0};
            unsigned char data_ID[10];
            quint64 data_savenum=0;
            quint64 data_addr_all=0;
            quint64 block_num=0;
            int sum=0;

            QDataStream setout(&ba_in,QIODevice::ReadOnly);
            setout.device()->seek(0);
            setout>>data_savenum;
            setout>>data_addr_all;
            setout>>block_num;

            for(uint i=0;i<(data_savenum-data_savenum%4);i+=4)
            {
                if(!start_flags)return;
                setout>>databuffer[0];
                setout>>databuffer[1];
                setout>>databuffer[2];
                setout>>databuffer[3];

                sum=0;
                data_ID[0]=0x55;
                data_ID[1]=0xaa;
                data_ID[2]=0x07;
                data_ID[3]=0x03;
                data_ID[4]=0x03;
                data_ID[5]=databuffer[0];
                data_ID[6]=databuffer[1];
                data_ID[7]=databuffer[2];
                data_ID[8]=databuffer[3];
                for(int j=2;j<9;j++)
                {
                    sum+=data_ID[j];
                }
                data_ID[9]=256-(sum&0xff);

                if(serialComTool->send_slot((const char *)&data_ID,10))
                {
                    emit progressBar_info((i*100)/data_savenum);
                }
                else
                {
                    Erroeinfosend(tr("写入失败！通信错误！"));
                    stepRun=0;
                    start_flags=false;
                    isALLAutoUpdata=false;
                    return;
                }
                //msleep(10);
            }
            if(data_savenum%4!=0)
            {
                if(!start_flags)return;
                for(uint i=0;i<(data_savenum%4);i++)
                {
                    setout>>databuffer[i];
                }
                for(uint i=data_savenum%4;i<4;i++)
                {
                  databuffer[i]=0xff;
                }
                sum=0;
                data_ID[0]=0x55;
                data_ID[1]=0xaa;
                data_ID[2]=0x07;
                data_ID[3]=0x03;
                data_ID[4]=0x03;
                data_ID[5]=databuffer[0];
                data_ID[6]=databuffer[1];
                data_ID[7]=databuffer[2];
                data_ID[8]=databuffer[3];
                for(int j=2;j<9;j++)
                {
                    sum+=data_ID[j];
                }
                data_ID[9]=256-(sum&0xff);
                if(serialComTool->send_slot((const char *)&data_ID,10))
                {
                    emit progressBar_info(100);
                    Erroeinfosend(tr("写入数据,成功！"));
                }
                else
                {
                    Erroeinfosend(tr("写入失败！通信错误！"));
                    stepRun=0;
                    start_flags=false;
                    isALLAutoUpdata=false;
                    return;
                }
            }
            else
            {
                emit progressBar_info(100);
                Erroeinfosend(tr("写入数据,成功！"));
            }
            Step_num=Step_ToUse;
        }break;
        case Step_ToUse://返回用户程序
        {
            unsigned char data_ID[6];
            data_ID[0]=0x55;
            data_ID[1]=0xaa;
            data_ID[2]=0x03;
            data_ID[3]=0x05;
            data_ID[4]=0x00;
            data_ID[5]=0xf8;

            if(serialComTool->send_slot((const char *)&data_ID,6))
            {
                Erroeinfosend(tr("返回用户程序,升级成功！"));
                stepRun=Step_Wait;
                Step_num=Step_ChooseID;
                start_flags=false;
            }
            else
            {
                Erroeinfosend(tr("返回用户程序,失败！通信错误！"));
                stepRun=Step_Wait;
                start_flags=false;
                isALLAutoUpdata=false;
                return;
            }

        }break;
        default:break;
        }
    }
    break;
    default : break;
    }
//return;
//serial_out1:
//    ;
//serial_erro:
//    serialComTool->comTimeout=0;
}

/**********各封装 函数*******************/
