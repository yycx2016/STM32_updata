#-------------------------------------------------
#
# Project created by QtCreator 2012-02-14T12:39:43
#
#-------------------------------------------------
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core gui network printsupport sql
#QT       += serialport
QT        += serialport svg opengl#testlib
TARGET = stm32updata_1_4_4
#TARGET = ../../QT_TARGET\stm32updata_1_5_20160906
INCLUDEPATH += ./Qwt
MOC_DIR=release/moc
RCC_DIR=release/rcc
UI_DIR=release/ui
OBJECTS_DIR=release/obj
DESTDIR=bin
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qextserial/win_qextserialport.cpp \
    qextserial/qextserialport.cpp \
    qextserial/qextserialbase.cpp \
    Thread_com/MyThread_com.cpp \
    serialcom.cpp \
    form_tool.cpp \
    form_stm32updatatool.cpp \
    extern_data.cpp \
    form_ymjc.cpp \
    Qwt/qwt_widget_overlay.cpp \
    Qwt/qwt_wheel.cpp \
    Qwt/qwt_transform.cpp \
    Qwt/qwt_thermo.cpp \
    Qwt/qwt_text_label.cpp \
    Qwt/qwt_text_engine.cpp \
    Qwt/qwt_text.cpp \
    Qwt/qwt_system_clock.cpp \
    Qwt/qwt_symbol.cpp \
    Qwt/qwt_spline.cpp \
    Qwt/qwt_slider.cpp \
    Qwt/qwt_series_data.cpp \
    Qwt/qwt_scale_widget.cpp \
    Qwt/qwt_scale_map.cpp \
    Qwt/qwt_scale_engine.cpp \
    Qwt/qwt_scale_draw.cpp \
    Qwt/qwt_scale_div.cpp \
    Qwt/qwt_sampling_thread.cpp \
    Qwt/qwt_round_scale_draw.cpp \
    Qwt/qwt_raster_data.cpp \
    Qwt/qwt_point_polar.cpp \
    Qwt/qwt_point_mapper.cpp \
    Qwt/qwt_point_data.cpp \
    Qwt/qwt_point_3d.cpp \
    Qwt/qwt_plot_zoomer.cpp \
    Qwt/qwt_plot_zoneitem.cpp \
    Qwt/qwt_plot_xml.cpp \
    Qwt/qwt_plot_tradingcurve.cpp \
    Qwt/qwt_plot_textlabel.cpp \
    Qwt/qwt_plot_svgitem.cpp \
    Qwt/qwt_plot_spectrogram.cpp \
    Qwt/qwt_plot_spectrocurve.cpp \
    Qwt/qwt_plot_shapeitem.cpp \
    Qwt/qwt_plot_seriesitem.cpp \
    Qwt/qwt_plot_scaleitem.cpp \
    Qwt/qwt_plot_rescaler.cpp \
    Qwt/qwt_plot_renderer.cpp \
    Qwt/qwt_plot_rasteritem.cpp \
    Qwt/qwt_plot_picker.cpp \
    Qwt/qwt_plot_panner.cpp \
    Qwt/qwt_plot_multi_barchart.cpp \
    Qwt/qwt_plot_marker.cpp \
    Qwt/qwt_plot_magnifier.cpp \
    Qwt/qwt_plot_legenditem.cpp \
    Qwt/qwt_plot_layout.cpp \
    Qwt/qwt_plot_item.cpp \
    Qwt/qwt_plot_intervalcurve.cpp \
    Qwt/qwt_plot_histogram.cpp \
    Qwt/qwt_plot_grid.cpp \
    Qwt/qwt_plot_glcanvas.cpp \
    Qwt/qwt_plot_directpainter.cpp \
    Qwt/qwt_plot_dict.cpp \
    Qwt/qwt_plot_curve.cpp \
    Qwt/qwt_plot_canvas.cpp \
    Qwt/qwt_plot_barchart.cpp \
    Qwt/qwt_plot_axis.cpp \
    Qwt/qwt_plot_abstract_barchart.cpp \
    Qwt/qwt_plot.cpp \
    Qwt/qwt_pixel_matrix.cpp \
    Qwt/qwt_picker_machine.cpp \
    Qwt/qwt_picker.cpp \
    Qwt/qwt_panner.cpp \
    Qwt/qwt_painter_command.cpp \
    Qwt/qwt_painter.cpp \
    Qwt/qwt_null_paintdevice.cpp \
    Qwt/qwt_matrix_raster_data.cpp \
    Qwt/qwt_math.cpp \
    Qwt/qwt_magnifier.cpp \
    Qwt/qwt_legend_label.cpp \
    Qwt/qwt_legend_data.cpp \
    Qwt/qwt_legend.cpp \
    Qwt/qwt_knob.cpp \
    Qwt/qwt_interval_symbol.cpp \
    Qwt/qwt_interval.cpp \
    Qwt/qwt_graphic.cpp \
    Qwt/qwt_event_pattern.cpp \
    Qwt/qwt_dyngrid_layout.cpp \
    Qwt/qwt_dial_needle.cpp \
    Qwt/qwt_dial.cpp \
    Qwt/qwt_date_scale_engine.cpp \
    Qwt/qwt_date_scale_draw.cpp \
    Qwt/qwt_date.cpp \
    Qwt/qwt_curve_fitter.cpp \
    Qwt/qwt_counter.cpp \
    Qwt/qwt_compass_rose.cpp \
    Qwt/qwt_compass.cpp \
    Qwt/qwt_column_symbol.cpp \
    Qwt/qwt_color_map.cpp \
    Qwt/qwt_clipper.cpp \
    Qwt/qwt_arrow_button.cpp \
    Qwt/qwt_analog_clock.cpp \
    Qwt/qwt_abstract_slider.cpp \
    Qwt/qwt_abstract_scale_draw.cpp \
    Qwt/qwt_abstract_scale.cpp \
    Qwt/qwt_abstract_legend.cpp \
    drawadvaluecurve.cpp

