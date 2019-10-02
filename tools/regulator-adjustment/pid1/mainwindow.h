#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ctestmotor.h"

#include "pid/pid.h"

#include "qcustomplot.h"

#include <QMainWindow>

#include <QLayout>
#include <QHBoxLayout>

#include <QGroupBox>

#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <QRadioButton>
#include <QCheckBox>

#include <QTimer>
#include <QTime>

#include <QGraphicsScale>

#ifdef APP_USE_CGRAPH
#   include"cgraph.h"
#endif

namespace Ui {
class MainWindow;
}


#include <QWidget>
#include <QLabel>


class CPIDK: public QGroupBox
{
    Q_OBJECT
public:
    explicit CPIDK(QWidget* parent=0);
    explicit CPIDK(const QString &title, QWidget* parent=0);
    ~CPIDK();

    inline double valueKp()
    {
        return Kp_spinBox->value();
    }

    inline double valueKi()
    {
        return Ki_spinBox->value();
    }

    inline double valueKd()
    {
        return Kd_spinBox->value();
    }

    inline void setValueKp(double value)
    {
        return Kp_spinBox->setValue(value);
    }

    inline void setValueKi(double value)
    {
        return Ki_spinBox->setValue(value);
    }

    inline void setValueKd(double value)
    {
        return Kd_spinBox->setValue(value);
    }

signals:
    void valuesChanged(double Kp, double Ki, double Kd);
private slots:
    void K_changed(double);
private:
    void init();

    QVBoxLayout * mainLayout;
    // Kp -  proportional gain
    QDoubleSpinBox * Kp_spinBox;
    // Ki -  Integral gain
    QDoubleSpinBox * Ki_spinBox;
    // Kd -  derivative gain
    QDoubleSpinBox * Kd_spinBox;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void pidK_changed(double Kp, double Ki, double Kd);
    void setpoint_ManualModeSelected(bool toggled);
    void setpoint_AutoModeSelected(bool toggled);

    void selection_leftValue_changed(double value);
    void selection_rightValue_changed(double value);
    void selection_tryLeft_clicked(bool);
    void selection_tryCenter_clicked(bool);
    void selection_tryRight_clicked(bool);
    void selection_setCenterToLeft_clicked(bool);
    void selection_setCenterToRight_clicked(bool);

    void Kselector_Kp_select(bool);
    void Kselector_Ki_select(bool);
    void Kselector_Kd_select(bool);

    void pause_unpause(bool);

    /* установить значение функции уставки */
    void setpoint_sliderChangeValue(int value);
    void setpoint_spinBoxChangeValue(int value);

    void mainEvent();

private:

    double m_processVariable;
    bool m_setpoint_manual;
    int m_accumulated_values;
    double m_axis_x_shift;

    QTimer * m_timer;
    long m_time;
    bool m_paused;

    CTestMotor *m_engine;


#if defined(PID_DISCRETE)
    PID * m_pid;
#elif defined(PID_RECURRENT)
    PID_recurrent * m_pid;
#elif defined(PID_RECURRENT_FIXED32)
    PID_recurrent_Fixed32 * m_pid;
#endif



    QVector<double> m_valuesSetpoint;
    QVector<double> m_valuesPV;

#ifdef APP_USE_CGRAPH
    QList<double> m_valuesSetpoint;
    QList<double> m_valuesMain;
#endif

    double m_setpoint;

    enum class PID_K_Selector
    {
        Kp,
        Ki,
        Kd,
    };

    PID_K_Selector m_kselector;

    void setpoint_valueSet(int value);

    QGroupBox *init_UI_pidSetup(QWidget * parent);
    QGroupBox *init_UI_Kselector(QWidget * parent);

    void setpointFuncSetValue(int value_);

    double map_integer_to_double(
            int iv,
            int imin,
            int imax,
            double dmin,
            double dmax
            );
    /* visual components, UI */
    void init_UI();

    struct
    {
        QWidget * central;

        QHBoxLayout *centralWLayout;

        QVBoxLayout *ctrl1_Layout;

        /* column1 */
        struct Selection
        {
            /*
          LLLL           CCCC             RRRR
          <try-left>               <try-right>
          <set-left-center> <set-center-right>
        */
            QGroupBox * main;

            QHBoxLayout * mainLayout;

            QVBoxLayout * leftLayout;
            QDoubleSpinBox * leftValue;
            QPushButton * tryLeft;
            QPushButton * setCenterToLeft;

            QVBoxLayout * centerLayout;
            QDoubleSpinBox * centerValue;
            QPushButton * tryCenter;

            QVBoxLayout * rightLayout;
            QDoubleSpinBox * rightValue;
            QPushButton * tryRight;
            QPushButton * setCenterToRight;

        } selection;

        /* column2 */
        QHBoxLayout * pidK_Kselector_Layout;
        CPIDK * pidK;
        QGroupBox * Kselector;

        QLabel * processVariable_indicator;
        QLabel * SPminusPV_indicator;
        QLabel * output_indicator;

        QPushButton * pausePushButton;

        /* column3 */
        QVBoxLayout * setpoint_Layout;
        QSlider * setpoint_slider;
        QSpinBox * setpoint_spinBox;

        QVBoxLayout *cplotCtrl_Layout;
        QCheckBox * cplotAutoScale;

#ifdef APP_USE_CGRAPH
        CGraph * graph;
        CGraphPlot * plotSetpoint;
        CGraphPlot * plotMain;
#endif

        QCustomPlot * cplot;
        QCPGraph * plotSetpoint;
        QCPGraph * plotPV;
    } m_ui;

};

#endif // MAINWINDOW_H
