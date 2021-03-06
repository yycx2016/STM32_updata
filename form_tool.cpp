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
    LineEdits_auto << ui->lineEdit_auto_1<< ui->lineEdit_auto_2
                  << ui->lineEdit_auto_3<< ui->lineEdit_auto_4
                  << ui->lineEdit_auto_5<< ui->lineEdit_auto_6
                  << ui->lineEdit_auto_7<< ui->lineEdit_auto_8
                  << ui->lineEdit_auto_9<< ui->lineEdit_auto_10
                  << ui->lineEdit_auto_11<< ui->lineEdit_auto_12;
    LineEdits << ui->lineEdit<< ui->lineEdit_2<< ui->lineEdit_3
              << ui->lineEdit_4<< ui->lineEdit_5<< ui->lineEdit_6
              << ui->lineEdit_7<< ui->lineEdit_8<< ui->lineEdit_9
              << ui->lineEdit_10<< ui->lineEdit_11<< ui->lineEdit_12;

    auto_sent_interval=new QTimer(this);//void auto_sent_interval_solt();
    connect(auto_sent_interval,SIGNAL(timeout()),this,SLOT(auto_sent_interval_solt()));

    updateList(0);

    connect(serialComTool->comDevice,SIGNAL(readyRead()),this,SLOT(readMyCom()));

    //qDebug()<<ui->lineEdit_startFlag->text();
    startFlag = ui->lineEdit_startFlag->text().section(' ',0,0).toInt();
    endFlag = ui->lineEdit_startFlag->text().section(' ',1,1).toInt();

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
    is16_= checked;
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
                if(receivebuf[4]==8&&receivebuf[5] == 2) string_recv+=tr("编码器AB正交值:%1").arg((receivebuf[6]<<16)+(receivebuf[7]<<8)+receivebuf[8]);
                if(receivebuf[4]==8&&receivebuf[5] == 3) string_recv+=tr("编码器电机脉冲值:%1").arg((receivebuf[6]<<16)+(receivebuf[7]<<8)+receivebuf[8]);
                if(receivebuf[3]==100) string_recv+=tr("编码值:%1").arg(receivebuf[4]*256+receivebuf[5]);
                if(receivebuf[3]==3&&receivebuf[4]==2) string_recv+=tr("AD值:%1").arg(receivebuf[5]*256+receivebuf[6]);
                if(receivebuf[3]==0x14) string_recv+=tr("对管值:%1").arg(receivebuf[5]*256+receivebuf[6]);
                if(receivebuf[3]==0x1f) string_recv+=tr("对管初值:%1").arg(receivebuf[5]*256+receivebuf[6]);
                if(receivebuf[3]==0xfe) string_recv+=tr("%1年%2月%3日").arg(receivebuf[6]).arg(receivebuf[7]).arg(receivebuf[8]);
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
                        if(isFinish == true)
                        {
                            send_GZZ();
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
   int i = 0 ,cnt = 0;
   Sleep(100);//延时以防底层收不到
   for(i = selectStep; i < 13; i++)
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
              i = 0;
          }
        }break;
         default:
            break;
      }
       if(iSsend == true )
       {
           selectStep = i + 1;
           break;//跳出for循环
       }
       else if( i >= 12)
       {
           if(cnt++ > 2)
           {
               selectStep = 0;
               break;//跳出for循环
           }
           else
           {
               i = 0;
           }
       }
   }//for
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
    if ((QFileInfo(fileName).suffix().isEmpty())) //若后缀为空自动添加.txt后缀
    {
        fileName.append(".txt");
    }
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
    selectStep = 0;
    if (send_GZZ() == false)
    {
      QMessageBox::about(this,tr("提示"),
                   tr("请在右侧的的小框内勾选要发送的指令……"));
    }
    else
    {
      auto_sent_interval->start(ui->spinBox_delay_ms->value());
      ui->pushButton_condition->setDisabled(true);
    }

}

//void Form_tool::auto_sent_interval_solt1()//定时器自动发送函数
//{
//    auto_sent_interval->stop();
//    isAutoSend = false;
//    if(ui->checkBox_auto_1->isChecked()){send_solt_GZZ(ui->lineEdit_auto_1->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_2->isChecked()){send_solt_GZZ(ui->lineEdit_auto_2->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_3->isChecked()){send_solt_GZZ(ui->lineEdit_auto_3->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_4->isChecked()){send_solt_GZZ(ui->lineEdit_auto_4->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_5->isChecked()){send_solt_GZZ(ui->lineEdit_auto_5->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_6->isChecked()){send_solt_GZZ(ui->lineEdit_auto_6->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_7->isChecked()){send_solt_GZZ(ui->lineEdit_auto_7->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_8->isChecked()){send_solt_GZZ(ui->lineEdit_auto_8->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_9->isChecked()){send_solt_GZZ(ui->lineEdit_auto_9->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_10->isChecked()){send_solt_GZZ(ui->lineEdit_auto_10->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_11->isChecked()){send_solt_GZZ(ui->lineEdit_auto_11->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(ui->checkBox_auto_12->isChecked()){send_solt_GZZ(ui->lineEdit_auto_12->text());isAutoSend=true;Sleep(100);}//延时以防底层收不到
//    if(isAutoSend)auto_sent_interval->start(ui->spinBox_delay_ms->value());
//}

