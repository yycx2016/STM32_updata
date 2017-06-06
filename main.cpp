#include <QApplication>
#include "mainwindow.h"
#include "extern_data.h"
#include <qtextcodec.h>
#include <QStyle>
#include <QFont>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

       QApplication::addLibraryPath("plugins/");
       //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
       //QTextCodec::setCodecForLocale(QTextCodec::codecForUtfText("UTF-8"));
       //Q_INIT_RESOURCE(myresource);

    serialComTool = new serialCOM();

    MainWindow w;
    w.show();

    return a.exec();
}
