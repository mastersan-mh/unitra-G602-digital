#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_ui.h"
#include "settingsdialog.h"
#include "CSlidingWindow.hpp"
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
    void P_setpointValueChanged(double value);

    void P_setpointSet0(bool);
    void P_setpointSet33(bool);
    void P_setpointSet45(bool);

    void P_tickEventSimulation();
    void P_tickEventCommon();
    void P_indication_update(double setpoint, double processVariable, double PV_amplitude);

    /**
     * @param intervalx     Интервал между значениями по оси X, мс
     */
    void P_replot(
            const QVector<double> &axis_x,
            const QVector<double> &vSetpoint,
            const QVector<double> &vPV,
            const QVector<double> &vOut
            );

private:

    enum class RunMode
    {
        SIMULATION,
        DEVICE
    };

    RunMode m_runMode;

    MainWindow_Ui * m_ui;
    SettingsDialog *m_settings;
    QSerialPort *m_serial;
    Comm * m_comm;
    Device * m_device;

    double m_sim_processVariable;
    bool m_sim_setpoint_manual;
    int m_accumulated_values;
    unsigned long m_sim_time_ms;

    QTimer * m_timer;
    bool m_paused;

    CTestMotor *m_engine;


#if defined(PID_DISCRETE)
    PID m_pid = PID(0.0, 0.0, 0.0);
#elif defined(PID_RECURRENT)
    PID_recurrent * m_pid = PID_recurrent(0.0, 0.0, 0.0);
#elif defined(PID_RECURRENT_FIXED32)
    PID_recurrent_Fixed32 m_pid = PID_recurrent_Fixed32(
                nostd::Fixed32(0.0, nostd::Fixed32::tag_double),
                nostd::Fixed32(0.0, nostd::Fixed32::tag_double),
                nostd::Fixed32(0.0, nostd::Fixed32::tag_double)
                );
#endif

    CSlidingWindow m_axis_x;
    CSlidingWindow m_valuesSetpoint;
    CSlidingWindow m_valuesPV;
    CSlidingWindow m_valuesOut;

#ifdef APP_USE_CGRAPH
    QList<double> m_valuesSetpoint;
    QList<double> m_valuesMain;
#endif

    double m_sim_setpoint; /**< Simulation setpoint */

    enum class PID_K_Selector
    {
        Kp,
        Ki,
        Kd,
    };

    PID_K_Selector m_kselector;

    void setpoint_valueSet(int value);

    void P_setpointFuncSetValue(int value);

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

    void P_plot_clear();
    void P_runModeChange(bool simulation);

    /** @brief Добавить сырые данные из m_serial в консоль и m_comm*/
    void P_appendRawData();
    /** @brief Прочитать кадр данных для Device */
    void P_readFrame();
    void P_dev_ready_runModeChanged(Device::RunMode mode);
    void P_dev_ready_SPPV(unsigned long time_ms, double defaultSizePolicy, double pv, double out);
    void P_dev_ready_pulsesRead(bool timedout, unsigned err, unsigned ppr);
    void P_dev_ready_runModeRead(bool timedout, unsigned err, Device::RunMode mode);
    void P_dev_ready_pidKoefRead(bool timedout, unsigned err, double Kp, double Ki, double Kd);
    void P_dev_ready_pidKoefWrite(bool timedout, unsigned err);
    void P_dev_ready_speedSetpointRead(bool timedout, unsigned err, double defaultSizePolicy);
    void P_dev_ready_speedSetpointWrite(bool timedout, unsigned err);
    void P_dev_ready_speedPVRead(bool timedout, unsigned err, double pv);
    void P_dev_ready_processStart(bool timedout, unsigned err);
    void P_dev_ready_processStop(bool timedout, unsigned err);
    void P_dev_ready_confStored(bool timedout, unsigned err);

    void P_writeRawData(const QByteArray &data);
    void P_button_rpc_request_2_koef_r(bool);
    void P_button_rpc_request_3_koef_w(bool);
    void P_button_rpc_request_4_speed_SP_r(bool);
    void P_button_rpc_request_5_speed_SP_w(bool);
    void P_button_rpc_request_6_speed_PV_r(bool);
    void P_button_rpc_request_7_process_start(bool);
    void P_button_rpc_request_8_process_stop(bool);
    void P_button_rpc_request_9_conf_store(bool);

    void P_button_driveReqstat_clearBad(bool);
    void P_button_driveReqstat_clearAll(bool);

    void P_device_status_update(Device::RunMode mode) const;
    const QString P_device_mode_string_get(Device::RunMode mode) const;

};

#endif // MAINWINDOW_H
