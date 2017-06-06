#include "form_stm32updatatool.h"
#include "ui_form_stm32updatatool.h"
#include "extern_data.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QWhatsThis>
#include <QRegExp>

Form_stm32UpdataTool::Form_stm32UpdataTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_stm32UpdataTool)
{
    isFileOpen=false;
    char_count = 0;
    cnt_ID = 0;
    ui->setupUi(this);
    progressBar = new QProgressBar(this);
    progressBar->setValue(0);

    ui->frame_step->setDisabled(true);
    /*************************************************************************/
    mythread_com=new MyThread1();
    connect(mythread_com,SIGNAL(Error_info(QString)),this,SLOT(infoshow_slot(QString)));
    connect(mythread_com,SIGNAL(progressBar_info(int)),progressBar,SLOT(setValue(int)));
    /*************************************************************************/
    timer_ctl=new QTimer();
    connect(timer_ctl,SIGNAL(timeout()),this,SLOT(timer_ctl_slot()));
    timer_ctl->start(1000);
    /*************************************************************************/


}

void Form_stm32UpdataTool::showMessage(QString path)
{
    QString fileName = path.section("/",-1,-1);
    QMessageBox::information(NULL,"文件已改变"," 文件 " + fileName + " 已被修改！ ");
}

Form_stm32UpdataTool::~Form_stm32UpdataTool()
{
    delete ui;
}

void Form_stm32UpdataTool::timer_ctl_slot()
{
    timer_ctl->stop();
    if(isALLAutoUpdata)
    {
        if(!start_flags&&mythread_com->stepRun ==Step_Wait)
        {
            ui->label_IDInfo->setText(tr("升级：%1").arg(to_sent_data[cnt_ID]));
            ui->spinBox_ID->setValue(to_sent_data[cnt_ID]);
            on_pushButton_AutoUpdata_clicked();
            cnt_ID++;
            if(cnt_ID>=char_count)
            {
                isALLAutoUpdata = false;
                char_count = 0;
                cnt_ID = 0;
                ui->label_IDInfo->setText(tr("升级完成"));
            }
        }
    }
    else
    {
        if((start_flags&&mythread_com->stepRun>0) || (!com_status) || (!isFileOpen)) // 正在烧写
        {
            ui->frame_step->setDisabled(true);
        }
        else
        {
            ui->frame->setEnabled(true);
            ui->frame_step->setEnabled(true);
        }
    }
    timer_ctl->start(1000);
}

void Form_stm32UpdataTool::infoshow_slot(QString cominfo)
{
    ui->textBrowser_2->append(cominfo);
}

void Form_stm32UpdataTool::on_pushButton_openfile_clicked()// 打开文件
{
    int i=0;
    QDir currentFileDir;
    if (!QFile::exists("./filepath.ini"))//文件路径
    {
        QSettings settings("./filepath.ini", QSettings::IniFormat);
        settings.setValue("FilePath/currentpath", "./");
    }
    QSettings settings("./filepath.ini", QSettings::IniFormat);
    currentFileDir.setPath(settings.value("FilePath/currentpath").toString());

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                              currentFileDir.path(),
                                              "HEX file(*.hex);;Any files (*)");

    fsWatcher.addPath(fileName);
