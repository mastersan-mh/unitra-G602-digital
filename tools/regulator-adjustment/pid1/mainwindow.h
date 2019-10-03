#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_ui.h"
#include "settingsdialog.h"

#include "ctestmotor.h"

#include "pid/pid.h"

class Parser: public QObject
{
    Q_OBJECT
public:
    Parser();
    virtual ~Parser();
    void reset();
public slots:
    void dataRead(const QByteArray &data);
signals:
    void stringReady(const QString &);
private:
    bool P_string_append(QByteArray::const_iterator &from, const QByteArray::const_iterator to, QString &str);
    QString m_str;
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

    MainWindow_Ui * m_ui;
    SettingsDialog *m_settings;
    QSerialPort *m_serial;
    Parser * m_parser;

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
    void P_writeRawData(const QByteArray &data);
    void P_readRawData();
    void P_readParcedData(const QString &str);

};

#endif // MAINWINDOW_H
