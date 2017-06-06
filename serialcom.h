#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <QObject>
#include "qextserial/win_qextserialport.h"

class serialCOM : public QObject
{
    Q_OBJECT
public:
    explicit serialCOM();
    Win_QextSerialPort *comDevice;
    int comTimeout;
private:
     unsigned char send_address[2];
     unsigned char send_data[7];
     unsigned char Readserial[7];

signals:
    void Error_info(const QString&);

public slots:
    /*****************************************************/
    bool opencom(const QString &com_Num, int com_Buad, char com_Testbit, int com_Databit, int com_Stopbit );
    void closecom();
    bool send_slot(const char *sendDataBuf,int len);
    /*****************************************************/

    
};

#endif // SERIALCOM_H