//    qDebug()<< fileName;
    connect(&fsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(showMessage(QString)));//监控所打开的文件是否变化

    if(!fileName.isEmpty() )
    {
        QFile file_in(fileName);
        if (file_in.open(QIODevice::ReadOnly))
        {
            //currentFileDir.setPath(fileName);
            settings.setValue("FilePath/currentpath", fileName);

            QByteArray updatafile;
            ui->textBrowser->clear();
            updatafile.clear();
            updatafile=file_in.readAll();
            //qDebug()<<"filesize="<<updatafile.size();
            ui->textBrowser->setPlainText(updatafile);
            file_in.close();
           //qDebug()<<"Open file";
            //ui->textBrowser_2->append(tr("文件打开成功，数据格式16进制"));
            //qDebug()<<fileName<<fileName.section("/",-1,-1).section('.',0,0).section('_',1,1);
            ui->label_filename->setText(fileName.section("/",-1,-1));
            this->setWindowTitle(tr("STM32_CAN在线升级软件 - ").append(ui->label_filename->text()));
            ui->spinBox_ID->setValue(fileName.section("/",-1,-1).section('.',0,0).section('_',-1,-1).toInt());
            if((fileName.section("/",-1,-1).section('.',0,0).section('_',-2,-2)=="1k")
             ||(fileName.section("/",-1,-1).section('.',0,0).section('_',-2,-2)=="1K"))
            {
                ui->comboBox_page_size->setCurrentIndex(0);//页数大小
            }
            else
            {
                ui->comboBox_page_size->setCurrentIndex(1);//页数大小
            }


            QDataStream setin(&inBlock,QIODevice::WriteOnly);
            setin.device()->seek(0);
            setin << quint64(0) << quint64(0) <<quint64(0) ;
            quint64 data_count=0;
            quint64 block_num=0;
            quint64 data_addr_all=0;
            quint64 data_savenum=0;

            for(i=0;i<updatafile.size();i++)
            {
                if(updatafile.at(i)==':')
                {
                    unsigned char datasize=0;
                    unsigned char data_type=0;
                    unsigned char data_check=0;
                    unsigned char data_addr[2]={0,0};
                    QByteArray ba;
                    QString stringbuffer;
                    data_count++;

                    /******[数据长度1Byte]******/
                    ba.clear();
                    ba=ba.append(updatafile.at(++i));
                    ba=ba.append(updatafile.at(++i));
                    datasize=ba.toInt(0,16);
                    //qDebug()<<"datasize="<<datasize;
                    /******[数据地址2Byte]******/
                    ba.clear();
                    ba=ba.append(updatafile.at(++i));
                    ba=ba.append(updatafile.at(++i));
                    data_addr[0]=ba.toInt(0,16);
                    ba.clear();
                    ba=ba.append(updatafile.at(++i));
                    ba=ba.append(updatafile.at(++i));
                    data_addr[1]=ba.toInt(0,16);
                    //qDebug()<<"data_addr_H="<<data_addr[0]<<"data_addr_L="<<data_addr[1];
                    /******[数据类型1Byte]******/
                    ba.clear();
                    ba=ba.append(updatafile.at(++i));
                    ba=ba.append(updatafile.at(++i));
                    data_type=ba.toInt(0,16);
                    //qDebug()<<"data_type="<<data_type;
                    /******[数据]******/
                    unsigned char data_buffer[datasize];
                    stringbuffer.clear();
                    for(int j=0;j<datasize;j++)
                    {
                        ba.clear();
                        ba=ba.append(updatafile.at(++i));
                        ba=ba.append(updatafile.at(++i));
                        data_buffer[j]=ba.toInt(0,16);
                        stringbuffer=stringbuffer.append(tr("%1 ").arg(data_buffer[j],0,16));
                        if(data_type==0)
                        {
                            setin<<data_buffer[j];
                            data_savenum++;
                        }
                    }
                    //qDebug()<<"data_buffer="<<stringbuffer;
                    /******[校验1Byte]******/
                    ba.clear();
                    ba=ba.append(updatafile.at(++i));
                    ba=ba.append(updatafile.at(++i));
                    data_check=ba.toInt(0,16);
                    //qDebug()<<"data_check="<<data_check;
                    /********************************************************************/
                    if(data_type==0)//数据
                    {
                        block_num++;
                        if(data_count==2)
                        {
                            data_addr_all=data_addr_all+(data_addr[0]<<8)+(data_addr[1]);
                        }
                    }
                    else if(data_type==1)//结束
                    {
                        //block_num=0x100;
                        int data_pagenum=data_savenum/1024;
                        if(data_savenum%1024!=0)data_pagenum+=1;
                        ui->lineEdit_page->setText(tr("0x%1").arg(data_pagenum,0,16));//擦除总页数
                        ui->lineEdit_addr->setText(tr("0x%1").arg(data_addr_all,0,16));//擦除首地址
                        ui->lineEdit_programaddr->setText(tr("0x%1").arg(data_addr_all,0,16));//编程首地址
                        ui->lineEdit_sum->setText(tr("0x%1").arg(block_num,0,16));//总帧数
                        ui->lineEdit_datanum->setText(tr("0x%1").arg(data_savenum,0,16));//数据个数
                        //ui->lineEdit_page_size->setText(tr("800"));//页数大小
                        setin.device()->seek(0);
                        setin<<(quint64)data_savenum;
                        setin<<(quint64)data_addr_all;
                        setin<<(quint64)block_num;
                        isFileOpen=true;
                    }
                    else if(data_type==4)//首地址
                    {
                        if(data_count==1&&datasize==2)
                        {
                            data_addr_all=((data_buffer[0]<<8)+data_buffer[1]);
                            data_addr_all=data_addr_all<<16;
                        }
                    }
                    if(0)
                    {
                        switch(data_type)
                        {
                        case 0:
                            ui->textBrowser_2->append(tr("第%1帧 长度:%2 地址:%3 %4 类型:%5 数据帧")
                                                      .arg(data_count,0,10)
                                                      .arg(datasize,0,16)
                                                      .arg(data_addr[0], 0, 16)
                                                      .arg(data_addr[1], 0, 16)
                                                      .arg(data_type,0,16)
                                                      );
                            break;
                        case 1:
                            ui->textBrowser_2->append(tr("第%1帧 长度:%2 地址:%3 %4 类型:%5 结束帧")
                                                      .arg(data_count,0,10)
                                                      .arg(datasize,0,16)
                                                      .arg(data_addr[0], 0, 16)
                                                      .arg(data_addr[1], 0, 16)
                                                      .arg(data_type,0,16)
                                                      );
                            break;
                        case 2:
                        case 4:
                            ui->textBrowser_2->append(tr("第%1帧 长度:%2 地址:%3 %4 类型:%5 地址帧")
                                                      .arg(data_count,0,10)
                                                      .arg(datasize,0,16)
                                                      .arg(data_addr[0], 0, 16)
                                                      .arg(data_addr[1], 0, 16)
                                                      .arg(data_type,0,16)
                                                      );

                            break;
                        case 5:
                            ui->textBrowser_2->append(tr("第%1帧 长度:%2 地址:%3 %4 类型:%5 开始线性地址帧")
                                                      .arg(data_count,0,10)
                                                      .arg(datasize,0,16)
                                                      .arg(data_addr[0], 0, 16)
                                                      .arg(data_addr[1], 0, 16)
                                                      .arg(data_type,0,16)
                                                      );
                            break;
                        default :
                            ui->textBrowser_2->append(tr("第%1帧 长度:%2 地址:%3 %4 类型:%5 未知")
                                                      .arg(data_count,0,10)
                                                      .arg(datasize,0,16)
                                                      .arg(data_addr[0], 0, 16)
                                                      .arg(data_addr[1], 0, 16)
                                                      .arg(data_type,0,16)
                                                      );
                            break;

                        }
                        ui->textBrowser_2->append(tr("数据:%1 校验:%2").arg(stringbuffer).arg(data_check,0,16));
                    }
                }
            }
        }
        else
        {
            isFileOpen=false;
            qDebug()<<"cannot open file";
        }
    }
}

