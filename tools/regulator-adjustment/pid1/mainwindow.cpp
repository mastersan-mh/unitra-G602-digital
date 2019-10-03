
#include "defs.h"

#include "mainwindow.h"

#include "nostd/fixed/fixed32"
#include "nostd/fixed/fixed32.h"

#include "ctestmotor.h"
#include "pid/fan.h"
#include "pid/pidadjustp.h"

#include <QtMath>

#include <algorithm>

Parser::Parser()
    : m_str()
{
    m_str.reserve(32);
}

Parser::~Parser()
{
}

void Parser::reset()
{
    m_str.clear();
}

bool Parser::P_string_append(QByteArray::const_iterator &from, const QByteArray::const_iterator to, QString &str)
{
    while(from != to)
    {
        char ch = (*from);
	++from;
	if(ch == 10 || ch == 13)
        {
            return true;
        }
        str.append(ch);
    }
    return false;
}


void Parser::dataRead(const QByteArray &data)
{
    QByteArray::const_iterator x = data.constBegin();
    do
    {
        bool string_end = P_string_append(x, data.constEnd(), m_str);
        if(string_end)
        {
            emit stringReady(m_str);
            m_str.clear();
        }
    }while(x != data.constEnd());
}

/*
http://www.controlplast.ru/site/index.php?/rinforms/kdocuments/nastroykapid
https://www.bookasutp.ru/Chapter5_5.aspx
http://lazysmart.ru/osnovy-avtomatiki/nastrojka-pid-regulyatora/
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new MainWindow_Ui)
{
    m_ui->init_UI(this);

    m_serial = new QSerialPort(this);
    m_parser = new Parser();
    m_settings = new SettingsDialog;

    m_engine = new CTestMotor(0.1);

    m_ui->mainMenu.actionConnect->setEnabled(true);
    m_ui->mainMenu.actionDisconnect->setEnabled(false);
    m_ui->mainMenu.actionQuit->setEnabled(true);
    m_ui->mainMenu.actionConfigure->setEnabled(true);

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
    connect(m_timer, SIGNAL(timeout()), this, SLOT(mainEvent()));
    m_timer->start(INTERVAL_MS);

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

    m_axis_x_shift = 0.0;

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

    m_ui->cplot->xAxis->setBasePen(axisPen);
    m_ui->cplot->xAxis->setLabel("time");
    m_ui->cplot->xAxis->setLabelColor(axisColor);
    m_ui->cplot->xAxis->setTickLabelColor(axisColor);
    m_ui->cplot->xAxis->setTickPen(axisPen);
    m_ui->cplot->xAxis->setSubTickPen(axisPen);
    m_ui->cplot->xAxis->setRange(AXIS_X_MIN, AXIS_X_MAX);

    m_ui->plotSetpoint = m_ui->cplot->addGraph();
    m_ui->plotSetpoint->setPen(QPen(QColor(Qt::red)));

    m_ui->plotPV = m_ui->cplot->addGraph();
    m_ui->plotPV->setPen(QPen(QColor(Qt::white)));

    QObject::connect(m_ui->mainMenu.actionConnect   , SIGNAL(triggered()), this, SLOT(P_openSerialPort()));
    QObject::connect(m_ui->mainMenu.actionDisconnect, SIGNAL(triggered()), this, SLOT(P_closeSerialPort()));
    QObject::connect(m_ui->mainMenu.actionQuit      , SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(m_ui->mainMenu.actionConfigure , SIGNAL(triggered()), m_settings, SLOT(show()));
    //QObject::connect(m_ui->mainMenu.actionClear     , SIGNAL(triggered()), console , SLOT(clear()));

    QObject::connect(m_serial, SIGNAL(readyRead()), this, SLOT(P_readRawData()));
    QObject::connect(m_ui->tab.serial->console, SIGNAL(getData(QByteArray)), this, SLOT(P_writeRawData(QByteArray)));
    QObject::connect(m_parser, SIGNAL(stringReady(QString)), this, SLOT(P_readParcedData(QString)));

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
                m_ui->tab.simulation->setpointMode_radio1, SIGNAL(toggled(bool)),
                this, SLOT(setpoint_ManualModeSelected(bool))
                );
    QObject::connect(
                m_ui->tab.simulation->setpointMode_radio2, SIGNAL(toggled(bool)),
                this, SLOT(setpoint_AutoModeSelected(bool))
                );

    QObject::connect(
                m_ui->Kselector_radio1_Kp, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Kp_select(bool))
                );
    QObject::connect(
                m_ui->Kselector_radio2_Ki, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Ki_select(bool))
                );
    QObject::connect(
                m_ui->Kselector_radio3_Kd, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Kd_select(bool))
                );

    QObject::connect(
                m_ui->tab.simulation->pausePushButton, SIGNAL(clicked(bool)),
                this, SLOT(pause_unpause(bool))
                );

    QObject::connect(
                m_ui->setpoint_slider, SIGNAL(valueChanged(int)),
                this, SLOT(setpoint_sliderChangeValue(int))
                );
    QObject::connect(
                m_ui->setpoint_spinBox, SIGNAL(valueChanged(int)),
                this, SLOT(setpoint_spinBoxChangeValue(int))
                );

    setpointFuncSetValue(MANUAL_SETPOINT_INITIAL_VALUE);
    m_ui->selection.leftValue->setValue(PIDK_SELECTION_RANGE_MIN);
    m_ui->selection.rightValue->setValue(PIDK_SELECTION_RANGE_MAX);
    m_ui->setpoint_slider->setValue(MANUAL_SETPOINT_INITIAL_VALUE);
    m_ui->setpoint_spinBox->setValue(MANUAL_SETPOINT_INITIAL_VALUE);
    m_ui->tab.simulation->setpointMode_radio1->setChecked(true);
    m_ui->Kselector_radio1_Kp->setChecked(true);
    m_ui->pidK->setValueKp(PID_Kp);
    m_ui->pidK->setValueKi(PID_Ki);
    m_ui->pidK->setValueKd(PID_Kd);

}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

//! [4]
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
            m_ui->tab.serial->console->setEnabled(true);
            m_ui->tab.serial->console->setLocalEchoEnabled(p.localEchoEnabled);
            m_ui->mainMenu.actionConnect->setEnabled(false);
            m_ui->mainMenu.actionDisconnect->setEnabled(true);
            m_ui->mainMenu.actionConfigure->setEnabled(false);
            m_ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
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
    m_ui->tab.serial->console->setEnabled(false);
    m_ui->mainMenu.actionConnect->setEnabled(true);
    m_ui->mainMenu.actionDisconnect->setEnabled(false);
    m_ui->mainMenu.actionConfigure->setEnabled(true);
    m_ui->statusBar->showMessage(tr("Disconnected"));
}

void MainWindow::P_writeRawData(const QByteArray &data)
{
    m_serial->write(data);
}

void MainWindow::P_readRawData()
{
    QByteArray data = m_serial->readAll();
    m_ui->tab.serial->console->putData(data);

    m_parser->dataRead(data);

}

void MainWindow::P_readParcedData(const QString & in)
{
	QString str(in.trimmed());
	if(str.length() == 0)
	{
		return;
	}
	QStringList list = str.split(QStringLiteral(" "));
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
    m_ui->setpoint_slider->setValue(value);
}

void MainWindow::setpoint_sliderChangeValue(int value)
{
    m_ui->setpoint_spinBox->setValue(value);
    setpointFuncSetValue(value);
}

void MainWindow::setpoint_spinBoxChangeValue(int value)
{
    m_ui->setpoint_slider->setValue(value);
    setpointFuncSetValue(value);
}

void MainWindow::setpointFuncSetValue(int value_)
{
    m_setpoint = map_integer_to_double(
                value_,
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


void MainWindow::mainEvent()
{
    fan::FuncDescr fdescr;

    int len_max = (AXIS_X_MAX - AXIS_X_MIN) * 1000 / INTERVAL_MS;

    bool autoScale = (m_ui->cplotAutoScale->checkState() != Qt::Unchecked);

    if(!m_setpoint_manual)
    {
        int sp_auto = 50;
        setpoint_valueSet(sp_auto);
    }

    if(!m_paused)
    {

#if defined(PID_DISCRETE)
        double power = pid->calculate(setpoint, processVariable);
#elif defined(PID_RECURRENT)
        double power = pid->calculate(setpoint, processVariable);
#elif defined(PID_RECURRENT_FIXED32)
        double power = m_pid->calculate(nostd::Fixed32(m_setpoint), nostd::Fixed32(m_processVariable)).toDouble();
#endif

        m_processVariable = m_engine->process(power);

        m_valuesSetpoint.push_back(m_setpoint);
        m_valuesPV.push_back(m_processVariable);
        m_time += INTERVAL_MS;

        if(m_valuesSetpoint.size() > len_max)
        {
            /* erase old values */
            m_valuesSetpoint.pop_front();
            m_valuesPV.pop_front();

            /* shift the axis-X */
            m_axis_x_shift += (INTERVAL_MS / 1000.0);
        }

        fan::FuncDescr::PointsDescr::const_iterator PV_index_min;
        fan::FuncDescr::PointsDescr::const_iterator PV_index_max;

        fan::functionAnalysis(m_valuesPV, fdescr);
        const fan::FuncDescr::PointsDescr & pdescr = fdescr.pointsDescrGet();
        fan::functionExtremumsGet(pdescr, PV_index_min, PV_index_max);

        double PV_min = (PV_index_min != pdescr.constEnd() ? m_valuesPV[PV_index_min.key()] : 0);
        double PV_max = (PV_index_max != pdescr.constEnd() ? m_valuesPV[PV_index_max.key()] : 0);

        double PV_amplitude = PV_max - PV_min;

        double sp_pv_diff = (double)((double)m_setpoint - m_processVariable);
        double sp_pv_diff_abs = fabs(sp_pv_diff);
        m_ui->indication.processVariable->setText(QString("process variable = %1").arg(m_processVariable));
        m_ui->indication.PV_amplitude->setText(QString("PV amplitude = %1").arg(PV_amplitude));
        m_ui->indication.diff_SP_PV->setText(QString("SP - PV = %1 (%2% SP) (SP %3 PV)")
                                     .arg(sp_pv_diff_abs, 0, 'g', 9)
                                     .arg(100.0 * (sp_pv_diff_abs / (double)m_setpoint) , 0, 'g', 2)
                                     .arg(
                                         (sp_pv_diff < 0.0) ? "<" :
                                                            (sp_pv_diff > 0.0) ? ">" :
                                                                               "="
                                                                               )
                                     );
        m_ui->indication.PID_power->setText(QString("PID output = %1").arg(power));

    }



    /* fill the axis-X values */
    QVector<double> axis_x(len_max);

    double idx = 0.0;
    for (int i = 0; i < len_max; ++i)
    {
        axis_x[i] = m_axis_x_shift + idx;
        idx += (INTERVAL_MS / 1000.0);
    }

    QVector<double> mins =
    {
        *std::min_element(m_valuesSetpoint.begin(), m_valuesSetpoint.end()),
        *std::min_element(m_valuesPV.begin(), m_valuesPV.end())
    };
    QVector<double> maxs =
    {
        *std::max_element(m_valuesSetpoint.begin(), m_valuesSetpoint.end()),
        *std::max_element(m_valuesPV.begin(), m_valuesPV.end())
    };
    double min = *std::min_element(mins.begin(), mins.end());
    double max = *std::max_element(maxs.begin(), maxs.end());

    m_ui->plotSetpoint->setData(axis_x, m_valuesSetpoint);
    m_ui->plotPV->setData(axis_x, m_valuesPV);

    /* scale setup */
    if(autoScale)
    {
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
    m_ui->cplot->xAxis->setRange(AXIS_X_MIN + m_axis_x_shift, AXIS_X_MAX + m_axis_x_shift);

    m_ui->cplot->replot();

#ifdef APP_USE_CGRAPH
    plotSetpoint->updatePlotValues(valuesSetpoint);
    plotMain->updatePlotValues(valuesMain);
#endif
}
