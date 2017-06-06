#ifndef DRAWADVALUECURVE_H
#define DRAWADVALUECURVE_H

#include <QWidget>
#include <QScrollBar>
#include <qmath.h>
#include <QVector>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_magnifier.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_panner.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_picker.h>
#include <qwt_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_point_data.h>
#include <qwt_plot_zoomer.h>

#define MAX_RTC_AD_NUM 5000

extern double yRTC_ADValue[4][MAX_RTC_AD_NUM];// 实时曲线
extern quint16 currentTimeEnd;

class DrawADValueCurve : public QWidget
{
    Q_OBJECT
public:
    explicit DrawADValueCurve(QWidget *parent = 0);

signals:

public slots:
    void slot_drawCurve(quint8 ZT_num, bool isDraw);
protected:
//    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
private slots:
    void reDrawADCurve();
private:
    QwtPlot *qwtplot_ADView;
    bool isDrawCurve[4];// 四个载台曲线显示标志位
    QwtPlotCurve ADCurve[4];// 实时曲线
    //QwtPlotCurve curveCutOff;
    QwtPlotMarker markerInfo;
    //QwtPlotCurve curve_r;// 硬折点曲线画布
    //QVector<double> xs;
    //QVector<double> ys;
//    double yData[4][MAX_RTC_AD_NUM];// 实时曲线
    double xData[MAX_RTC_AD_NUM];

};

#endif // DRAWADVALUECURVE_H