void Form_stm32UpdataTool::on_pushButton_ID_clicked()//选择芯片
{
    //myserialcom->send_slot(const char *sendDataBuf,int len);
    //55 AA 04 00 01 ID CHECK
    unsigned char data_ID[7];
    int sum=0;
    data_ID[0]=0x55;
    data_ID[1]=0xaa;
    data_ID[2]=0x04;
    data_ID[3]=0x00;
    data_ID[4]=0x01;
    data_ID[5]=ui->spinBox_ID->value();
    for(int i=2;i<6;i++)
    {
        sum+=data_ID[i];
    }
    data_ID[6]=256-(sum&0xff);
    serialComTool->send_slot((const char *)&data_ID,7);
}

void Form_stm32UpdataTool::on_pushButton_remove_clicked()//解除保护
{
    //55 AA 03 01 00 FC
    unsigned char data_ID[6];
    data_ID[0]=0x55;
    data_ID[1]=0xaa;
    data_ID[2]=0x03;
    data_ID[3]=0x01;
    data_ID[4]=0x00;
    data_ID[5]=0xfc;
    serialComTool->send_slot((const char *)&data_ID,6);
}

void Form_stm32UpdataTool::on_pushButton_addr_clicked()//擦除首地址
{
    //55 AA 07 02 00 ADDR(32位地址高位在前) CHECK
    unsigned char data_ID[10];
    int sum=0;
    data_ID[0]=0x55;
    data_ID[1]=0xaa;
    data_ID[2]=0x07;
    data_ID[3]=0x02;
    data_ID[4]=0x00;
    data_ID[5]=(ui->lineEdit_addr->text().toInt(0,16)>>24)&0xff;
    data_ID[6]=(ui->lineEdit_addr->text().toInt(0,16)>>16)&0xff;
    data_ID[7]=(ui->lineEdit_addr->text().toInt(0,16)>>8)&0xff;
    data_ID[8]=(ui->lineEdit_addr->text().toInt(0,16))&0xff;
    for(int i=2;i<9;i++)
    {
        sum+=data_ID[i];
    }
    data_ID[9]=256-(sum&0xff);
    serialComTool->send_slot((const char *)&data_ID,10);
}

