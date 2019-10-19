
#include "defs.h"

#include "mainwindow.h"

#include "nostd/fixed/fixed32"
#include "nostd/fixed/fixed32.h"

#include "ctestmotor.h"
#include "pid/fan.h"
#include "pid/pidadjustp.h"

#include <QtMath>

#include <algorithm>


/*
http://www.controlplast.ru/site/index.php?/rinforms/kdocuments/nastroykapid
https://www.bookasutp.ru/Chapter5_5.aspx
http://lazysmart.ru/osnovy-avtomatiki/nastrojka-pid-regulyatora/
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new MainWindow_Ui)
    , m_device(new Device)
    , m_axis_x(1)
    , m_valuesSetpoint(1)
    , m_valuesPV(1)
    , m_valuesOut(1)
{
    m_ui->init_UI(this);

    m_settings = new SettingsDialog;
    m_serial = new QSerialPort(this);
    m_comm = new Comm();

    m_engine = new CTestMotor(0.1);

    m_ui->mainMenu.actionConnect->setEnabled(true);
    m_ui->mainMenu.actionDisconnect->setEnabled(false);
    m_ui->mainMenu.actionQuit->setEnabled(true);
    m_ui->mainMenu.actionConfigure->setEnabled(true);
    m_ui->mainMenu.actionRunMode->setEnabled(true);

#if defined(PID_DISCRETE)
    pid = new PID(0.0, 0.0, 0.0);
#elif defined(PID_RECURRENT)
    pid = new PID_recurrent(0.0, 0.0, 0.0);
#elif defined(PID_RECURRENT_FIXED32)
    m_pid = new PID_recurrent_Fixed32(nostd::Fixed32(0.0), nostd::Fixed32(0.0), nostd::Fixed32(0.0));
#endif

    m_processVariable = 0.0;

    m_timer = new QTimer();
    m_timer->setSingleShot(false);
    //timer->setTimerType(Qt::PreciseTimer);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(P_tickEventSimulation()));

    m_time = 0;

    m_kselector = PID_K_Selector::Kp;
    m_paused = false;

    /* graph */
#ifdef APP_USE_CGRAPH
    graph = new CGraph(central);
    mainLayout->addWidget(graph);
    plotSetpoint = graph->addPlot(QColor(Qt::red));
    plotMain = graph->addPlot(QColor(Qt::yellow));
