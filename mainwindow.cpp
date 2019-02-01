#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extern_data.h"
#include <QSettings>
#include <QtSerialPort/QtSerialPort>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("在线升级软件_1.2.4"));

    stm32UpdataTool = new Form_stm32UpdataTool(this);
    ui->tab->layout()->addWidget(stm32UpdataTool);
    connect(stm32UpdataTool,SIGNAL(modeChanged()),this,SLOT(updataComStatus()));
    serialTool = new Form_tool(this);
    ui->tab_2->layout()->addWidget(serialTool);
    connect(serialTool,SIGNAL(modeChanged()),this,SLOT(updataComStatus()));  
    QyYmJC = new Form_ymjc(this);
    ui->tab_3->layout()->addWidget(QyYmJC);
    connect(QyYmJC,SIGNAL(modeChanged()),this,SLOT(updataComStatus()));

    label_comStatus = new QLabel(tr("状态:串口未打开"));
    ui->statusBar->addWidget(label_comStatus, 1);
    ui->statusBar->addWidget(stm32UpdataTool->progressBar, 3);

    gCom_QueryMode = 1;
    ui->comboBox_mode->setCurrentIndex(1);
    ui->tabWidget->setCurrentIndex(1);

   foreachComPort();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updataComStatus()
{
    QString comStateString ;
    if(com_status == false)
    {
        comStateString = "串口关闭";
    }
    else
    {
       comStateString = "串口打开";
    }
    if(gCom_QueryMode==0)
    {
        //serialComTool->comDevice->setTimeout(-1);
        label_comStatus->setText(tr("状态:%1 -- 升级模式").arg(comStateString));
        disconnect(serialComTool->comDevice,SIGNAL(readyRead()),serialTool,SLOT(readMyCom()));
        disconnect(serialComTool->comDevice,SIGNAL(readyRead()),QyYmJC,SLOT(readMyCom()));
    }
    else if(gCom_QueryMode == 2)
    {
        label_comStatus->setText(tr("状态:%1 -- 曲线模式").arg(comStateString));
//        label_comStatus->setText(tr("状态:串口打开 -- 曲线模式"));
        com_status=false;
        serialComTool->closecom();
        openSerial_slot();// 打开串口
        disconnect(serialComTool->comDevice,SIGNAL(readyRead()),serialTool,SLOT(readMyCom()));
        connect(serialComTool->comDevice,SIGNAL(readyRead()),QyYmJC,SLOT(readMyCom()));
    }
    else
    {
        //serialComTool->comDevice->setTimeout(100);
//        label_comStatus->setText(tr("状态:串口打开 -- 调试模式"));
        label_comStatus->setText(tr("状态:%1 -- 调试模式").arg(comStateString));
        com_status=false;
        serialComTool->closecom();
        openSerial_slot();// 打开串口
        disconnect(serialComTool->comDevice,SIGNAL(readyRead()),QyYmJC,SLOT(readMyCom()));
        connect(serialComTool->comDevice,SIGNAL(readyRead()),serialTool,SLOT(readMyCom()));
    }
    ui->comboBox_mode->setCurrentIndex(gCom_QueryMode);
}

void MainWindow::foreachComPort()
{
    if (QFile::exists("./Serial.ini"))
    {
        QSettings settingsTube("./Serial.ini", QSettings::IniFormat);
        QString str;
        settingsTube.beginGroup("Serial");
        str =  settingsTube.value("COM").toString();
        int x = ui->selectserialComboBox->findText(str);
        if (x > 0)
        {
          ui->selectserialComboBox->setCurrentIndex(x);
        }

        str = settingsTube.value("Baud").toString();
        x = ui->selectBuadComboBox->findText(str);
        if (x > 0)
        {
          ui->selectBuadComboBox->setCurrentIndex(x);
        }

        str = settingsTube.value("TestBit").toString();
         x = ui->selsetTestbitComboBox->findText(str);
        if (x > 0)
        {
          ui->selsetTestbitComboBox->setCurrentIndex(x);
        }

        str = settingsTube.value("DataBit").toString();
         x = ui->selectDatabitComboBox->findText(str);
        if (x > 0)
        {
          ui->selectDatabitComboBox->setCurrentIndex(x);
        }

        str = settingsTube.value("StopBit").toString();
         x = ui->selectStopbitComboBox->findText(str);
        if (x > 0)
        {
          ui->selectStopbitComboBox->setCurrentIndex(x);
        }
    }

    ui->selectserialComboBox->clear();
    //qWarning("cyycyh\n");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
   {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            ui->selectserialComboBox->addItem(info.portName());
            serial.close();
        }
    }
    if(ui->selectserialComboBox->count() == 0)
    {
        ui->selectserialComboBox->clear();
        ui->selectserialComboBox->addItem("COM0");
        ui->selectserialComboBox->addItem("COM1");
        ui->selectserialComboBox->addItem("COM2");
        ui->selectserialComboBox->addItem("COM3");
        ui->selectserialComboBox->addItem("COM4");
        ui->selectserialComboBox->addItem("COM5");
        ui->selectserialComboBox->addItem("COM6");
        ui->selectserialComboBox->addItem("COM7");
        ui->selectserialComboBox->addItem("COM8");
        ui->selectserialComboBox->addItem("COM9");
        ui->selectserialComboBox->addItem("COM10");
        ui->selectserialComboBox->addItem("COM11");
    }
}