void Form_stm32UpdataTool::on_pushButton_page_size_clicked()//擦除页大小
{
    //55 AA 07 02 01 ADDR(32位页大小高位在前) CHECK 返回 02 01 FD
    unsigned char data_ID[10];
    int sum=0;
    data_ID[0]=0x55;
    data_ID[1]=0xaa;
    data_ID[2]=0x07;
    data_ID[3]=0x02;
    data_ID[4]=0x01;
    data_ID[5]=(ui->comboBox_page_size->currentText().toInt(0,16)>>24)&0xff;
    data_ID[6]=(ui->comboBox_page_size->currentText().toInt(0,16)>>16)&0xff;
    data_ID[7]=(ui->comboBox_page_size->currentText().toInt(0,16)>>8)&0xff;
    data_ID[8]=(ui->comboBox_page_size->currentText().toInt(0,16))&0xff;
    for(int i=2;i<9;i++)
    {
        sum+=data_ID[i];
    }
    data_ID[9]=256-(sum&0xff);
    serialComTool->send_slot((const char *)&data_ID,10);
}

void Form_stm32UpdataTool::on_pushButton_page_clicked()//擦除总页数
{
    //55 AA 07 02 02 ADDR(32位页总数高位在前) CHECK 返回 02 01 FD
    unsigned char data_ID[10];
    int sum=0;
    data_ID[0]=0x55;
    data_ID[1]=0xaa;
    data_ID[2]=0x07;
    data_ID[3]=0x02;
    data_ID[4]=0x02;
    data_ID[5]=(ui->lineEdit_page->text().toInt(0,16)>>24)&0xff;
    data_ID[6]=(ui->lineEdit_page->text().toInt(0,16)>>16)&0xff;
    data_ID[7]=(ui->lineEdit_page->text().toInt(0,16)>>8)&0xff;
    data_ID[8]=(ui->lineEdit_page->text().toInt(0,16))&0xff;
    for(int i=2;i<9;i++)
    {
        sum+=data_ID[i];
    }
    data_ID[9]=256-(sum&0xff);
    serialComTool->send_slot((const char *)&data_ID,10);
    removepagenum=ui->lineEdit_page->text().toInt(0,16);
}