#endif

    QColor axisColor(Qt::green);
    QPen axisPen(axisColor);
    axisPen.setColor(axisColor);

    m_ui->cplot->setBackground(QBrush(QColor(Qt::black)));

    m_ui->cplot->yAxis->setLabelColor(axisColor);
    m_ui->cplot->yAxis->setBasePen(axisPen);
    m_ui->cplot->yAxis->setTickLabelColor(axisColor);
    m_ui->cplot->yAxis->setTickPen(axisPen);
    m_ui->cplot->yAxis->setSubTickPen(axisPen);
    m_ui->cplot->yAxis->setRange(AXIS_Y_MIN, AXIS_Y_MAX);

    m_ui->cplot->yAxis2->setLabelColor(axisColor);
    m_ui->cplot->yAxis2->setBasePen(axisPen);
    m_ui->cplot->yAxis2->setTickLabelColor(axisColor);
    m_ui->cplot->yAxis2->setTickPen(axisPen);
    m_ui->cplot->yAxis2->setSubTickPen(axisPen);
    m_ui->cplot->yAxis2->setRange(0, -300.0, Qt::AlignRight);
    m_ui->cplot->yAxis2->setVisible(true);

    m_ui->cplot->xAxis->setBasePen(axisPen);
    m_ui->cplot->xAxis->setLabel("time");
    m_ui->cplot->xAxis->setLabelColor(axisColor);
    m_ui->cplot->xAxis->setTickLabelColor(axisColor);
    m_ui->cplot->xAxis->setTickPen(axisPen);
    m_ui->cplot->xAxis->setSubTickPen(axisPen);
    m_ui->cplot->xAxis->setRange(AXIS_X_MIN, AXIS_X_MAX);

    m_ui->plotSetpoint = m_ui->cplot->addGraph();
    m_ui->plotSetpoint->setPen(QPen(QColor(Qt::white)));

    m_ui->plotPV = m_ui->cplot->addGraph();
    m_ui->plotPV->setPen(QPen(QColor(Qt::red)));

    m_ui->plotOut = m_ui->cplot->addGraph(m_ui->cplot->xAxis, m_ui->cplot->yAxis2);
    m_ui->plotOut->setPen(QPen(QColor(Qt::cyan)));

    QObject::connect(m_ui->mainMenu.actionConnect   , SIGNAL(triggered()), this, SLOT(P_openSerialPort()));
    QObject::connect(m_ui->mainMenu.actionDisconnect, SIGNAL(triggered()), this, SLOT(P_closeSerialPort()));
    QObject::connect(m_ui->mainMenu.actionQuit      , SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(m_ui->mainMenu.actionConfigure , SIGNAL(triggered()), m_settings, SLOT(show()));
    QObject::connect(m_ui->mainMenu.actionClear     , SIGNAL(triggered()), m_ui->tab.device->m_console , SLOT(clear()));
    QObject::connect(m_ui->mainMenu.actionRunMode   , SIGNAL(toggled(bool)), this , SLOT(P_runModeChange(bool)));

    QObject::connect(m_serial, SIGNAL(readyRead()), this, SLOT(P_appendRawData()));
    QObject::connect(m_ui->tab.device->m_console, SIGNAL(getData(const QByteArray &)), this, SLOT(P_writeRawData(const QByteArray &)));
    QObject::connect(m_comm, SIGNAL(readyFrameToRead()), this, SLOT(P_readFrame()));
    QObject::connect(m_comm, SIGNAL(readyOutputStream(const QByteArray &)), this, SLOT(P_writeRawData(const QByteArray &)));
    QObject::connect(m_device, SIGNAL(ready_dataToSend(const QByteArray &)), m_comm, SLOT(writeFrame(const QByteArray&)));
    QObject::connect(m_device, SIGNAL(ready_runModeChanged(Device::RunMode)          ), this, SLOT(P_dev_ready_runModeChanged(Device::RunMode)          ));
    QObject::connect(m_device, SIGNAL(ready_SPPV(unsigned long, double, double, double)        ), this, SLOT(P_dev_ready_SPPV(unsigned long, double, double, double)        ));

    QObject::connect(m_device, SIGNAL(ready_pulsesRead(bool, unsigned, unsigned)               ), this, SLOT(P_dev_ready_pulsesRead(bool, unsigned, unsigned)               ));
    QObject::connect(m_device, SIGNAL(ready_runModeRead(bool, unsigned, Device::RunMode)       ), this, SLOT(P_dev_ready_runModeRead(bool, unsigned, Device::RunMode)       ));
    QObject::connect(m_device, SIGNAL(ready_pidKoefRead(bool, unsigned, double, double, double)), this, SLOT(P_dev_ready_pidKoefRead(bool, unsigned, double, double, double)));
    QObject::connect(m_device, SIGNAL(ready_pidKoefWrite(bool, unsigned)                       ), this, SLOT(P_dev_ready_pidKoefWrite(bool, unsigned)                       ));
    QObject::connect(m_device, SIGNAL(ready_speedSetpointRead(bool, unsigned, double)          ), this, SLOT(P_dev_ready_speedSetpointRead(bool, unsigned, double)          ));
    QObject::connect(m_device, SIGNAL(ready_speedSetpointWrite(bool, unsigned)                 ), this, SLOT(P_dev_ready_speedSetpointWrite(bool, unsigned)                 ));
    QObject::connect(m_device, SIGNAL(ready_speedPVRead(bool, unsigned, double)                ), this, SLOT(P_dev_ready_speedPVRead(bool, unsigned, double)                ));
    QObject::connect(m_device, SIGNAL(ready_processStart(bool, unsigned)                       ), this, SLOT(P_dev_ready_processStart(bool, unsigned)                       ));
    QObject::connect(m_device, SIGNAL(ready_processStop(bool, unsigned)                        ), this, SLOT(P_dev_ready_processStop(bool, unsigned)                        ));
    QObject::connect(m_device, SIGNAL(ready_confStored(bool, unsigned)                         ), this, SLOT(P_dev_ready_confStored(bool, unsigned)                         ));

    QObject::connect(
                m_ui->selection.leftValue, SIGNAL(valueChanged(double)),
                this, SLOT(selection_leftValue_changed(double))
                );

    QObject::connect(
                m_ui->selection.tryLeft, SIGNAL(clicked(bool)),
                this, SLOT(selection_tryLeft_clicked(bool))
                );

    QObject::connect(
                m_ui->selection.setCenterToLeft, SIGNAL(clicked(bool)),
                this, SLOT(selection_setCenterToLeft_clicked(bool))
                );

    QObject::connect(
                m_ui->selection.tryCenter, SIGNAL(clicked(bool)),
                this, SLOT(selection_tryCenter_clicked(bool))
                );

    QObject::connect(
                m_ui->selection.rightValue, SIGNAL(valueChanged(double)),
                this, SLOT(selection_rightValue_changed(double))
                );

    QObject::connect(
                m_ui->selection.tryRight, SIGNAL(clicked(bool)),
                this, SLOT(selection_tryRight_clicked(bool))
                );

    QObject::connect(
                m_ui->selection.setCenterToRight, SIGNAL(clicked(bool)),
                this, SLOT(selection_setCenterToRight_clicked(bool))
                );

    QObject::connect(
                m_ui->pidK, SIGNAL(valuesChanged(double, double, double)),
                this, SLOT(pidK_changed(double, double, double))
                );

    QObject::connect(
                m_ui->tab.device->m_buttonReq2, SIGNAL(clicked(bool)),
                this, SLOT(P_button_rpc_request_2_koef_r(bool))
                );
    QObject::connect(
                m_ui->tab.device->m_buttonReq3, SIGNAL(clicked(bool)),
                this, SLOT(P_button_rpc_request_3_koef_w(bool))
                );
    QObject::connect(
                m_ui->tab.device->m_buttonReq4, SIGNAL(clicked(bool)),
                this, SLOT(P_button_rpc_request_4_speed_SP_r(bool))
                );
    QObject::connect(
                m_ui->tab.device->m_buttonReq5, SIGNAL(clicked(bool)),
                this, SLOT(P_button_rpc_request_5_speed_SP_w(bool))
                );
    QObject::connect(
                m_ui->tab.device->m_buttonReq6, SIGNAL(clicked(bool)),
                this, SLOT(P_button_rpc_request_6_speed_PV_r(bool))
                );
    QObject::connect(
                m_ui->tab.device->m_buttonReq7, SIGNAL(clicked(bool)),
                this, SLOT(P_button_rpc_request_7_process_start(bool))
                );
    QObject::connect(
                m_ui->tab.device->m_buttonReq8, SIGNAL(clicked(bool)),
                this, SLOT(P_button_rpc_request_8_process_stop(bool))
                );
    QObject::connect(
                m_ui->tab.device->m_buttonReq9, SIGNAL(clicked(bool)),
                this, SLOT(P_button_rpc_request_9_conf_store(bool))
                );

    QObject::connect(
                m_ui->tab.device->m_buttonClearBad, SIGNAL(clicked(bool)),
                this, SLOT(P_button_driveReqstat_clearBad(bool))
                );

    QObject::connect(
                m_ui->tab.device->m_buttonClearAll, SIGNAL(clicked(bool)),
                this, SLOT(P_button_driveReqstat_clearAll(bool))
                );

    QObject::connect(
                m_ui->tab.simulation->setpointMode_radio1, SIGNAL(toggled(bool)),
                this, SLOT(setpoint_ManualModeSelected(bool))
                );
    QObject::connect(
                m_ui->tab.simulation->setpointMode_radio2, SIGNAL(toggled(bool)),
                this, SLOT(setpoint_AutoModeSelected(bool))
                );

    QObject::connect(
                m_ui->selection.Kswitch.Kp, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Kp_select(bool))
                );
    QObject::connect(
                m_ui->selection.Kswitch.Ki, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Ki_select(bool))
                );
    QObject::connect(
                m_ui->selection.Kswitch.Kd, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Kd_select(bool))
                );

    QObject::connect(
                m_ui->tab.simulation->pausePushButton, SIGNAL(clicked(bool)),
                this, SLOT(pause_unpause(bool))
                );

    QObject::connect(
                m_ui->m_setpoint, SIGNAL(valueChanged(int)),
                this, SLOT(P_setpointValueChanged(int))
                );

    m_ui->mainMenu.actionRunMode->setChecked(true); /* to able to emit signal */
    m_ui->mainMenu.actionRunMode->setChecked(false);
    m_ui->mainMenu.actionAutoscale->setChecked(false);

    P_device_status_update(Device::RunMode::UNKNOWN);

    P_setpointFuncSetValue(MANUAL_SETPOINT_INITIAL_VALUE);
    m_ui->selection.leftValue->setValue(PIDK_SELECTION_VALUE_DEFAULT_MIN);
    m_ui->selection.rightValue->setValue(PIDK_SELECTION_VALUE_DEFAULT_MAX);
    m_ui->m_setpoint->setValue(MANUAL_SETPOINT_INITIAL_VALUE);
    m_ui->tab.simulation->setpointMode_radio1->setChecked(true);
    m_ui->selection.Kswitch.Kp->setChecked(true);
    m_ui->pidK->setValueKp(PID_Kp);
    m_ui->pidK->setValueKi(PID_Ki);
    m_ui->pidK->setValueKd(PID_Kd);

}

