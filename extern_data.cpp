
#include "extern_data.h"

volatile bool stopthread_flag=false;
bool start_flags=false;
bool com_status=false;
bool isALLAutoUpdata=false;
unsigned char receiveDataBuf[10];
unsigned char programDataBuf[20];
int removepagenum=1;
int gCom_QueryMode=0;

serialCOM *serialComTool;