void Form_stm32UpdataTool::on_pushButton_clean_clicked()//擦除flash
{
    //55 AA 03 02 03 F8 		02 01 FD 成功 02 00 FE失败
    unsigned char data_ID[6];
    data_ID[0]=0x55;
    data_ID[1]=0xaa;
    data_ID[2]=0x03;
    data_ID[3]=0x02;
    data_ID[4]=0x03;
    data_ID[5]=0xf8;
    serialComTool->send_slot((const char *)&data_ID,6);
}

void Form_stm32UpdataTool::on_pushButton_programaddr_clicked()//设置Flash编程起始地址
{
    //55 AA 07 03 00 ADDR(32位地址高位在前) CHECK 返回 02 01 FD
    unsigned char data_ID[10];
    int sum=0;
    data_ID[0]=0x55;
    data_ID[1]=0xaa;
    data_ID[2]=0x07;
    data_ID[3]=0x03;
    data_ID[4]=0x00;
    data_ID[5]=(ui->lineEdit_programaddr->text().toInt(0,16)>>24)&0xff;
    data_ID[6]=(ui->lineEdit_programaddr->text().toInt(0,16)>>16)&0xff;
    data_ID[7]=(ui->lineEdit_programaddr->text().toInt(0,16)>>8)&0xff;
    data_ID[8]=(ui->lineEdit_programaddr->text().toInt(0,16))&0xff;
    for(int i=2;i<9;i++)
    {
        sum+=data_ID[i];
    }
    data_ID[9]=256-(sum&0xff);
    serialComTool->send_slot((const char *)&data_ID,10);
}

void Form_stm32UpdataTool::on_pushButton_updata_clicked()//一键升级 连续写入
{
    if(start_flags&&mythread_com->stepRun>0)
    {
        mythread_com->set_data(0);
        start_flags=false;
    }
    else
    {
        mythread_com->set_ByteArraydata(inBlock);
        mythread_com->set_data(COM_Program);
        stopthread_flag=false;
        start_flags=true;
        mythread_com->start();
        ui->frame->setDisabled(true);
    }
}

void Form_stm32UpdataTool::on_pushButton_user_clicked()//返回用户程序
{
    //55 AA 03 05 00 F8 		返回 02 01 FD 成功 02 00 FE失败
    unsigned char data_ID[6];
    data_ID[0]=0x55;
    data_ID[1]=0xaa;
    data_ID[2]=0x03;
    data_ID[3]=0x05;
    data_ID[4]=0x00;
    data_ID[5]=0xf8;
    switch(QMessageBox::question(this,tr("提示"),
           tr("是否要返回用户程序！").arg(ui->spinBox_ID->value()),
           QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel))
    {
    case QMessageBox::Ok:
    {
        serialComTool->send_slot((const char *)&data_ID,6);
    }
    break;
    case QMessageBox::Cancel:
    {
    }
    break;
    default:
        break;
    }

}

void Form_stm32UpdataTool::on_pushButton_clicked()
{
    ui->textBrowser_2->clear();
}

