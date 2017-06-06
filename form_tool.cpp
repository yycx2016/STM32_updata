#include "form_tool.h"
#include "ui_form_tool.h"
#include "extern_data.h"
#include <QFileDialog>
#include <QByteArray>
#include <QSettings>

//
//
//int buffer_pos[1024];
//int current_pos=0;
//unsigned char sent_data[12];
//

//quint8 receivebuf[200];
//int n=0;
//int m=0;
//QString string_recv="";
//

//int fd_com,sty_num;
//unsigned int auto_sent_count;
//unsigned int auto_sent_time;
//QString saveFileName,openFileName;

int sendDataNum=11;
int startFlag=85;
int endFlag=187;

int selectStep = 0;
bool isFinish = false;

bool isAutoMated=false;

Form_tool::Form_tool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_tool)
{
    is16_=false;
    receive_byte_counter=0;
    receive_counter=0;
    sent_byte_counter=0;
    n = 0;
    ui->setupUi(this);   

    auto_sent_interval=new QTimer(this);//void auto_sent_interval_solt();
    connect(auto_sent_interval,SIGNAL(timeout()),this,SLOT(auto_sent_interval_solt()));
    updateList();

    connect(serialComTool->comDevice,SIGNAL(readyRead()),this,SLOT(readMyCom()));

    //qDebug()<<ui->lineEdit_startFlag->text();
    startFlag=ui->lineEdit_startFlag->text().section(' ',0,0).toInt();
    endFlag=ui->lineEdit_startFlag->text().section(' ',1,1).toInt();

}

Form_tool::~Form_tool()
{
    delete ui;
}

void Form_tool::on_pushButton_clearDisplay_clicked()//清除接收显示
{
    ui->comReceivePlainTextEdit->clear();
    receive_byte_counter = 0;
    sent_byte_counter = 0;
    ui->comSentLabel->setNum(sent_byte_counter);
    ui->comReceiveLabel->setNum(receive_byte_counter);
}

void Form_tool::on_checkBox_16_clicked(bool checked)
{
    is16_=checked;
}

void Form_tool::readMyCom()//串口接收函数
{
    //if(gCom_QueryMode==0)return;
    t_temp.restart();
    while(serialComTool->comDevice->bytesAvailable())
    {
        n = serialComTool->comDevice->bytesAvailable();

        if((n > 0 && t_temp.elapsed() > 300)||(n >= sendDataNum))
        {
            if(n >= sendDataNum) n= sendDataNum;
            serialComTool->comDevice->read((char *)&receivebuf , n);
            receive_byte_counter+=n;
            receive_counter+=n;
            string_recv.clear();
            for(int i = 0;i < n;i++)
            {
                if(is16_)
                {
                   receivebuf[1] = 0;
                   string_recv += (QString(QLatin1String((char*)receivebuf)));
                }
                else
                {
                    string_recv += (QString::number(receivebuf[i])+" ");
                }
            }
            if(receivebuf[0] == startFlag&&receivebuf[1] == endFlag)
            {
                if(receivebuf[3]==100) string_recv+=tr("编码值:%1").arg(receivebuf[4]*256+receivebuf[5]);
                if(receivebuf[3]==3&&receivebuf[4]==2) string_recv+=tr("AD值:%1").arg(receivebuf[5]*256+receivebuf[6]);
                if(receivebuf[3]==0x14) string_recv+=tr("对管值:%1").arg(receivebuf[5]*256+receivebuf[6]);
                if(receivebuf[3]==0x1f) string_recv+=tr("对管初值:%1").arg(receivebuf[5]*256+receivebuf[6]);
                if(receivebuf[3]==18||(receivebuf[3]==129&&receivebuf[4]==2))
                {
                    string_recv+=(tr("电机执行完毕"));
                }
                string_recv.prepend(QTime::currentTime().toString("[hh:mm:ss:zzz]:"));
                ui->comReceivePlainTextEdit->append(string_recv);
                if(isAutoMated)// 自动应答
                {
                    if(receivebuf[3] == 18||(receivebuf[3]==129&&receivebuf[4]==2))
                    {
                        //ui->comReceivePlainTextEdit->append(tr(" 自动应答"));
                        if(isFinish == true)
                        {
                            QString user_input;
                            if(receivebuf[3] == 18)
                            {
                                user_input=tr("%1 9 1 0 0 0 0 0 0").arg(receivebuf[8]);//电机应答 旧
                                send_solt_GZZ(user_input);// 串口发送 字符转换函数
                            }
                            else
                            {
                                user_input=tr("%1 128 255 0 0 0 0 0 0").arg(receivebuf[9]);//电机应答 新
                                send_solt_GZZ(user_input);// 串口发送 字符转换函数
                            }
                            send_GZZ();
                        }
                        else
                        {
                            on_pushButton_OldAsk_clicked();// 电机应答 旧
                        }
                    }
                }
            }
            else
            {
                string_recv.prepend(tr("误码:"));
                ui->comReceivePlainTextEdit->append(string_recv);
            }

            ui->comSentLabel->setNum (receive_byte_counter);

            t_temp.restart();
        }
    }

    ui->comReceivePlainTextEdit->moveCursor(QTextCursor::End);
    if(gCom_QueryMode == 3)//2
    {
        gCom_QueryMode = 0;
        emit modeChanged();
    }
}