MainWindow::~MainWindow()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
    }

    delete m_device;
    delete m_comm;
    delete m_serial;
    delete m_settings;
    delete m_ui;
}

void MainWindow::P_openSerialPort()
{
    SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite))
    {
            m_ui->tab.device->m_console->setEnabled(true);
            m_ui->tab.device->m_console->setLocalEchoEnabled(p.localEchoEnabled);
            m_ui->mainMenu.actionConnect->setEnabled(false);
            m_ui->mainMenu.actionDisconnect->setEnabled(true);
            m_ui->mainMenu.actionConfigure->setEnabled(false);
            m_ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
            m_device->devConnect();

            if(m_runMode == RunMode::DEVICE)
            {
                P_runMode_device_plot_clear();
            }

    }
    else
    {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        m_ui->statusBar->showMessage(tr("Open error"));
    }
}

void MainWindow::P_closeSerialPort()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
    }
    m_device->devDisconnect();
    m_ui->tab.device->m_console->setEnabled(false);
    m_ui->mainMenu.actionConnect->setEnabled(true);
    m_ui->mainMenu.actionDisconnect->setEnabled(false);
    m_ui->mainMenu.actionConfigure->setEnabled(true);
    m_ui->statusBar->showMessage(tr("Disconnected"));
}

void MainWindow::P_device_reqstats_update()
{
    QMap<uint16_t, struct Device::req_status> stats = m_device->requestsStatGet();

}