void MainWindow::on_pushButton_dis_Help_clicked()
{
    if(1)
    {
        foreachComPort();
    }
    else
    {
        QMessageBox msgBox(this);
        QString	mymes = this->windowTitle().append(tr("\n\n基于 QT5.2.1&WinXp 的串口调试工具\n\nAuthor：WHSu"));
        QString pix;
        pix = ":/thank.png";
        msgBox.setIconPixmap(pix);
        msgBox.setWindowTitle(tr("关于"));
        msgBox.setText(tr("\n\n\n感谢使用本串口调试工具！"));
        msgBox.setDetailedText (mymes);
        msgBox.exec();
    }

}

void MainWindow::on_openCloseSerialPushButton_clicked()// 打开串口
{
    if(com_status == false)
    {
       openSerial_slot();// 打开串口
    }
    else
    {
        com_status=false;
        serialComTool->closecom();
        ui->openCloseSerialPushButton->setText(tr("打开串口"));
        label_comStatus->setText(tr("状态:串口关闭"));
        ui->serialstatusLabel->setPixmap(QPixmap(":/off.jpg"));
    }
}

void MainWindow::openSerial_slot()// 打开串口
{
    char com_Testbit='n';
    QString path;
    QString com_Num,com_Buad,Testbit, com_Databit, com_Stopbit;

    com_Num = ui->selectserialComboBox->currentText();
    com_Buad = ui->selectBuadComboBox->currentText();
    Testbit = ui->selsetTestbitComboBox->currentText();
    com_Databit = ui->selectDatabitComboBox->currentText();
    com_Stopbit = ui->selectStopbitComboBox->currentText();
    if(Testbit == tr("无校验(NONE)"))com_Testbit='n';
    else if(Testbit == tr("偶校验(EVEN)"))com_Testbit='e';
    else if(Testbit == tr("奇校验(ODD)"))com_Testbit='o';
    else if(Testbit == tr("MAKE校验"))com_Testbit='m';
    else if(Testbit == tr("空校验(SPACE)"))com_Testbit='s';
    else com_Testbit='n';
    com_status = serialComTool->opencom(com_Num,
                                      com_Buad.toInt(),
                                      com_Testbit,com_Databit.toInt(),
                                      com_Stopbit.toInt()
                                      );//int com_QueryMode

    if(com_status)
    {
            path = ":/on.jpg";
            ui->openCloseSerialPushButton->setText(tr("关闭串口"));
            if(gCom_QueryMode==0)
            {
                label_comStatus->setText(tr("状态:串口打开 -- 升级模式"));
            }
            else if(gCom_QueryMode == 2)
            {
                label_comStatus->setText(tr("状态:串口打开 -- 曲线模式"));
            }
             else
            {
                label_comStatus->setText(tr("状态:串口打开 -- 调试模式"));
            }

            QSettings settingsTube("./Serial.ini", QSettings::IniFormat);
            QString str;
            str = ui->selectserialComboBox->currentText();
            settingsTube.setValue("Serial/COM", str);
            str = ui->selectBuadComboBox->currentText();
            settingsTube.setValue("Serial/Baud", str);
            str = ui->selsetTestbitComboBox->currentText();
            settingsTube.setValue("Serial/TestBit", str);
            str = ui->selectDatabitComboBox->currentText();
            settingsTube.setValue("Serial/DataBit", str);
            str = ui->selectStopbitComboBox->currentText();
            settingsTube.setValue("Serial/StopBit", str);
    }
    else
    {
            path = ":/error.jpg";
            ui->openCloseSerialPushButton->setText(tr("打开串口"));
            label_comStatus->setText(tr("状态:串口关闭"));
    }

    ui->serialstatusLabel->setPixmap(path);
}


void MainWindow::on_comboBox_mode_currentIndexChanged(int index)
{
    gCom_QueryMode = index;
//    qDebug() << index;
    updataComStatus();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    gCom_QueryMode = index;
//    qDebug() << index;
    if(index != 0)
    {
     updataComStatus();
    }
}
