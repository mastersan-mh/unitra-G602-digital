#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_ui.h"
#include "settingsdialog.h"

#include "ctestmotor.h"

#include "pid/pid.h"
#include "comm.hpp"
#include "Device.hpp"

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

    void P_mainEvent();

private:

    MainWindow_Ui * m_ui;
    SettingsDialog *m_settings;
    QSerialPort *m_serial;
    Comm * m_comm;
    Device * m_device;

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

    void setpointFuncSetValue(int value_);

    double map_integer_to_double(
            int iv,
            int imin,
            int imax,
            double dmin,
            double dmax
            );

private slots:
    void P_openSerialPort();
    void P_closeSerialPort();

    void P_device_reqstats_update();

    void P_runModeChange(bool simulation);

    /** @brief Добавить сырые данные из m_serial в консоль и m_comm*/
    void P_appendRawData();
    /** @brief Прочитать кадр данных для Device */
    void P_readFrame();
    void P_dev_ready_toDisconnect(bool timedout);
    void P_dev_ready_runModeChanged(Device::RunMode mode);
    void P_dev_ready_SPPV(unsigned long time_ms, double sp, double pv);
    void P_dev_ready_runModeRead(bool timedout, Device::RunMode mode);
    void P_dev_ready_pidKoefRead(bool timedout, double Kp, double Ki, double Kd);
    void P_dev_ready_pidKoefWrite(bool timedout);
    void P_dev_ready_speedSetpointRead(bool timedout, double sp);
    void P_dev_ready_speedSetpointWrite(bool timedout);
    void P_dev_ready_speedPVRead(bool timedout, double pv);

    void P_writeRawData(const QByteArray &data);
    void P_button_rpc_request_0_pulses_r(bool);
    void P_button_rpc_request_1_mode_r(bool);
    void P_button_rpc_request_2_koef_r(bool);
    void P_button_rpc_request_3_koef_w(bool);
    void P_button_rpc_request_4_speed_SP_r(bool);
    void P_button_rpc_request_5_speed_SP_w(bool);
    void P_button_rpc_request_6_speed_PV_r(bool);
    void P_button_rpc_request_7_process_start(bool);
    void P_button_rpc_request_8_process_stop(bool);

    void P_device_status_update(Device::RunMode mode) const;
    const QString P_device_mode_string_get(Device::RunMode mode) const;

};

#endif // MAINWINDOW_H