void MainWindow::P_runMode_device_plot_clear()
{
    int len_max;
    len_max = 200;
    m_axis_x.sizeSet(len_max);
    m_valuesSetpoint.sizeSet(len_max);
    m_valuesPV.sizeSet(len_max);
    m_valuesOut.sizeSet(len_max);

    m_axis_x.clear();
    m_valuesSetpoint.clear();
    m_valuesPV.clear();
    m_valuesOut.clear();

}

void MainWindow::P_runMode_simulatin_plot_clear()
{
    int len_max;
    len_max = (AXIS_X_MAX - AXIS_X_MIN) * 1000 / INTERVAL_MS;

    m_axis_x.sizeSet(len_max);
    m_valuesSetpoint.sizeSet(len_max);
    m_valuesPV.sizeSet(len_max);
    m_valuesOut.sizeSet(len_max);

    m_axis_x.clear();
    m_valuesSetpoint.clear();
    m_valuesPV.clear();
    m_valuesOut.clear();

    m_plot_start = AXIS_X_MIN;
    m_plot_end = AXIS_X_MAX;
    m_simulatuion_value = AXIS_X_MIN;

}


void MainWindow::P_runModeChange(bool simulation)
{
    m_ui->tab.device->setEnabled(!simulation);
    m_ui->tab.simulation->setEnabled(simulation);
    m_axis_x.clear();
    m_valuesSetpoint.clear();
    m_valuesPV.clear();
    m_valuesOut.clear();
    m_processVariable = 0.0;

    int len_max;
    if(simulation)
    {
        m_runMode = RunMode::SIMULATION;
        P_runMode_simulatin_plot_clear();
        m_timer->start(INTERVAL_MS);
    }
    else
    {
        m_timer->stop();
        m_runMode = RunMode::DEVICE;
        P_runMode_device_plot_clear();
    }


    P_tickEventCommon();

}