int Form_tool::send_GZZ()
{
   bool iSsend = false;
   int  overtime = 0;
   Sleep(100);//延时以防底层收不到
   for(int i = selectStep;i < 15;i++)
   {
    switch(i)
    {
       case 1:
       {
        if(ui->checkBox_auto_1->isChecked())
         {
             send_solt_GZZ(ui->lineEdit_auto_1->text());
             iSsend = true;
         }
       }break;
    case 2:
    {
      if(ui->checkBox_auto_2->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_2->text());
          iSsend = true;
      }
    }break;
    case 3:
    {
      if(ui->checkBox_auto_3->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_3->text());
          iSsend = true;
      }
    }break;
    case 4:
    {
      if(ui->checkBox_auto_4->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_4->text());
          iSsend = true;
      }
    }break;
    case 5:
    {
      if(ui->checkBox_auto_5->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_5->text());
          iSsend = true;
      }
    }break;
    case 6:
    {
      if(ui->checkBox_auto_6->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_6->text());
          iSsend = true;

      }
    }break;
    case 7:
    {
      if(ui->checkBox_auto_7->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_7->text());
          iSsend = true;
      }
    }break;
    case 8:
    {
      if(ui->checkBox_auto_8->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_8->text());
          iSsend = true;
      }
    }break;
    case 9:
    {
      if(ui->checkBox_auto_9->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_9->text());
          iSsend = true;
      }
    }break;
    case 10:
    {
      if(ui->checkBox_auto_10->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_10->text());
          iSsend = true;
      }
    }break;
    case 11:
    {
      if(ui->checkBox_auto_11->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_11->text());
          iSsend = true;
      }
    }break;
    case 12:
    {
      if(ui->checkBox_auto_12->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_12->text());
          iSsend = true;
      }
    }break;
    case 13:
    {
      if(ui->checkBox_auto_12->isChecked())
      {
          send_solt_GZZ(ui->lineEdit_auto_12->text());
          iSsend = true;
      }
    }break;
    case 14:
    {
       i = 0;
    }
     break;
     default:
        break;
  }
     if((iSsend == true )||(overtime++ > 0x1f))
    {

        if(overtime >= 0x1f)
        {
            ui->comReceivePlainTextEdit->append(tr("条件轮发，回数据……"));
        }
        else
        {
           selectStep = i + 1;
        }
        break;
    }
   }

   return  iSsend;
}