void Form_tool::auto_sent_interval_solt()//定时器自动发送函数
{
    auto_sent_interval->stop();
    if(send_GZZ() == true)
    {
        auto_sent_interval->start(ui->spinBox_delay_ms->value());
    }
     else
    {
        ui->pushButton_condition->setDisabled(false);
    }
}

void Form_tool::on_pushButton_saveList_clicked()
{
//    QSettings settingsTube("./List.ini", QSettings::IniFormat);
//    QString str;

//    str = ui->lineEdit_auto_1->text();
//    settingsTube.setValue("List/line_1", str);
//    str = ui->lineEdit->text();
//    settingsTube.setValue("List/explain_1", str);

//    str = ui->lineEdit_auto_2->text();
//    settingsTube.setValue("List/line_2", str);
//    str = ui->lineEdit_2->text();
//    settingsTube.setValue("List/explain_2", str);

//    str = ui->lineEdit_auto_3->text();
//    settingsTube.setValue("List/line_3", str);
//    str = ui->lineEdit_3->text();
//    settingsTube.setValue("List/explain_3", str);

//    str = ui->lineEdit_auto_4->text();
//    settingsTube.setValue("List/line_4", str);
//    str = ui->lineEdit_4->text();
//    settingsTube.setValue("List/explain_4", str);

//    str = ui->lineEdit_auto_5->text();
//    settingsTube.setValue("List/line_5", str);
//    str = ui->lineEdit_5->text();
//    settingsTube.setValue("List/explain_5", str);

//    str = ui->lineEdit_auto_6->text();
//    settingsTube.setValue("List/line_6", str);
//    str = ui->lineEdit_6->text();
//    settingsTube.setValue("List/explain_6", str);

//    str = ui->lineEdit_auto_7->text();
//    settingsTube.setValue("List/line_7", str);
//    str = ui->lineEdit_7->text();
//    settingsTube.setValue("List/explain_7", str);

//    str = ui->lineEdit_auto_8->text();
//    settingsTube.setValue("List/line_8", str);
//    str = ui->lineEdit_8->text();
//    settingsTube.setValue("List/explain_8", str);

//    str = ui->lineEdit_auto_9->text();
//    settingsTube.setValue("List/line_9", str);
//    str = ui->lineEdit_9->text();
//    settingsTube.setValue("List/explain_9", str);

//    str = ui->lineEdit_auto_10->text();
//    settingsTube.setValue("List/line_10", str);
//    str = ui->lineEdit_10->text();
//    settingsTube.setValue("List/explain_10", str);

//    str = ui->lineEdit_auto_11->text();
//    settingsTube.setValue("List/line_11", str);
//    str = ui->lineEdit_11->text();
//    settingsTube.setValue("List/explain_11", str);

//    str = ui->lineEdit_auto_12->text();
//    settingsTube.setValue("List/line_12", str);
//    str = ui->lineEdit_12->text();
//    settingsTube.setValue("List/explain_12", str);

//    str = ui->lineEdit_startFlag->text();
//    settingsTube.setValue("List/line_startFlag", str);
//    str = ui->lineEdit_endFlag->text();
//    settingsTube.setValue("List/explain_endFlag", str);
//    settingsTube.setValue("List/explain_dataNum", ui->spinBox_dataNum->value());

    QString str;
    QList<QLineEdit*>::size_type i;
    for( i= 0;(i != LineEdits_auto.size())||( i!= LineEdits.size());++i)
     {
        if(i < LineEdits_auto.size())
        {
            str += "指令" + QString::number(i+1) +':';
            str += LineEdits_auto.at(i)->text();
            str += '#';
        }
        if(i < LineEdits.size())
        {
            str += LineEdits.at(i)->text();
            str += "#\n";
        }
//      qDebug()<<LineEdits_auto.at(i) <<LineEdits.at(i);
     }
     str += "标志" +  QString::number((++i)+1) +'=';
     str += ui->lineEdit_startFlag->text();
     str += "#";
     str += "起始标志";
     str += "#\n";
     str += "标志" +  QString::number((++i)+1)+'=';
     str += ui->lineEdit_endFlag->text();
     str += "#";
     str += "结束标志";
     str += "#\n";
     str += "标志" +  QString::number((++i)+1) +'=';
     str += QString::number(ui->spinBox_dataNum->value());
     str += "#";
     str += "位数";
     str += "#\n";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "init",
                               tr("TXT (*.txt)"));

    if (QFileInfo(fileName).suffix().isEmpty()) //若后缀为空自动添加.txt后缀
    {
        fileName.append(".txt");
    }

   QFile fd(fileName);
   if (fd.open(QIODevice::WriteOnly| QIODevice::Text))
   {
       QSettings settings("./filepath.ini", QSettings::IniFormat);
       settings.setValue("FilePath/camandcurrentpath", fileName);

       fd.seek(0);
       QByteArray text;
       text.clear();
       text=text.append(str);
       fd.write(text);
       fd.close();
   }
}