void Form_stm32UpdataTool::on_pushButton_AutoUpdata_clicked()
{
    if(!com_status)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请打开串口!"));
        return;
    }
    if(gCom_QueryMode == 1)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请在升级模式下，升级!"));
        return;
    }
    if(!isFileOpen)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请打开文件!"));
        return;
    }


    if(start_flags&&mythread_com->stepRun>0)
    {
        mythread_com->set_data(0);
        start_flags=false;
    }
    else
    {
        if(cnt_ID == 0)
        {
            switch(QMessageBox::question(this,tr("提示"),
                   tr("请确认<%1号>是否为当前要升级的ID号！").arg(ui->spinBox_ID->value()),
                   QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok))
            {
            case QMessageBox::Ok:
            {
            }
            break;
            case QMessageBox::Cancel:
            {
                return ;
            }
            break;
            default:
                break;
            }
        }
        ui->textBrowser_2->clear();
        //ID
        programDataBuf[0]=ui->spinBox_ID->value();
        //擦除首地址
        programDataBuf[1]=(ui->lineEdit_addr->text().toInt(0,16)>>24)&0xff;
        programDataBuf[2]=(ui->lineEdit_addr->text().toInt(0,16)>>16)&0xff;
        programDataBuf[3]=(ui->lineEdit_addr->text().toInt(0,16)>>8)&0xff;
        programDataBuf[4]=(ui->lineEdit_addr->text().toInt(0,16))&0xff;
        //页数大小
        programDataBuf[5]=(ui->comboBox_page_size->currentText().toInt(0,16)>>24)&0xff;
        programDataBuf[6]=(ui->comboBox_page_size->currentText().toInt(0,16)>>16)&0xff;
        programDataBuf[7]=(ui->comboBox_page_size->currentText().toInt(0,16)>>8)&0xff;
        programDataBuf[8]=(ui->comboBox_page_size->currentText().toInt(0,16))&0xff;
        //擦除总页数
        programDataBuf[9]=(ui->lineEdit_page->text().toInt(0,16)>>24)&0xff;
        programDataBuf[10]=(ui->lineEdit_page->text().toInt(0,16)>>16)&0xff;
        programDataBuf[11]=(ui->lineEdit_page->text().toInt(0,16)>>8)&0xff;
        programDataBuf[12]=(ui->lineEdit_page->text().toInt(0,16))&0xff;
        //设置Flash编程起始地址
        programDataBuf[13]=(ui->lineEdit_programaddr->text().toInt(0,16)>>24)&0xff;
        programDataBuf[14]=(ui->lineEdit_programaddr->text().toInt(0,16)>>16)&0xff;
        programDataBuf[15]=(ui->lineEdit_programaddr->text().toInt(0,16)>>8)&0xff;
        programDataBuf[16]=(ui->lineEdit_programaddr->text().toInt(0,16))&0xff;

        mythread_com->set_ByteArraydata(inBlock);
        mythread_com->set_data(Auto_Program);
        stopthread_flag=false;
        start_flags=true;
        mythread_com->start();
        ui->frame->setDisabled(true);
        //ui->frame_2->setDisabled(true);
        //ui->pushButton_updata->setText(tr("8停止写入"));
    }

}

void Form_stm32UpdataTool::on_pushButton_IDUpdata_clicked()
{
    if(!com_status)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请打开串口!"));
        return;
    }
    if(!isFileOpen)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请打开文件!"));
        return;
    }
    if(ui->lineEdit_ID->text().isEmpty())
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("ID不能为空"));
        return;
    }

    QString str = ui->lineEdit_ID->text();
    QRegExp rx("(\\d+)");  // 匹配数字
    int pos = 0;
    char_count=0;

    while ((pos = rx.indexIn(str, pos)) != -1)
    {
        to_sent_data[char_count]=rx.cap(0).toInt();
//        qDebug()<<to_sent_data[char_count];
        ++char_count;
        pos += rx.matchedLength();
    }
//    qDebug()<<ui->lineEdit_ID->text()<<char_count;
    isALLAutoUpdata = true;
    cnt_ID = 0;
}

void Form_stm32UpdataTool::on_pushButton_mode_clicked()
{
    unsigned char data_ID[7];
    int sum=0;
    if(gCom_QueryMode == 0)
    {
        data_ID[0]=0x55;
        data_ID[1]=0xaa;
        data_ID[2]=0x04;
        data_ID[3]=0x00;
        data_ID[4]=0x02;
        data_ID[5]=1;
        for(int i=2;i<6;i++)
        {
            sum+=data_ID[i];
        }
        data_ID[6]=256-(sum&0xff);
        serialComTool->send_slot((const char *)&data_ID,7);
        //emit modeChanged();
    }
    gCom_QueryMode = 1;
    emit modeChanged();

}