void Form_tool::send_solt_GZZ(QString user_input)//串口发送 字符转换函数
{
    size_t char_count;
    //already_sent_times++;
    char_count = 0;
    QRegExp rx("(\\d+)");  // 匹配数字
    int pos = 0;
    char_count=0;
    user_input=ui->lineEdit_startFlag->text()+" "+user_input+" "+ui->lineEdit_endFlag->text();
    while ((pos = rx.indexIn(user_input, pos)) != -1)
    {
        to_sent_data[char_count]=rx.cap(0).toInt();
        //qDebug()<<to_sent_data[char_count];
        ++char_count;
        pos += rx.matchedLength();
    }
    serialComTool->comDevice->write((const char *)&to_sent_data, char_count);
    sent_byte_counter  +=  char_count;
    ui->comReceiveLabel->setNum (sent_byte_counter);
    ui->comReceivePlainTextEdit->append(tr("发送:")+user_input);
    t_temp.restart();
}

void Form_tool::on_pushButton_send_1_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_1->text());
}
void Form_tool::on_pushButton_send_2_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_2->text());
}
void Form_tool::on_pushButton_send_3_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_3->text());
}
void Form_tool::on_pushButton_send_4_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_4->text());
}
void Form_tool::on_pushButton_send_5_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_5->text());
}
void Form_tool::on_pushButton_send_6_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_6->text());
}
void Form_tool::on_pushButton_send_7_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_7->text());
}
void Form_tool::on_pushButton_send_8_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_8->text());
}
void Form_tool::on_pushButton_send_9_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_9->text());
}
void Form_tool::on_pushButton_send_10_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_10->text());
}
void Form_tool::on_pushButton_send_11_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_11->text());
}
void Form_tool::on_pushButton_send_12_clicked()
{
    send_solt_GZZ(ui->lineEdit_auto_12->text());
}

void Form_tool::on_pushButton_savefile_clicked()//接收文件保存
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "",
                               tr("TXT (*.txt)"));
    QFile fd(fileName);
       if (fd.open(QIODevice::WriteOnly| QIODevice::Text))
       {
           fd.seek(0);
           QByteArray text;
           text.clear();
           text=text.append(ui->comReceivePlainTextEdit->toPlainText());
           fd.write(text);
           fd.close();
       }
}

void Form_tool::on_pushButton_cycle_clicked()//开始定时轮发
{
    auto_sent_interval->start(ui->spinBox_delay_ms->value());
}
int isAutoSend = false;