void Form_tool::updateList(bool flag)
{
//    if (QFile::exists("./List.ini"))
//    {
//        QSettings settingsTube("./List.ini", QSettings::IniFormat);
//        QString str;
//        settingsTube.beginGroup("List");
//        str = settingsTube.value("line_1").toString();
//        ui->lineEdit_auto_1->setText(str);
//        str = settingsTube.value("explain_1").toString();
//        ui->lineEdit->setText(str);

//        str = settingsTube.value("line_2").toString();
//        ui->lineEdit_auto_2->setText(str);
//        str = settingsTube.value("explain_2").toString();
//        ui->lineEdit_2->setText(str);

//        str = settingsTube.value("line_3").toString();
//        ui->lineEdit_auto_3->setText(str);
//        str = settingsTube.value("explain_3").toString();
//        ui->lineEdit_3->setText(str);

//        str = settingsTube.value("line_4").toString();
//        ui->lineEdit_auto_4->setText(str);
//        str = settingsTube.value("explain_4").toString();
//        ui->lineEdit_4->setText(str);

//        str = settingsTube.value("line_5").toString();
//        ui->lineEdit_auto_5->setText(str);
//        str = settingsTube.value("explain_5").toString();
//        ui->lineEdit_5->setText(str);

//        str = settingsTube.value("line_6").toString();
//        ui->lineEdit_auto_6->setText(str);
//        str = settingsTube.value("explain_6").toString();
//        ui->lineEdit_6->setText(str);

//        str = settingsTube.value("line_7").toString();
//        ui->lineEdit_auto_7->setText(str);
//        str = settingsTube.value("explain_7").toString();
//        ui->lineEdit_7->setText(str);

//        str = settingsTube.value("line_8").toString();
//        ui->lineEdit_auto_8->setText(str);
//        str = settingsTube.value("explain_8").toString();
//        ui->lineEdit_8->setText(str);

//        str = settingsTube.value("line_9").toString();
//        ui->lineEdit_auto_9->setText(str);
//        str = settingsTube.value("explain_9").toString();
//        ui->lineEdit_9->setText(str);

//        str = settingsTube.value("line_10").toString();
//        ui->lineEdit_auto_10->setText(str);
//        str = settingsTube.value("explain_10").toString();
//        ui->lineEdit_10->setText(str);

//        str = settingsTube.value("line_11").toString();
//        ui->lineEdit_auto_11->setText(str);
//        str = settingsTube.value("explain_11").toString();
//        ui->lineEdit_11->setText(str);

//        str = settingsTube.value("line_12").toString();
//        ui->lineEdit_auto_12->setText(str);
//        str = settingsTube.value("explain_12").toString();
//        ui->lineEdit_12->setText(str);

//        str = settingsTube.value("line_startFlag").toString();
//        ui->lineEdit_startFlag->setText(str);
//        str = settingsTube.value("explain_endFlag").toString();
//        ui->lineEdit_endFlag->setText(str);

//        ui->spinBox_dataNum->setValue(settingsTube.value("explain_dataNum").toInt());

//        settingsTube.endGroup();

//    }

    QDir currentFileDir;
    QString fileName;
    if (!QFile::exists("./filepath.ini"))//文件路径
    {
        QSettings settings("./filepath.ini", QSettings::IniFormat);
        settings.setValue("FilePath/camandcurrentpath", "./init.txt");
        return ;
    }
    QSettings settings("./filepath.ini", QSettings::IniFormat);
    currentFileDir.setPath(settings.value("FilePath/camandcurrentpath").toString());
    if(flag == 1)
    {
      fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                              currentFileDir.path(),
                                              "TXT file(*.txt);;Any files (*)");
    }
    else
    {
       fileName = currentFileDir.path();
    }