HEADERS  += mainwindow.h \
    qextserial/win_qextserialport.h \
    qextserial/qextserialport.h \
    qextserial/qextserialbase.h \
    Thread_com/MyThread_com.h \
    extern_data.h \
    mydefine.h \
    serialcom.h \
    form_tool.h \
    form_stm32updatatool.h \
    form_ymjc.h \
    Qwt/qwt_widget_overlay.h \
    Qwt/qwt_wheel.h \
    Qwt/qwt_transform.h \
    Qwt/qwt_thermo.h \
    Qwt/qwt_text_label.h \
    Qwt/qwt_text_engine.h \
    Qwt/qwt_text.h \
    Qwt/qwt_system_clock.h \
    Qwt/qwt_symbol.h \
    Qwt/qwt_spline.h \
    Qwt/qwt_slider.h \
    Qwt/qwt_series_store.h \
    Qwt/qwt_series_data.h \
    Qwt/qwt_scale_widget.h \
    Qwt/qwt_scale_map.h \
    Qwt/qwt_scale_engine.h \
    Qwt/qwt_scale_draw.h \
    Qwt/qwt_scale_div.h \
    Qwt/qwt_sampling_thread.h \
    Qwt/qwt_samples.h \
    Qwt/qwt_round_scale_draw.h \
    Qwt/qwt_raster_data.h \
    Qwt/qwt_point_polar.h \
    Qwt/qwt_point_mapper.h \
    Qwt/qwt_point_data.h \
    Qwt/qwt_point_3d.h \
    Qwt/qwt_plot_zoomer.h \
    Qwt/qwt_plot_zoneitem.h \
    Qwt/qwt_plot_tradingcurve.h \
    Qwt/qwt_plot_textlabel.h \
    Qwt/qwt_plot_svgitem.h \
    Qwt/qwt_plot_spectrogram.h \
    Qwt/qwt_plot_spectrocurve.h \
    Qwt/qwt_plot_shapeitem.h \
    Qwt/qwt_plot_seriesitem.h \
    Qwt/qwt_plot_scaleitem.h \
    Qwt/qwt_plot_rescaler.h \
    Qwt/qwt_plot_renderer.h \
    Qwt/qwt_plot_rasteritem.h \
    Qwt/qwt_plot_picker.h \
    Qwt/qwt_plot_panner.h \
    Qwt/qwt_plot_multi_barchart.h \
    Qwt/qwt_plot_marker.h \
    Qwt/qwt_plot_magnifier.h \
    Qwt/qwt_plot_legenditem.h \
    Qwt/qwt_plot_layout.h \
    Qwt/qwt_plot_item.h \
    Qwt/qwt_plot_intervalcurve.h \
    Qwt/qwt_plot_histogram.h \
    Qwt/qwt_plot_grid.h \
    Qwt/qwt_plot_glcanvas.h \
    Qwt/qwt_plot_directpainter.h \
    Qwt/qwt_plot_dict.h \
    Qwt/qwt_plot_curve.h \
    Qwt/qwt_plot_canvas.h \
    Qwt/qwt_plot_barchart.h \
    Qwt/qwt_plot_abstract_barchart.h \
    Qwt/qwt_plot.h \
    Qwt/qwt_pixel_matrix.h \
    Qwt/qwt_picker_machine.h \
    Qwt/qwt_picker.h \
    Qwt/qwt_panner.h \
    Qwt/qwt_painter_command.h \
    Qwt/qwt_painter.h \
    Qwt/qwt_null_paintdevice.h \
    Qwt/qwt_matrix_raster_data.h \
    Qwt/qwt_math.h \
    Qwt/qwt_magnifier.h \
    Qwt/qwt_legend_label.h \
    Qwt/qwt_legend_data.h \
    Qwt/qwt_legend.h \
    Qwt/qwt_knob.h \
    Qwt/qwt_interval_symbol.h \
    Qwt/qwt_interval.h \
    Qwt/qwt_graphic.h \
    Qwt/qwt_global.h \
    Qwt/qwt_event_pattern.h \
    Qwt/qwt_dyngrid_layout.h \
    Qwt/qwt_dial_needle.h \
    Qwt/qwt_dial.h \
    Qwt/qwt_date_scale_engine.h \
    Qwt/qwt_date_scale_draw.h \
    Qwt/qwt_date.h \
    Qwt/qwt_curve_fitter.h \
    Qwt/qwt_counter.h \
    Qwt/qwt_compat.h \
    Qwt/qwt_compass_rose.h \
    Qwt/qwt_compass.h \
    Qwt/qwt_column_symbol.h \
    Qwt/qwt_color_map.h \
    Qwt/qwt_clipper.h \
    Qwt/qwt_arrow_button.h \
    Qwt/qwt_analog_clock.h \
    Qwt/qwt_abstract_slider.h \
    Qwt/qwt_abstract_scale_draw.h \
    Qwt/qwt_abstract_scale.h \
    Qwt/qwt_abstract_legend.h \
    Qwt/qwt.h \
    drawadvaluecurve.h

FORMS    += mainwindow.ui \
    form_tool.ui \
    form_stm32updatatool.ui \
    form_ymjc.ui

RC_FILE = myapp.rc

RESOURCES += \
    mysource.qrc

OTHER_FILES += \
    Qwt/src.pro \
    Qwt/src.pri