void Form_tool::auto_sent_interval_solt()//定时器自动发送函数
{
    auto_sent_interval->stop();
    isAutoSend = false;
    if(ui->checkBox_auto_1->isChecked()){send_solt_GZZ(ui->lineEdit_auto_1->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_2->isChecked()){send_solt_GZZ(ui->lineEdit_auto_2->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_3->isChecked()){send_solt_GZZ(ui->lineEdit_auto_3->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_4->isChecked()){send_solt_GZZ(ui->lineEdit_auto_4->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_5->isChecked()){send_solt_GZZ(ui->lineEdit_auto_5->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_6->isChecked()){send_solt_GZZ(ui->lineEdit_auto_6->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_7->isChecked()){send_solt_GZZ(ui->lineEdit_auto_7->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_8->isChecked()){send_solt_GZZ(ui->lineEdit_auto_8->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_9->isChecked()){send_solt_GZZ(ui->lineEdit_auto_9->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_10->isChecked()){send_solt_GZZ(ui->lineEdit_auto_10->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_11->isChecked()){send_solt_GZZ(ui->lineEdit_auto_11->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(ui->checkBox_auto_12->isChecked()){send_solt_GZZ(ui->lineEdit_auto_12->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
    if(isAutoSend)auto_sent_interval->start(ui->spinBox_delay_ms->value());
}

void Form_tool::on_pushButton_saveList_clicked()
{
    QSettings settingsTube("./List.ini", QSettings::IniFormat);
    QString str;

    str = ui->lineEdit_auto_1->text();
    settingsTube.setValue("List/line_1", str);
    str = ui->lineEdit->text();
    settingsTube.setValue("List/explain_1", str);

    str = ui->lineEdit_auto_2->text();
    settingsTube.setValue("List/line_2", str);
    str = ui->lineEdit_2->text();
    settingsTube.setValue("List/explain_2", str);

    str = ui->lineEdit_auto_3->text();
    settingsTube.setValue("List/line_3", str);
    str = ui->lineEdit_3->text();
    settingsTube.setValue("List/explain_3", str);

    str = ui->lineEdit_auto_4->text();
    settingsTube.setValue("List/line_4", str);
    str = ui->lineEdit_4->text();
    settingsTube.setValue("List/explain_4", str);

    str = ui->lineEdit_auto_5->text();
    settingsTube.setValue("List/line_5", str);
    str = ui->lineEdit_5->text();
    settingsTube.setValue("List/explain_5", str);

    str = ui->lineEdit_auto_6->text();
    settingsTube.setValue("List/line_6", str);
    str = ui->lineEdit_6->text();
    settingsTube.setValue("List/explain_6", str);

    str = ui->lineEdit_auto_7->text();
    settingsTube.setValue("List/line_7", str);
    str = ui->lineEdit_7->text();
    settingsTube.setValue("List/explain_7", str);

    str = ui->lineEdit_auto_8->text();
    settingsTube.setValue("List/line_8", str);
    str = ui->lineEdit_8->text();
    settingsTube.setValue("List/explain_8", str);

    str = ui->lineEdit_auto_9->text();
    settingsTube.setValue("List/line_9", str);
    str = ui->lineEdit_9->text();
    settingsTube.setValue("List/explain_9", str);

    str = ui->lineEdit_auto_10->text();
    settingsTube.setValue("List/line_10", str);
    str = ui->lineEdit_10->text();
    settingsTube.setValue("List/explain_10", str);

    str = ui->lineEdit_auto_11->text();
    settingsTube.setValue("List/line_11", str);
    str = ui->lineEdit_11->text();
    settingsTube.setValue("List/explain_11", str);

    str = ui->lineEdit_auto_12->text();
    settingsTube.setValue("List/line_12", str);
    str = ui->lineEdit_12->text();
    settingsTube.setValue("List/explain_12", str);

    str = ui->lineEdit_startFlag->text();
    settingsTube.setValue("List/line_startFlag", str);
    str = ui->lineEdit_endFlag->text();
    settingsTube.setValue("List/explain_endFlag", str);

    settingsTube.setValue("List/explain_dataNum", ui->spinBox_dataNum->value());

}

void Form_tool::updateList()
{
    if (QFile::exists("./List.ini"))
    {
        QSettings settingsTube("./List.ini", QSettings::IniFormat);
        QString str;
        settingsTube.beginGroup("List");
        str = settingsTube.value("line_1").toString();
        ui->lineEdit_auto_1->setText(str);
        str = settingsTube.value("explain_1").toString();
        ui->lineEdit->setText(str);

        str = settingsTube.value("line_2").toString();
        ui->lineEdit_auto_2->setText(str);
        str = settingsTube.value("explain_2").toString();
        ui->lineEdit_2->setText(str);

        str = settingsTube.value("line_3").toString();
        ui->lineEdit_auto_3->setText(str);
        str = settingsTube.value("explain_3").toString();
        ui->lineEdit_3->setText(str);

        str = settingsTube.value("line_4").toString();
        ui->lineEdit_auto_4->setText(str);
        str = settingsTube.value("explain_4").toString();
        ui->lineEdit_4->setText(str);

        str = settingsTube.value("line_5").toString();
        ui->lineEdit_auto_5->setText(str);
        str = settingsTube.value("explain_5").toString();
        ui->lineEdit_5->setText(str);

        str = settingsTube.value("line_6").toString();
        ui->lineEdit_auto_6->setText(str);
        str = settingsTube.value("explain_6").toString();
        ui->lineEdit_6->setText(str);

        str = settingsTube.value("line_7").toString();
        ui->lineEdit_auto_7->setText(str);
        str = settingsTube.value("explain_7").toString();
        ui->lineEdit_7->setText(str);

        str = settingsTube.value("line_8").toString();
        ui->lineEdit_auto_8->setText(str);
        str = settingsTube.value("explain_8").toString();
        ui->lineEdit_8->setText(str);

        str = settingsTube.value("line_9").toString();
        ui->lineEdit_auto_9->setText(str);
        str = settingsTube.value("explain_9").toString();
        ui->lineEdit_9->setText(str);

        str = settingsTube.value("line_10").toString();
        ui->lineEdit_auto_10->setText(str);
        str = settingsTube.value("explain_10").toString();
        ui->lineEdit_10->setText(str);

        str = settingsTube.value("line_11").toString();
        ui->lineEdit_auto_11->setText(str);
        str = settingsTube.value("explain_11").toString();
        ui->lineEdit_11->setText(str);

        str = settingsTube.value("line_12").toString();
        ui->lineEdit_auto_12->setText(str);
        str = settingsTube.value("explain_12").toString();
        ui->lineEdit_12->setText(str);

        str = settingsTube.value("line_startFlag").toString();
        ui->lineEdit_startFlag->setText(str);
        str = settingsTube.value("explain_endFlag").toString();
        ui->lineEdit_endFlag->setText(str);

        ui->spinBox_dataNum->setValue(settingsTube.value("explain_dataNum").toInt());

        settingsTube.endGroup();

    }

}

void Form_tool::on_pushButton_mode_clicked()
{
    if(gCom_QueryMode == 1)
    {
        send_solt_GZZ(tr("255 2 0 0 0 0 0 0 0"));
        //emit modeChanged();
        gCom_QueryMode=3;//2
    }
    else
    {
        gCom_QueryMode=0;
    }

}

void Form_tool::on_lineEdit_startFlag_textChanged(const QString &arg1)
{
    startFlag=arg1.section(' ',0,0).toInt();
    endFlag=arg1.section(' ',1,1).toInt();
}

void Form_tool::on_spinBox_dataNum_valueChanged(int arg1)
{
    sendDataNum=arg1;
}

void Form_tool::on_pushButton_CX_clicked()// 查询电机
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 0 0 0 0 0 0 0 0").arg(ui->spinBox_MotorID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_OldAsk_clicked()// 电机应答 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 9 1 0 0 0 0 0 0").arg(ui->spinBox_MotorID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_NewAsk_clicked()// 电机应答 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 255 0 0 0 0 0 0").arg(ui->spinBox_MotorID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Min_old_clicked()// 电机初速度设置 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 1 1 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Min->value()>>24)&0xff)
            .arg((ui->spinBox_V_Min->value()>>16)&0xff)
            .arg((ui->spinBox_V_Min->value()>>8)&0xff)
            .arg((ui->spinBox_V_Min->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Min_new_clicked()// 电机初速度设置 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 1 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Min->value()>>24)&0xff)
            .arg((ui->spinBox_V_Min->value()>>16)&0xff)
            .arg((ui->spinBox_V_Min->value()>>8)&0xff)
            .arg((ui->spinBox_V_Min->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Max_old_clicked()// 电机最大速度设置 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 1 2 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Max->value()>>24)&0xff)
            .arg((ui->spinBox_V_Max->value()>>16)&0xff)
            .arg((ui->spinBox_V_Max->value()>>8)&0xff)
            .arg((ui->spinBox_V_Max->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Max_new_clicked()// 电机最大速度设置 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 2 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Max->value()>>24)&0xff)
            .arg((ui->spinBox_V_Max->value()>>16)&0xff)
            .arg((ui->spinBox_V_Max->value()>>8)&0xff)
            .arg((ui->spinBox_V_Max->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Accr_old_clicked()// 电机加速度设置 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 1 3 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Accr->value()>>24)&0xff)
            .arg((ui->spinBox_V_Accr->value()>>16)&0xff)
            .arg((ui->spinBox_V_Accr->value()>>8)&0xff)
            .arg((ui->spinBox_V_Accr->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Accr_new_clicked()// 电机加速度设置 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 5 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Accr->value()>>24)&0xff)
            .arg((ui->spinBox_V_Accr->value()>>16)&0xff)
            .arg((ui->spinBox_V_Accr->value()>>8)&0xff)
            .arg((ui->spinBox_V_Accr->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Even_old_clicked()// 电机匀速速度设置 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 4 1 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Even->value()>>24)&0xff)
            .arg((ui->spinBox_V_Even->value()>>16)&0xff)
            .arg((ui->spinBox_V_Even->value()>>8)&0xff)
            .arg((ui->spinBox_V_Even->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Even_new_clicked()// 电机匀速速度设置 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 3 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Even->value()>>24)&0xff)
            .arg((ui->spinBox_V_Even->value()>>16)&0xff)
            .arg((ui->spinBox_V_Even->value()>>8)&0xff)
            .arg((ui->spinBox_V_Even->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Reset_old_clicked()// 复位速度设置 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 4 3 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Reset->value()>>24)&0xff)
            .arg((ui->spinBox_V_Reset->value()>>16)&0xff)
            .arg((ui->spinBox_V_Reset->value()>>8)&0xff)
            .arg((ui->spinBox_V_Reset->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_V_Reset_new_clicked()// 复位速度设置 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 4 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_V_Reset->value()>>24)&0xff)
            .arg((ui->spinBox_V_Reset->value()>>16)&0xff)
            .arg((ui->spinBox_V_Reset->value()>>8)&0xff)
            .arg((ui->spinBox_V_Reset->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_setpSet_clicked()// 电机步数设置 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 1 4 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg((ui->spinBox_setpSet->value()>>24)&0xff)
            .arg((ui->spinBox_setpSet->value()>>16)&0xff)
            .arg((ui->spinBox_setpSet->value()>>8)&0xff)
            .arg((ui->spinBox_setpSet->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_DirSet_clicked()// 电机方向设置 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 1 5 %2 0 0 0 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg(ui->comboBox_DirSet->currentIndex())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_Enanble_clicked()// 电机使能设置 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 1 6 %2 0 0 0 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg(ui->comboBox_Enanble->currentIndex())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_Run_Accr_clicked()// 电机加减速运行 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 1 7 0 0 0 0 0 0")
            .arg(ui->spinBox_MotorID->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_Run_Even_clicked()// 电机匀速运行 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 4 2 0 0 0 0 0 0")
            .arg(ui->spinBox_MotorID->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_Run_Reset_clicked()// 电机复位 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 4 4 %2 0 0 0 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg(ui->comboBox_DirSet->currentIndex())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_Run_Accr_new_clicked()// 电机加减速运行 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 32 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg(ui->comboBox_DirSet_new->currentIndex())
            .arg((ui->spinBox_setpSet_new->value()>>16)&0xff)
            .arg((ui->spinBox_setpSet_new->value()>>8)&0xff)
            .arg((ui->spinBox_setpSet_new->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_Run_Even_new_clicked()// 电机匀速运行 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 33 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg(ui->comboBox_DirSet_new->currentIndex())
            .arg((ui->spinBox_setpSet_new->value()>>16)&0xff)
            .arg((ui->spinBox_setpSet_new->value()>>8)&0xff)
            .arg((ui->spinBox_setpSet_new->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_Run_Reset_new_clicked()// 电机复位 新
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 34 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_MotorID->value())
            .arg(ui->comboBox_DirSet_new->currentIndex())
            .arg((ui->spinBox_setpSet_new->value()>>16)&0xff)
            .arg((ui->spinBox_setpSet_new->value()>>8)&0xff)
            .arg((ui->spinBox_setpSet_new->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_ReadDG_clicked()// 读吸嘴检测对管
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 3 1 0 0 0 0 0 0")
            .arg(ui->spinBox_MotorID->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_InitDG_clicked()// 吸嘴对管初始化 初值采集
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 3 5 0 0 0 0 0 0")
            .arg(ui->spinBox_MotorID->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_ReadOP_clicked()// 读取复位光开状态
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 3 2 0 0 0 0 0 0")
            .arg(ui->spinBox_MotorID->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_CX_clicked()// 液面检测查询 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 0 0 0 0 0 0 0 0").arg(ui->spinBox_YM_ID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_Ask_clicked()// 液面检测应答 旧
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 9 1 0 0 0 0 0 0").arg(ui->spinBox_YM_ID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YMJC_clicked()// 开始液面检测
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 3 1 %2 0 %3 0 0 0")
            .arg(ui->spinBox_YM_ID->value())
            .arg(ui->comboBox_YMJC_DCF->currentIndex())
            .arg(ui->spinBox_YMJC_CH->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_AD_clicked()// 读取通道AD 值
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 3 2 0 0 %2 0 0 0")
            .arg(ui->spinBox_YM_ID->value())
            //.arg(ui->comboBox_Set_DCF->currentIndex())
            .arg(ui->spinBox_Set_CH->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_DCF_clicked()// 开关电磁阀
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 3 3 %2 0 %3 0 0 0")
            .arg(ui->spinBox_YM_ID->value())
            .arg(ui->comboBox_Set_DCF->currentIndex())
            .arg(ui->spinBox_Set_CH->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_ERROR_clicked()// 判断异常
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 3 7 %2 0 %3 0 0 0")
            .arg(ui->spinBox_YM_ID->value())
            .arg(ui->comboBox_Set_DCF->currentIndex())
            .arg(ui->spinBox_Set_CH->value())
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_SET_clicked()// 设置液面检测 阈值，Z轴电机行程，泵电机行程
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8

    user_input=tr("%1 3 %2 %3 %4 0 0 0 0")
            .arg(ui->spinBox_YM_ID->value())
            .arg(ui->comboBox_YM_SET->currentIndex()+4)
            .arg(ui->spinBox_YM_Value->value()/256)
            .arg(ui->spinBox_YM_Value->value()&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_STOP_clicked()// 液面检测电机停止
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 4 8 0 0 0 0 0 0").arg(ui->spinBox_YM_MotorID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_Return_clicked()// 液面检测电机 快速归位
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 4 9 0 0 0 0 0 0").arg(ui->spinBox_YM_MotorID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_MotorRun_clicked()// 液面检测电机 开始匀速运行
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 4 10 %2 %3 %4 0 0 0")
            .arg(ui->spinBox_YM_MotorID->value())
            .arg((ui->spinBox_YM_MotorStep->value()>>16)&0xff)
            .arg((ui->spinBox_YM_MotorStep->value()>>8)&0xff)
            .arg((ui->spinBox_YM_MotorStep->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_pushButton_YM_MotorAsk_clicked()
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 9 1 0 0 0 0 0 0").arg(ui->spinBox_YM_MotorID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

void Form_tool::on_checkBox_automated_clicked(bool checked)
{
    isAutoMated=checked;
}

void Form_tool::on_pushButton_condition_clicked(bool checked)
{
    isFinish = checked;
    selectStep = 0;
    if(isFinish == true)
    {
      if (send_GZZ() == false)
      {
          QMessageBox::about(this,tr("提示"),
                       tr("请在右侧的的小框内勾选要发送的指令……"));
      }
    }
}