//    qDebug()<<fileName;
    int i = 0;
    QString stringbuffer;
    QString buff;
    if(!fileName.isEmpty() )
    {
        QFile file_in(fileName);
        if (file_in.open(QIODevice::ReadOnly))
        {
            QSettings settings("./filepath.ini", QSettings::IniFormat);
            settings.setValue("FilePath/camandcurrentpath", fileName);
            stringbuffer = file_in.readAll();
                for(int j = 0;j < stringbuffer.size() ;j++)
                {
//                    qDebug() << stringbuffer.at(j);
                    if(stringbuffer.at(j) == ':')
                    {
                        if(i < LineEdits_auto.size())
                        {
                            buff.clear();
                            for(j = j+1;stringbuffer.at(j) != '#';j++)
                            {
                               buff +=stringbuffer.at(j);
                            }
                            LineEdits_auto.at(i)->setText(buff);
//                            qDebug()<<buff;
                        }
                        if(i < LineEdits.size())
                        {
                            buff.clear();
                            for(j = j+1;stringbuffer.at(j) != '#';j++)
                            {
                               buff +=stringbuffer.at(j);
                            }
                            LineEdits.at(i)->setText(buff);
//                            qDebug()<<buff;
                        }
                        i++;
                    }
                    else if(stringbuffer.at(j) == '=')
                    {
                        QString temp;
                        for(j = j+1;stringbuffer.at(j) != '#';j++)
                        {
                           temp +=stringbuffer.at(j);
                        }
                        buff.clear();
                        for(j = j+1;stringbuffer.at(j) != '#';j++)
                        {
                            buff +=stringbuffer.at(j);
                        }
                        if(buff == "起始标志")
                        {
                            ui->lineEdit_startFlag->setText(temp);
                        }
                        else if(buff == "结束标志")
                        {
                            ui->lineEdit_endFlag->setText(temp);
                        }else if(buff == "位数")
                        {
                            ui->spinBox_dataNum->setValue(temp.toInt(0,10));
                        }
//                        qDebug()<<temp;
                    }
                }
        }
    }

}

void Form_tool::on_pushButton_mode_clicked()
{
    if(gCom_QueryMode == 1)
    {
        send_solt_GZZ(tr("255 2 0 0 0 0 0 0 0"));
        //emit modeChanged();
        gCom_QueryMode=3;
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
    isAutoMated = checked;
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
      else
      {
          isAutoMated = true;
          ui->checkBox_automated->setChecked(true);
          ui->pushButton_cycle->setDisabled(true);
      }
    }
    else
    {
        ui->checkBox_automated->setChecked(false);
        ui->pushButton_cycle->setDisabled(false);
    }
}

void Form_tool::on_pushButton_open_clicked()
{
    updateList(1);
}
//编码器AB正交值查询
void Form_tool::on_pushButton_encoder_CX_clicked()
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 51 0 0 0 0 0 0").arg(ui->spinBox_encoderID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}

//设置编码器分度值
void Form_tool::on_pushButton_encoder_Scale_clicked()
{
    QString user_input;
    user_input=tr("%1 128 50 %2 %3 0 0 0 0")
            .arg(ui->spinBox_encoderID->value())
            .arg((ui->spinBox_encoderID_2->value()>>8)&0xff)
            .arg((ui->spinBox_encoderID_2->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}
//设置最小误差脉冲
void Form_tool::on_pushButton_encoder_Error_clicked()
{
    QString user_input;
    user_input=tr("%1 128 52 %2 %3 %4 %5 0 0")
            .arg(ui->spinBox_encoderID->value())
            .arg((ui->spinBox_encoderID_3->value()>>24)&0xff)
            .arg((ui->spinBox_encoderID_3->value()>>16)&0xff)
            .arg((ui->spinBox_encoderID_3->value()>>8)&0xff)
            .arg((ui->spinBox_encoderID_3->value())&0xff)
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}
//设置编码器方向
void Form_tool::on_pushButton_EncoderDirSet_clicked()
{
    QString user_input;
    user_input=tr("%1 128 48 %2 0 0 0 0 0")
            .arg(ui->spinBox_encoderID->value())
            .arg((ui->comboBox_EncoderDirSet->currentIndex()))
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数

}
//设置编码器使能
void Form_tool::on_pushButton_EncoderEnanble_clicked()
{
    QString user_input;
    user_input=tr("%1 128 49 %2 0 0 0 0 0")
            .arg(ui->spinBox_encoderID->value())
            .arg((ui->comboBox_EncoderEnanble->currentIndex()))
            ;
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}
//编码器电机脉冲值查询
void Form_tool::on_pushButton_encoder_CX_2_clicked()
{
    QString user_input;
    //ID 1 2 3 4 5 6 7 8
    user_input=tr("%1 128 54 0 0 0 0 0 0").arg(ui->spinBox_encoderID->value());
    send_solt_GZZ(user_input);// 串口发送 字符转换函数
}
