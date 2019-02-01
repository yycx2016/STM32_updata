#include "drawadvaluecurve.h"
#include <QPainter>
#include <QDebug>
#include <math.h>
#include <QGridLayout>

QColor RTC_T_Color[4]={Qt::black,Qt::red,Qt::green,Qt::blue};

//quint16 RTC_ADValue[4][MAX_RTC_AD_NUM];   //采集温度值
double yRTC_ADValue[4][MAX_RTC_AD_NUM];// 实时曲线
//double xData[MAX_RTC_AD_NUM];
quint16 currentTimeEnd=100;

DrawADValueCurve::DrawADValueCurve(QWidget *parent) :
    QWidget(parent)
{
    qwtplot_ADView=new QwtPlot();
//设置坐标轴的名称
    qwtplot_ADView->setAxisTitle(QwtPlot::yLeft, "AD寄存器值->");
    qwtplot_ADView->setAxisTitle(QwtPlot::xBottom, "采集间隔");
 //设置坐标轴的范围
    qwtplot_ADView->setAxisScale(QwtPlot::yLeft,0,4096,0);
    qwtplot_ADView->setAxisScale(QwtPlot::xBottom,0,MAX_RTC_AD_NUM,0);
//    qwtplot_ADView->enableAxis(QwtPlot::xBottom,false);
//设置右边标注
//  qwtplot_ADView->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
//使用滚轮放大/缩小
  //(void) new QwtPlotMagnifier( qwtplot_TemView->canvas() );
    QwtPlotMagnifier* plotmagnifier=new QwtPlotMagnifier( qwtplot_ADView->canvas() );
    plotmagnifier->setAxisEnabled(QwtPlot::yLeft,false);
    //plotmagnifier->setAxisEnabled(QwtPlot::xBottom,false);
   // plotmagnifier->setMouseButton();
//使用鼠标左键平移
  //(void) new QwtPlotPanner( qwtplot_TemView->canvas() );
//    QwtPlotPanner*plotpanner=new QwtPlotPanner( qwtplot_ADView->canvas() );
//    plotpanner->setAxisEnabled(QwtPlot::yLeft,false);
    //plotpanner->setAxisEnabled(QwtPlot::xBottom,false);

    //一个选择器，十字线，以xBottom和yLeft坐标
        QwtPlotPicker*picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                                   QwtPlotPicker::CrossRubberBand, QwtPicker::ActiveOnly,
                                   qwtplot_ADView->canvas());
        picker->setStateMachine(new QwtPickerDragPointMachine());//拖拽点起作用
        picker->setRubberBandPen(QPen(QColor(Qt::yellow)));
        picker->setTrackerPen(QColor(Qt::black));
        connect(picker,SIGNAL(selected(QPointF )),this,SLOT(slot_select( QPointF  )));

    //鼠标左键选择区域放大：（右键还原）
             QwtPlotZoomer* zoomer = new QwtPlotZoomer( qwtplot_ADView->canvas() );
            zoomer->setRubberBandPen( QColor( Qt::black ) );
            zoomer->setTrackerPen( QColor( Qt::black ) );
            zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier );
            zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton );

    //增加网格
        QwtPlotGrid *grid = new QwtPlotGrid;
        grid->setMajorPen(QPen(Qt::lightGray, 0, Qt::DotLine));
        grid->attach(qwtplot_ADView);
        for (quint8 zt_num = 0; zt_num < 4; zt_num++)
        {
            ADCurve[zt_num].setPen(QPen(RTC_T_Color[zt_num], 1));
            //ADCurve[zt_num].setCurveAttribute(QwtPlotCurve::Fitted, true);//使曲线更光滑，不加这句曲线会很硬朗，有折点
            ADCurve[zt_num].setStyle(QwtPlotCurve::Lines);//直线形式
            ADCurve[zt_num].attach(qwtplot_ADView);//把曲线附加到plot上
            isDrawCurve[zt_num] = true;
        }

        for (quint16 time_j = 0; time_j < MAX_RTC_AD_NUM ; time_j++)
        {
            xData[time_j]= time_j;
        }

    /*******************************************************/
        qwtplot_ADView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        QGridLayout*gridLayout = new QGridLayout(this);
        gridLayout->setSpacing(0);
        gridLayout->setMargin(0);
        gridLayout->addWidget(qwtplot_ADView);
        this->setLayout(gridLayout);
}

void DrawADValueCurve::slot_select(QPointF point)
{
   qDebug()<<point.x()<<point.y();
}
/*
 *@brief 还原温度曲线
 */
void DrawADValueCurve::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //qwtplot_TemView->setAxisScale(QwtPlot::yLeft,30,100,0);
    //qwtplot_ADView->setAxisAutoScale(QwtPlot::yLeft, true);
    qwtplot_ADView->setAxisScale(QwtPlot::xBottom,0, currentTimeEnd,0);
    qwtplot_ADView->replot();
}
/*
 *@brief 获取四个载台温度曲线是否显示的槽函数
 */
void DrawADValueCurve::slot_drawCurve(quint8 ZT_num, bool isDraw)
{
    if (ZT_num > 3)
    {
        return;
    }
    isDrawCurve[ZT_num] = isDraw;
    reDrawADCurve();
    this->update();

}
/*
 *@brief 画温度曲线
 */
void DrawADValueCurve::reDrawADCurve()
{
    //quint16 time_end = 0;
    //time_end = getTemperatureNum();
    //特殊情况处理：尚未读取数据
    if (currentTimeEnd == 0) currentTimeEnd= 1;
    else if (currentTimeEnd > MAX_RTC_AD_NUM) currentTimeEnd = MAX_RTC_AD_NUM;
    qwtplot_ADView->setAxisScale(QwtPlot::xBottom,0,currentTimeEnd,0);
//    for(quint8  i = 0; i < 4; i++)
//    {
//        if(isDrawCurve[i])
//        {    //double xData[2][MAX_ZT_NUM][MAX_RTC_T_NUM];//0 目标曲线，1 实时曲线
//            for (quint16 time_j = 0; time_j < time_end ; time_j++)
//            {
//                yData[i][time_j]=((double)RTC_ADValue[i][time_j])/100;
//                xData[time_j]=time_j;
//            }
//        }
//    }
//画曲线
    for(quint8  i = 0; i < 4; i++)
    {
        if(isDrawCurve[i] == true)
        {
            ADCurve[i].setSamples(xData,yRTC_ADValue[i],currentTimeEnd);
        }
    }
    qwtplot_ADView->replot();
//    qwtplot_ADView->setAxisScale(QwtPlot::yLeft,0,4095,100);//固定间距
    qwtplot_ADView->setAxisAutoScale(QwtPlot::yLeft, true);
//    qDebug()<<"时间"<<currentTimeEnd;
   // qwtplot_ADView->setAxisAutoScale(QwtPlot::xBottom, true);

}