void MainWindow::P_device_status_update(Device::RunMode mode) const
{
    bool connected = m_serial->isOpen();
    QString modeStr;
    if(!connected)
    {
        modeStr = QStringLiteral("Disconnected");
    }
    else
    {
        modeStr = P_device_mode_string_get(mode);
    }

    m_ui->tab.device->m_deviceStatusLabel->setText(modeStr);
}

void MainWindow::P_writeRawData(const QByteArray &data)
{
    m_serial->write(data);
}

void MainWindow::P_appendRawData()
{
    QByteArray data;
    while(m_serial->bytesAvailable() > 0)
    {
        data = m_serial->readAll();
        m_ui->tab.device->m_console->putData(data);
        m_comm->appendInputStream(data);
    }
}

void MainWindow::P_readFrame()
{
    QByteArray frame;
    while(m_comm->readFrame(frame))
    {
        m_device->dataReplyReceive(frame);
    }
}

void MainWindow::P_dev_ready_runModeChanged(Device::RunMode mode)
{
    P_device_reqstats_update();
    P_device_status_update(mode);
}

void MainWindow::P_dev_ready_SPPV(unsigned long time_ms, double sp, double pv, double out)
{

    m_processVariable = pv;
    m_ui->indication.PID_power->setText(QString("PID output = %1").arg(out));


    m_plot_start = AXIS_X_MIN;
    m_plot_end = AXIS_X_MAX;

    m_axis_x.append((double)time_ms / 1000.0);
    m_valuesSetpoint.append(sp);
    m_valuesPV.append(m_processVariable);
    m_valuesOut.append(out);

    const QVector <double> &axis_x_vals = m_axis_x.get();

    int size = axis_x_vals.size();
    double begin = axis_x_vals[0];
    double end   = axis_x_vals[size > 0 ? size - 1 : 0];

    m_time = time_ms;

    m_plot_start = begin;
    m_plot_end   = end;
    P_tickEventCommon();
}

void MainWindow::P_dev_ready_pulsesRead(bool timedout, unsigned err, unsigned ppr)
{
    if(timedout) return;
    if(err) return;
    m_ui->tab.device->m_devicePPR->setText(QString("%1").arg(ppr));
}

void MainWindow::P_dev_ready_runModeRead(bool timedout, unsigned err, Device::RunMode mode)
{
    if(timedout) return;
    if(err) return;
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    P_device_reqstats_update();
    P_device_status_update(timedout ? Device::RunMode::UNKNOWN : mode);
}

void MainWindow::P_dev_ready_pidKoefRead(bool timedout, unsigned err, double Kp, double Ki, double Kd)
{
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    if(timedout) return;
    if(err) return;
    m_ui->pidK->setValueKp(Kp);
    m_ui->pidK->setValueKi(Ki);
    m_ui->pidK->setValueKd(Kd);
}

void MainWindow::P_dev_ready_pidKoefWrite(bool timedout, unsigned err)
{
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    if(timedout) return;
    if(err) return;
}

void MainWindow::P_dev_ready_speedSetpointRead(bool timedout, unsigned err, double sp)
{
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    if(timedout) return;
    if(err) return;
}

void MainWindow::P_dev_ready_speedSetpointWrite(bool timedout, unsigned err)
{
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    if(timedout) return;
    if(err) return;
}

void MainWindow::P_dev_ready_speedPVRead(bool timedout, unsigned err, double pv)
{
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    if(timedout) return;
    if(err) return;
}

void MainWindow::P_dev_ready_processStart(bool timedout, unsigned err)
{
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    if(timedout) return;
    if(err) return;
}

void MainWindow::P_dev_ready_processStop(bool timedout, unsigned err)
{
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    if(timedout) return;
    if(err) return;
}

void MainWindow::P_dev_ready_confStored(bool timedout, unsigned err)
{
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
    if(timedout) return;
    if(err) return;
}

