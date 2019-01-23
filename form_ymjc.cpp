#include "form_ymjc.h"
#include "ui_form_ymjc.h"
#include "extern_data.h"

quint16 ADValue[4]={500,1000,1500,2000};
quint16 ADValue_max[4]={500,1000,1500,2000};
quint16 ADValue_min[4]={500,1000,1500,2000};
unsigned char to_receivebuf[200];

Form_ymjc::Form_ymjc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_ymjc)
{
    n = 0;
    ui->setupUi(this);
    Tabel[0] = ui->label_CH1;
    Tabel[1] = ui->label_CH1_MAX;
    Tabel[2] = ui->label_CH1_MIN;
    Tabel[3] = ui->label_CH2;
    Tabel[4] = ui->label_CH2_MAX;
    Tabel[5] = ui->label_CH2_MIN;
    Tabel[6] = ui->label_CH3;
    Tabel[7] = ui->label_CH3_MAX;
    Tabel[8] = ui->label_CH3_MIN;
    Tabel[9] = ui->label_CH4;
    Tabel[10] = ui->label_CH4_MAX;
    Tabel[11] = ui->label_CH4_MIN;

    ADValueCurve = new DrawADValueCurve();
    ui->frame_3->layout()->addWidget(ADValueCurve);
    connect(this, SIGNAL(sigADChanged()), ADValueCurve, SLOT(reDrawADCurve()));

    connect(serialComTool->comDevice,SIGNAL(readyRead()),this,SLOT(readMyCom()));

    timer_getValue = new QTimer();
    connect(timer_getValue,SIGNAL(timeout()),this,SLOT(timer_getValue_slot()));

}

Form_ymjc::~Form_ymjc()
{
    delete ui;

}

void Form_ymjc::readMyCom()
{
    overtime.restart();
    for(;serialComTool->comDevice->bytesAvailable();)
    {
        n = serialComTool->comDevice->bytesAvailable();
        if(((n > 0)&&(overtime.elapsed() > 500))||(n >= 11))//当n小于11，通过Overtime超时来让串口读取数据,退出循环
        {
            serialComTool->comDevice->read((char *)&to_receivebuf , n);
            for(int i = 0; i <= n; i++)
            {
                if((to_receivebuf[i+8] == ui->spinBox_addr->value())&&(n >= (i+11)))
                {
                    if((to_receivebuf[i+3] == 0x03)&&(to_receivebuf[i+4] == 0x0d)&&(to_receivebuf[i+5] > 0))
                    {
                        int cnt = to_receivebuf[i+5]-1;
                        ADValue[cnt]= to_receivebuf[i+6]*256+to_receivebuf[i+7];
                        if(ADValue[cnt]>ADValue_max[cnt])ADValue_max[cnt]=ADValue[cnt];
                        if(ADValue[cnt]<ADValue_min[cnt])ADValue_min[cnt]=ADValue[cnt];
//                          qDebug()<<ADValue[to_receivebuf[i+4]]<<to_receivebuf[i+4]<<"串口接收"<<receive_flag;
//                         qDebug()<<to_receivebuf[i+2]<<to_receivebuf[i+3]<<to_receivebuf[i+4]<<to_receivebuf[i+7];
                        Tabel[cnt*3]->setText(QString::number(ADValue[cnt]));
                        Tabel[cnt*3+1]->setText(QString::number(ADValue_max[cnt]));
                        Tabel[cnt*3+2]->setText(QString::number(ADValue_min[cnt]));
//                          qDebug()<<cnt<<Tabel[cnt][0]->setText(QString::number(ADValue[cnt]))<<Tabel[cnt][1]->setText(QString::number(ADValue_max[cnt]))<<Tabel[cnt][2]->setText(QString::number(ADValue_min[cnt]));
                    break;
                    }
                }

            }
        }
    }
}

void Form_ymjc::on_pushButton_start_clicked()
{
    if(!com_status)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请打开串口!"));
        return;
    }
    unsigned char to_sent_data[12];
    to_sent_data[0]=0x55;
    to_sent_data[1]=0xbb;

    to_sent_data[2]=ui->spinBox_addr->value();
    to_sent_data[3] = 3;
    to_sent_data[4] = 13;
    to_sent_data[5] = 0;
    to_sent_data[6] = 0;
    to_sent_data[7] = 0;
    to_sent_data[8] = 0;
    to_sent_data[9] = 0;
    to_sent_data[10] = 0;
    to_sent_data[11] = 0xff;
    serialComTool->comDevice->write((const char *)&to_sent_data, 12);
    currentTimeEnd=0;
    timer_getValue->start(ui->spinBox_time->value());

    qDebug()<<"send start";
}

void Form_ymjc::on_pushButton_close_clicked()
{
    if(com_status == false)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请打开串口!"));
        return;
    }
    unsigned char to_sent_data[12];
    to_sent_data[0]=0x55;
    to_sent_data[1]=0xbb;

    to_sent_data[2] = ui->spinBox_addr->value();
    to_sent_data[3] = 3;
    to_sent_data[4] = 13;
    to_sent_data[5] = 1;
    to_sent_data[6] = 0;
    to_sent_data[7] = 0;
    to_sent_data[8] = 0;
    to_sent_data[9] = 0;
    to_sent_data[10] = 0;
    to_sent_data[11] = 0xff;
    serialComTool->comDevice->write((const char *)&to_sent_data, 12);
    timer_getValue->stop();
    currentTimeEnd = 0;
    qDebug()<<"send stop";
}

void Form_ymjc::on_pushButton_clicked()
{

    if(!com_status)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请打开串口!"));
    }
    else if( currentTimeEnd == 0)
    {
        QMessageBox::warning(0, tr("错误提示:"),tr("请先点击<开始>！"));
    }
    else
    {
        currentTimeEnd = 0;
        ADValue_max[0]= ADValue[0];
        ADValue_min[0]= ADValue[0];
        ADValue_max[1]= ADValue[1];
        ADValue_min[1]= ADValue[1];
        ADValue_max[2]= ADValue[2];
        ADValue_min[2]= ADValue[2];
        ADValue_max[3]= ADValue[3];
        ADValue_min[3]= ADValue[3];
    }

}

void Form_ymjc::on_checkBox_CH1_clicked(bool checked)
{
    ADValueCurve->slot_drawCurve(0, checked);
}

void Form_ymjc::on_checkBox_CH2_clicked(bool checked)
{
    ADValueCurve->slot_drawCurve(1, checked);
}

void Form_ymjc::on_checkBox_CH3_clicked(bool checked)
{
    ADValueCurve->slot_drawCurve(2, checked);
}

void Form_ymjc::on_checkBox_CH4_clicked(bool checked)
{
    ADValueCurve->slot_drawCurve(3, checked);
}

void Form_ymjc::timer_getValue_slot()
{
    timer_getValue->stop();
    for(int i=0;i<4;i++)
    {
        yRTC_ADValue[i][currentTimeEnd] = ADValue[i];
//        qDebug()<<"ADValue[i] "<<ADValue[i]<<currentTimeEnd;
    }
    currentTimeEnd++;
    if( currentTimeEnd >= MAX_RTC_AD_NUM)currentTimeEnd=0;
    emit sigADChanged();
    timer_getValue->start(ui->spinBox_time->value());
}