void MainWindow::P_button_rpc_request_2_koef_r(bool)
{
    m_device->pidKoefRead();
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_rpc_request_3_koef_w(bool)
{
    m_device->pidKoefWrite(
                m_ui->pidK->valueKp(),
                m_ui->pidK->valueKi(),
                m_ui->pidK->valueKd()
                );
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_rpc_request_4_speed_SP_r(bool)
{
    m_device->speedSetpointRead();
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_rpc_request_5_speed_SP_w(bool)
{
    try
    {
        m_device->speedSetpointWrite(33.0);
    }
    catch(...)
    {
        QMessageBox::critical(this, tr("Error"), QString("Device info not ready"));
        m_ui->statusBar->showMessage(tr("Device info not ready"));
    }

    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_rpc_request_6_speed_PV_r(bool)
{
    m_device->speedPVRead();
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_rpc_request_7_process_start(bool)
{
    m_device->request_07_process_start();
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_rpc_request_8_process_stop(bool)
{
    m_device->request_08_process_stop();
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_rpc_request_9_conf_store(bool)
{
    m_device->confStore();
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_driveReqstat_clearBad(bool)
{
    m_device->requestsStatClear(Device::ReqResult::TIMEOUT);
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::P_button_driveReqstat_clearAll(bool)
{
    m_device->requestsStatClearAll();
    m_ui->tab.device->m_reqstat_model->update(m_device->requestsStatGet());
}

void MainWindow::pidK_changed(double Kp, double Ki, double Kd)
{

#if defined(PID_DISCRETE)
    pid->KpSet(Kp_);
    pid->KiSet(Ki_);
    pid->KdSet(Kd_);
#elif defined(PID_RECURRENT)
    pid->KpSet(Kp_);
    pid->KiSet(Ki_);
    pid->KdSet(Kd_);
#elif defined(PID_RECURRENT_FIXED32)
    m_pid->KpSet(nostd::Fixed32(Kp));
    m_pid->KiSet(nostd::Fixed32(Ki));
    m_pid->KdSet(nostd::Fixed32(Kd));
#endif

    m_pid->reset();

}

void MainWindow::setpoint_ManualModeSelected(bool toggled)
{
    if(!toggled) return;
    m_setpoint_manual = true;
}

void MainWindow::setpoint_AutoModeSelected(bool toggled)
{
    if(!toggled) return;
    m_setpoint_manual = false;
}

void MainWindow::setpoint_valueSet(int value)
{
    m_ui->m_setpoint->setValue(value);
}

void MainWindow::P_setpointValueChanged(int value)
{
    switch(m_runMode)
    {
        case RunMode::DEVICE:
        {
            try
            {
                m_device->speedSetpointWrite(value);
            }
            catch(...)
            {
                m_ui->statusBar->showMessage(tr("Device info not ready"));
            }
            break;
        }
        case RunMode::SIMULATION:
        {
            P_setpointFuncSetValue(value);
            break;
        }
    }
}

void MainWindow::P_setpointFuncSetValue(int value)
{
    m_sim_setpoint = map_integer_to_double(
                value,
                MANUAL_SETPOINT_MIN,
                MANUAL_SETPOINT_MAX,
                MANUAL_SETPOINT_MIN,
                MANUAL_SETPOINT_MAX
                );

}

void MainWindow::selection_leftValue_changed(double value)
{
    double mid = (m_ui->selection.rightValue->value() + value) / 2;
    m_ui->selection.centerValue->setValue(mid);
}

void MainWindow::selection_rightValue_changed(double value)
{
    double mid = (value + m_ui->selection.leftValue->value()) / 2;
    m_ui->selection.centerValue->setValue(mid);
}

void MainWindow::selection_tryLeft_clicked(bool)
{
    double val = m_ui->selection.leftValue->value();
    switch(m_kselector)
    {
    case PID_K_Selector::Kp: m_ui->pidK->setValueKp(val); break;
    case PID_K_Selector::Ki: m_ui->pidK->setValueKi(val); break;
    case PID_K_Selector::Kd: m_ui->pidK->setValueKd(val); break;
    }
}

void MainWindow::selection_tryCenter_clicked(bool)
{
    double val = m_ui->selection.centerValue->value();
    switch(m_kselector)
    {
    case PID_K_Selector::Kp: m_ui->pidK->setValueKp(val); break;
    case PID_K_Selector::Ki: m_ui->pidK->setValueKi(val); break;
    case PID_K_Selector::Kd: m_ui->pidK->setValueKd(val); break;
    }
}

void MainWindow::selection_tryRight_clicked(bool)
{
    double val = m_ui->selection.rightValue->value();
    switch(m_kselector)
    {
    case PID_K_Selector::Kp: m_ui->pidK->setValueKp(val); break;
    case PID_K_Selector::Ki: m_ui->pidK->setValueKi(val); break;
    case PID_K_Selector::Kd: m_ui->pidK->setValueKd(val); break;
    }
}

void MainWindow::selection_setCenterToLeft_clicked(bool)
{
    m_ui->selection.leftValue->setValue(
                m_ui->selection.centerValue->value()
                );
}

void MainWindow::selection_setCenterToRight_clicked(bool)
{
    m_ui->selection.rightValue->setValue(
                m_ui->selection.centerValue->value()
                );
}

void MainWindow::Kselector_Kp_select(bool)
{
    m_kselector = PID_K_Selector::Kp;
}

void MainWindow::Kselector_Ki_select(bool)
{
    m_kselector = PID_K_Selector::Ki;
}

void MainWindow::Kselector_Kd_select(bool)
{
    m_kselector = PID_K_Selector::Kd;
}

void MainWindow::pause_unpause(bool)
{
    m_paused = !m_paused;
    m_ui->tab.simulation->pausePushButton->setText(tr(m_paused ? "UNPAUSE" : "PAUSE"));
}

double MainWindow::map_integer_to_double(
        int iv,
        int imin,
        int imax,
        double dmin,
        double dmax
        )
{
    return (iv * (dmax - dmin)) / (double)(imax - imin);
}


void MainWindow::P_tickEventSimulation()
{
    if(!m_paused)
    {

        if(!m_setpoint_manual)
        {
            int sp_auto = 50;
            setpoint_valueSet(sp_auto);
        }

#if defined(PID_DISCRETE)
        double power = pid->calculate(setpoint, processVariable);
#elif defined(PID_RECURRENT)
        double power = pid->calculate(setpoint, processVariable);
#elif defined(PID_RECURRENT_FIXED32)
        double power = m_pid->calculate(nostd::Fixed32(m_sim_setpoint), nostd::Fixed32(m_processVariable)).toDouble();
#endif

        m_processVariable = m_engine->process(power);
        m_ui->indication.PID_power->setText(QString("PID output = %1").arg(power));


        if(m_axis_x.isFull())
        {
            m_plot_start += (INTERVAL_MS / 1000.0);
            m_plot_end += (INTERVAL_MS / 1000.0);
        }

        m_axis_x.append(m_simulatuion_value);
        m_valuesSetpoint.append(m_sim_setpoint);
        m_valuesPV.append(m_processVariable);
        m_valuesOut.append(power);
        m_simulatuion_value += (INTERVAL_MS / 1000.0);
        m_time += INTERVAL_MS;

    }

    P_tickEventCommon();
}

void MainWindow::P_tickEventCommon()
{
    fan::FuncDescr fdescr;

    fan::FuncDescr::PointsDescr::const_iterator PV_index_min;
    fan::FuncDescr::PointsDescr::const_iterator PV_index_max;

    fan::functionAnalysis(m_valuesPV.get(), fdescr);
    const fan::FuncDescr::PointsDescr & pdescr = fdescr.pointsDescrGet();
    fan::functionExtremumsGet(pdescr, PV_index_min, PV_index_max);

    double PV_min = (PV_index_min != pdescr.constEnd() ? m_valuesPV.get()[PV_index_min.key()] : 0);
    double PV_max = (PV_index_max != pdescr.constEnd() ? m_valuesPV.get()[PV_index_max.key()] : 0);

    double PV_amplitude = PV_max - PV_min;

    const QVector<double> & vec_sp = m_valuesSetpoint.get();
    const QVector<double> & vec_pv = m_valuesPV.get();
    const QVector<double> & vec_out = m_valuesOut.get();

    double sp = (vec_sp.isEmpty() ? 0.0 : vec_sp.last());
    double pv = (vec_pv.isEmpty() ? 0.0 : vec_pv.last());

    P_indication_update(sp, pv, PV_amplitude);

    P_replot(
                m_axis_x.get(),
                vec_sp,
                vec_pv,
                vec_out
                );

}

void MainWindow::P_indication_update(double setpoint, double processVariable, double PV_amplitude)
{
    double sp_pv_diff = (double)((double)setpoint - processVariable);
    double sp_pv_diff_abs = fabs(sp_pv_diff);

    m_ui->indication.processVariable->setText(QString("process variable = %1").arg(processVariable));
    m_ui->indication.PV_amplitude->setText(QString("PV amplitude = %1").arg(PV_amplitude));
    m_ui->indication.diff_SP_PV->setText(QString("SP - PV = %1 (%2% SP) (SP %3 PV)")
                                         .arg(sp_pv_diff_abs, 0, 'g', 9)
                                         .arg(100.0 * (sp_pv_diff_abs / (double)setpoint) , 0, 'g', 2)
                                         .arg(
                                             (sp_pv_diff < 0.0) ? "<" :
                                                                  ((sp_pv_diff > 0.0) ? ">" : "=")
                                                                  )
                                         );
}

void MainWindow::P_replot(
        const QVector<double> &axis_x,
        const QVector<double> &vSetpoint,
        const QVector<double> &vPV,
        const QVector<double> &vOut
        )
{
    /* fill the axis-X values */

    m_ui->plotSetpoint->setData(axis_x, vSetpoint);
    m_ui->plotPV->setData(axis_x, vPV);
    m_ui->plotOut->setData(axis_x, vOut);

    /* scale setup */
    bool autoScale = (m_ui->mainMenu.actionAutoscale->isChecked());
    if(autoScale)
    {
        QVector<double> mins =
        {
            *std::min_element(vSetpoint.begin(), vSetpoint.end()),
            *std::min_element(vPV.begin(), vPV.end())
        };
        QVector<double> maxs =
        {
            *std::max_element(vSetpoint.begin(), vSetpoint.end()),
            *std::max_element(vPV.begin(), vPV.end())
        };
        double min = *std::min_element(mins.begin(), mins.end());
        double max = *std::max_element(maxs.begin(), maxs.end());
        double half = (max - min) / 2.0;
        double mid = (max + min) / 2.0;

        if(half == 0.0) half = 1;
        QCPRange range(mid - half * 1.3, mid + half * 1.3);
        m_ui->cplot->yAxis->setRange(range);
    }
    else
    {
        m_ui->cplot->yAxis->setRange(AXIS_Y_MIN, AXIS_Y_MAX);
    }
    m_ui->cplot->xAxis->setRange(m_plot_start, m_plot_end);

    m_ui->cplot->replot();

#ifdef APP_USE_CGRAPH
    plotSetpoint->updatePlotValues(valuesSetpoint);
    plotMain->updatePlotValues(valuesMain);
#endif
}

const QString MainWindow::P_device_mode_string_get(Device::RunMode mode) const
{
    static const QMap<Device::RunMode, QString> modes
    {
        { Device::RunMode::UNKNOWN, QStringLiteral("Unknown") },
        { Device::RunMode::NORMAL_STOPPED, QStringLiteral("Stopped") },
        { Device::RunMode::NORMAL_STARTED_AUTO, QStringLiteral("Started auto") },
        { Device::RunMode::NORMAL_STARTED_MANUAL, QStringLiteral("Started manual") },
        { Device::RunMode::SERVICE_MODE1_STOPPED, QStringLiteral("Service Mode #1: stopped") },
        { Device::RunMode::SERVICE_MODE1_STARTED, QStringLiteral("Service Mode #1: started") },
        { Device::RunMode::SERVICE_MODE2_STOPPED, QStringLiteral("Service Mode #2: stopped") },
        { Device::RunMode::SERVICE_MODE2_STARTED, QStringLiteral("Service Mode #2: started") },
        { Device::RunMode::SERVICE_MODE3_STOPPED, QStringLiteral("Service Mode #3: stopped") },
        { Device::RunMode::SERVICE_MODE3_STARTED, QStringLiteral("Service Mode #3: started") },
    };
    return modes[mode];
}
