#include "mainwindow.h"

#include "nostd/fixed/fixed32"
#include "nostd/fixed/fixed32.h"

#include <QtMath>

#include <algorithm>


#define INTERVAL_MS 100

#if defined(PID_RECURRENT_FIXED32)
#   define MANUAL_SETPOINT_MAX (255)
#   define MANUAL_SETPOINT_MIN (0)
#   define MANUAL_SETPOINT_INITIAL_VALUE (0)
#   define AXIS_Y_MIN (MANUAL_SETPOINT_MAX)
#   define AXIS_Y_MAX (MANUAL_SETPOINT_MIN)
#else
#   define MANUAL_SETPOINT_MAX (255)
#   define MANUAL_SETPOINT_MIN (0)
#   define MANUAL_SETPOINT_INITIAL_VALUE (0)
#   define AXIS_Y_MIN (MANUAL_SETPOINT_MAX)
#   define AXIS_Y_MAX (MANUAL_SETPOINT_MIN)
#endif


#define ENGINE_POWER_MIN (0.0)
#define ENGINE_POWER_MAX (1000.0)

/* seconds */
#define AXIS_X_MIN (0)
#define AXIS_X_MAX (15)

#define UI_LEVEL_BEGIN(xlayout) do
#define UI_LEVEL_END(xlayout) while(0)

#if defined(PID_DISCRETE)

#   define PIDK_SELECTION_MIN (0.0)
#   define PIDK_SELECTION_MAX (100.0)
#   define PIDK_SELECTION_RANGE_MIN (0.0)
#   define PIDK_SELECTION_RANGE_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.00488281)
#   define PID_Kd (0.07934570)

#elif defined(PID_RECURRENT)

#   define PIDK_SELECTION_MIN (0.0)
#   define PIDK_SELECTION_MAX (100.0)
#   define PIDK_SELECTION_RANGE_MIN (0.0)
#   define PIDK_SELECTION_RANGE_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.00488281)
#   define PID_Kd (0.07934570)

#elif defined(PID_RECURRENT_FIXED32)

#   define PIDK_SELECTION_MIN (0.0)
#   define PIDK_SELECTION_MAX (100.0)
#   define PIDK_SELECTION_RANGE_MIN (0.0)
#   define PIDK_SELECTION_RANGE_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.01464844)
#   define PID_Kd (0.11718750)

#endif




void CPIDK::init()
{
    mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);

    QLabel * Kp_label = new QLabel(tr("Kp:"), this);
    QLabel * Ki_label = new QLabel(tr("Ki:"), this);
    QLabel * Kd_label = new QLabel(tr("Kd:"), this);

    mainLayout->addWidget(Kp_label);
    Kp_spinBox = new QDoubleSpinBox(this);

    mainLayout->addWidget(Kp_spinBox);
    QObject::connect(
                Kp_spinBox, SIGNAL(valueChanged(double)),
                this, SLOT(K_changed(double))
                );

    mainLayout->addWidget(Ki_label);

    Ki_spinBox = new QDoubleSpinBox(this);
    mainLayout->addWidget(Ki_spinBox);
    QObject::connect(
                Ki_spinBox, SIGNAL(valueChanged(double)),
                this, SLOT(K_changed(double))
                );

    mainLayout->addWidget(Kd_label);

    Kd_spinBox = new QDoubleSpinBox(this);
    mainLayout->addWidget(Kd_spinBox);
    QObject::connect(
                Kd_spinBox, SIGNAL(valueChanged(double)),
                this, SLOT(K_changed(double))
                );



    Kp_spinBox->setDecimals(8);
    Kp_spinBox->setMinimum(PIDK_SELECTION_MIN);
    Kp_spinBox->setMaximum(PIDK_SELECTION_MAX);

    Ki_spinBox->setDecimals(8);
    Ki_spinBox->setMinimum(PIDK_SELECTION_MIN);
    Ki_spinBox->setMaximum(PIDK_SELECTION_MAX);

    Kd_spinBox->setDecimals(8);
    Kd_spinBox->setMinimum(PIDK_SELECTION_MIN);
    Kd_spinBox->setMaximum(PIDK_SELECTION_MAX);

    mainLayout->addStretch();
}

CPIDK::CPIDK(QWidget* parent)
    :QGroupBox(parent)
{
    init();
}

CPIDK::CPIDK(const QString &title, QWidget* parent)
    :QGroupBox(title, parent)
{
    init();
}

CPIDK::~CPIDK(){}

void CPIDK::K_changed(double)
{
    emit valuesChanged(
                Kp_spinBox->value(),
                Ki_spinBox->value(),
                Kd_spinBox->value()
                );
}

void MainWindow::init_UI()
{
    QSizePolicy::Policy sp = QSizePolicy::Maximum;

    m_ui.central = new QWidget(this);
    setCentralWidget(m_ui.central);

    m_ui.centralWLayout = new QHBoxLayout(m_ui.central);
    m_ui.central->setLayout(m_ui.centralWLayout);
    UI_LEVEL_BEGIN(m_ui.central)
    {
        m_ui.ctrl1_Layout = new QVBoxLayout(m_ui.central);
        m_ui.centralWLayout->addLayout(m_ui.ctrl1_Layout);
        UI_LEVEL_BEGIN(m_ui.ctrl1_Layout)
        {
            m_ui.selection.main = new QGroupBox("K select", m_ui.central);
            m_ui.ctrl1_Layout->addWidget(m_ui.selection.main);
            UI_LEVEL_BEGIN(m_ui.selection.mainLayout)
            {

                /*
                  LLLL          CCCC            RRRR
                  <try-left>             <try-right>
                  <left-to-center> <right-to-center>
                */

                m_ui.selection.mainLayout = new QHBoxLayout(m_ui.selection.main);
                m_ui.selection.main->setLayout(m_ui.selection.mainLayout);

                m_ui.selection.leftLayout = new QVBoxLayout(m_ui.selection.main);
                m_ui.selection.mainLayout->addLayout(m_ui.selection.leftLayout);
                UI_LEVEL_BEGIN(m_ui.selection.leftLayout)
                {
                    m_ui.selection.leftValue = new QDoubleSpinBox(m_ui.selection.main);
                    m_ui.selection.leftValue->setMinimum(PIDK_SELECTION_MIN);
                    m_ui.selection.leftValue->setMaximum(PIDK_SELECTION_MAX);
                    m_ui.selection.leftLayout->addWidget(m_ui.selection.leftValue);

                    m_ui.selection.tryLeft = new QPushButton("Try", m_ui.selection.main);
                    m_ui.selection.leftLayout->addWidget(m_ui.selection.tryLeft);

                    m_ui.selection.setCenterToLeft = new QPushButton("Set <", m_ui.selection.main);
                    m_ui.selection.leftLayout->addWidget(m_ui.selection.setCenterToLeft);

                    m_ui.selection.leftLayout->addStretch();

                    QObject::connect(
                                m_ui.selection.leftValue, SIGNAL(valueChanged(double)),
                                this, SLOT(selection_leftValue_changed(double))
                                );

                    QObject::connect(
                                m_ui.selection.tryLeft, SIGNAL(clicked(bool)),
                                this, SLOT(selection_tryLeft_clicked(bool))
                                );

                    QObject::connect(
                                m_ui.selection.setCenterToLeft, SIGNAL(clicked(bool)),
                                this, SLOT(selection_setCenterToLeft_clicked(bool))
                                );

                    m_ui.selection.leftValue->setDecimals(9);
                    m_ui.selection.leftValue->setValue(PIDK_SELECTION_RANGE_MIN);

                } UI_LEVEL_END(m_ui.selection.leftLayout);

                m_ui.selection.centerLayout = new QVBoxLayout(m_ui.selection.main);
                m_ui.selection.mainLayout->addLayout(m_ui.selection.centerLayout);
                UI_LEVEL_BEGIN(m_ui.selection.centerLayout)
                {
                    m_ui.selection.centerValue = new QDoubleSpinBox(m_ui.selection.main);
                    m_ui.selection.centerValue->setMinimum(PIDK_SELECTION_MIN);
                    m_ui.selection.centerValue->setMaximum(PIDK_SELECTION_MAX);
                    m_ui.selection.centerLayout->addWidget(m_ui.selection.centerValue);

                    m_ui.selection.tryCenter = new QPushButton("Try", m_ui.selection.main);
                    m_ui.selection.centerLayout->addWidget(m_ui.selection.tryCenter);

                    m_ui.selection.centerLayout->addStretch();

                    QObject::connect(
                                m_ui.selection.tryCenter, SIGNAL(clicked(bool)),
                                this, SLOT(selection_tryCenter_clicked(bool))
                                );

                    m_ui.selection.centerValue->setDecimals(9);

                } UI_LEVEL_END(m_ui.selection.centerLayout);

                m_ui.selection.rightLayout = new QVBoxLayout(m_ui.selection.main);
                m_ui.selection.mainLayout->addLayout(m_ui.selection.rightLayout);
                UI_LEVEL_BEGIN(m_ui.selection.rightLayout)
                {
                    m_ui.selection.rightValue = new QDoubleSpinBox(m_ui.selection.main);
                    m_ui.selection.rightValue->setMinimum(PIDK_SELECTION_MIN);
                    m_ui.selection.rightValue->setMaximum(PIDK_SELECTION_MAX);
                    m_ui.selection.rightLayout->addWidget(m_ui.selection.rightValue);

                    m_ui.selection.tryRight = new QPushButton("Try", m_ui.selection.main);
                    m_ui.selection.rightLayout->addWidget(m_ui.selection.tryRight);

                    m_ui.selection.setCenterToRight = new QPushButton("> Set", m_ui.selection.main);
                    m_ui.selection.rightLayout->addWidget(m_ui.selection.setCenterToRight);

                    m_ui.selection.rightLayout->addStretch();

                    QObject::connect(
                                m_ui.selection.rightValue, SIGNAL(valueChanged(double)),
                                this, SLOT(selection_rightValue_changed(double))
                                );

                    QObject::connect(
                                m_ui.selection.tryRight, SIGNAL(clicked(bool)),
                                this, SLOT(selection_tryRight_clicked(bool))
                                );

                    QObject::connect(
                                m_ui.selection.setCenterToRight, SIGNAL(clicked(bool)),
                                this, SLOT(selection_setCenterToRight_clicked(bool))
                                );

                    m_ui.selection.rightValue->setDecimals(9);
                    m_ui.selection.rightValue->setValue(PIDK_SELECTION_RANGE_MAX);

                } UI_LEVEL_END(m_ui.selection.rightLayout);

                m_ui.selection.main->setSizePolicy(sp, sp);

            } UI_LEVEL_END(m_ui.selection.mainLayout);

            /* pid */

            m_ui.pidK_Kselector_Layout = new QHBoxLayout();
            m_ui.ctrl1_Layout->addLayout(m_ui.pidK_Kselector_Layout);

            UI_LEVEL_BEGIN(m_ui.pidK_Kselector_Layout)
            {

#define PIDK_MAXIMUM_WIDTH (28 + 8 + 10 * 12)
                m_ui.pidK = new CPIDK("PID", m_ui.central);
                m_ui.pidK->setMaximumWidth(PIDK_MAXIMUM_WIDTH);
                m_ui.pidK_Kselector_Layout->addWidget(m_ui.pidK);

                QObject::connect(
                            m_ui.pidK, SIGNAL(valuesChanged(double, double, double)),
                            this, SLOT(pidK_changed(double, double, double))
                            );

                m_ui.pidK->setValueKp(PID_Kp);
                m_ui.pidK->setValueKi(PID_Ki);
                m_ui.pidK->setValueKd(PID_Kd);

                m_ui.Kselector = init_UI_Kselector(m_ui.central);
                m_ui.pidK_Kselector_Layout->addWidget(m_ui.Kselector);
                m_ui.Kselector->setSizePolicy(sp, QSizePolicy::Preferred);

            } UI_LEVEL_END(m_ui.pidK_Kselector_Layout);

            m_ui.processVariable_indicator = new QLabel(m_ui.central);
            m_ui.ctrl1_Layout->addWidget(m_ui.processVariable_indicator);
            m_ui.processVariable_indicator->setText(QString("process variable = %1").arg(m_processVariable));

            m_ui.SPminusPV_indicator = new QLabel(m_ui.central);
            m_ui.ctrl1_Layout->addWidget(m_ui.SPminusPV_indicator);
            m_ui.SPminusPV_indicator->setText(QString("SP - PV = %1").arg(0));

            m_ui.output_indicator = new QLabel(m_ui.central);
            m_ui.ctrl1_Layout->addWidget(m_ui.output_indicator);
            m_ui.output_indicator->setText(QString("PID output = %1").arg(0));

            m_ui.pausePushButton = new QPushButton(m_ui.central);
            m_ui.ctrl1_Layout->addWidget(m_ui.pausePushButton);
            m_ui.pausePushButton->setSizePolicy(sp, QSizePolicy::Preferred);

            m_ui.pausePushButton->setText(tr("PAUSE"));
            QObject::connect(
                        m_ui.pausePushButton, SIGNAL(clicked(bool)),
                        this, SLOT(pause_unpause(bool))
                        );

            QGroupBox *setpointMode = init_UI_pidSetup(m_ui.central);

            m_ui.ctrl1_Layout->addWidget(setpointMode);

            /* Stretch */
            m_ui.ctrl1_Layout->addStretch();

        } UI_LEVEL_END(m_ui.ctrl1_Layout);

        /* setpoint */
        m_ui.setpoint_Layout = new QVBoxLayout(m_ui.central);
        m_ui.centralWLayout->addLayout(m_ui.setpoint_Layout);
        UI_LEVEL_BEGIN(m_ui.setpoint_Layout)
        {
            m_ui.setpoint_slider = new QSlider(Qt::Vertical, m_ui.central);
            m_ui.setpoint_Layout->addWidget(m_ui.setpoint_slider);
            m_ui.setpoint_slider->setMinimum(MANUAL_SETPOINT_MIN);
            m_ui.setpoint_slider->setMaximum(MANUAL_SETPOINT_MAX);
            m_ui.setpoint_slider->setValue(MANUAL_SETPOINT_INITIAL_VALUE);
            QObject::connect(
                        m_ui.setpoint_slider, SIGNAL(valueChanged(int)),
                        this, SLOT(setpoint_sliderChangeValue(int))
                        );
            m_ui.setpoint_spinBox = new QSpinBox(m_ui.central);
            m_ui.setpoint_Layout->addWidget(m_ui.setpoint_spinBox);
            m_ui.setpoint_spinBox->setMinimum(MANUAL_SETPOINT_MIN);
            m_ui.setpoint_spinBox->setMaximum(MANUAL_SETPOINT_MAX);
            m_ui.setpoint_spinBox->setValue(MANUAL_SETPOINT_INITIAL_VALUE);
            m_ui.setpoint_spinBox->setMaximumWidth(12*10);
            QObject::connect(
                        m_ui.setpoint_spinBox, SIGNAL(valueChanged(int)),
                        this, SLOT(setpoint_spinBoxChangeValue(int))
                        );
            setpointFuncSetValue(MANUAL_SETPOINT_INITIAL_VALUE);
        } UI_LEVEL_END(setpoint_Layout);

        /* cplot ctrl */
        m_ui.cplotCtrl_Layout = new QVBoxLayout(m_ui.central);
        m_ui.centralWLayout->addLayout(m_ui.cplotCtrl_Layout);
        UI_LEVEL_BEGIN(m_ui.cplotCtrl_Layout)
        {
            m_ui.cplotAutoScale = new QCheckBox(tr("autoscale"), m_ui.central);
            m_ui.cplotCtrl_Layout->addWidget(m_ui.cplotAutoScale);
            m_ui.cplotAutoScale->setMaximumWidth(8*15);
            m_ui.cplotCtrl_Layout->addStretch();

        } UI_LEVEL_END(m_ui.cplotCtrl_Layout);

        /* cplot widget */
        m_ui.cplot = new QCustomPlot(m_ui.central);
        m_ui.centralWLayout->addWidget(m_ui.cplot);

    } UI_LEVEL_END(m_central);

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    m_engine = new CTestMotor(0.1);

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

    init_UI();

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

    m_ui.cplot->setBackground(QBrush(QColor(Qt::black)));

    m_ui.cplot->yAxis->setLabelColor(axisColor);
    m_ui.cplot->yAxis->setBasePen(axisPen);
    m_ui.cplot->yAxis->setTickLabelColor(axisColor);
    m_ui.cplot->yAxis->setTickPen(axisPen);
    m_ui.cplot->yAxis->setSubTickPen(axisPen);
    m_ui.cplot->yAxis->setRange(AXIS_Y_MIN, AXIS_Y_MAX);

    m_ui.cplot->xAxis->setBasePen(axisPen);
    m_ui.cplot->xAxis->setLabel("time");
    m_ui.cplot->xAxis->setLabelColor(axisColor);
    m_ui.cplot->xAxis->setTickLabelColor(axisColor);
    m_ui.cplot->xAxis->setTickPen(axisPen);
    m_ui.cplot->xAxis->setSubTickPen(axisPen);
    m_ui.cplot->xAxis->setRange(AXIS_X_MIN, AXIS_X_MAX);

    m_accumulated_values = 0;

    m_ui.plotSetpoint = m_ui.cplot->addGraph();
    m_ui.plotSetpoint->setPen(QPen(QColor(Qt::red)));

    m_ui.plotPV = m_ui.cplot->addGraph();
    m_ui.plotPV->setPen(QPen(QColor(Qt::white)));

}

MainWindow::~MainWindow()
{

}

QGroupBox *MainWindow::init_UI_pidSetup(QWidget * parent)
{
    QGroupBox *groupBox = new QGroupBox(tr("Setpoint"), parent);

    QRadioButton *radio1 = new QRadioButton(tr("&Manual"), parent);
    QRadioButton *radio2 = new QRadioButton(tr("&Auto"), parent);


    QObject::connect(
                radio1, SIGNAL(toggled(bool)),
                this, SLOT(setpoint_ManualModeSelected(bool))
                );
    QObject::connect(
                radio2, SIGNAL(toggled(bool)),
                this, SLOT(setpoint_AutoModeSelected(bool))
                );

    radio1->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout(parent);
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    groupBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    return groupBox;
}

QGroupBox *MainWindow::init_UI_Kselector(QWidget * parent)
{
    QGroupBox *groupBox = new QGroupBox(tr("Kselector"), parent);

    QRadioButton *radio1 = new QRadioButton(tr("K&p"), parent);
    QRadioButton *radio2 = new QRadioButton(tr("K&i"), parent);
    QRadioButton *radio3 = new QRadioButton(tr("K&d"), parent);

    QObject::connect(
                radio1, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Kp_select(bool))
                );
    QObject::connect(
                radio2, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Ki_select(bool))
                );
    QObject::connect(
                radio3, SIGNAL(toggled(bool)),
                this, SLOT(Kselector_Kd_select(bool))
                );

    radio1->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout(parent);
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addWidget(radio3);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
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
    m_ui.setpoint_slider->setValue(value);
}

void MainWindow::setpoint_sliderChangeValue(int value)
{
    m_ui.setpoint_spinBox->setValue(value);
    setpointFuncSetValue(value);
}

void MainWindow::setpoint_spinBoxChangeValue(int value)
{
    m_ui.setpoint_slider->setValue(value);
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
    double mid = (m_ui.selection.rightValue->value() + value) / 2;
    m_ui.selection.centerValue->setValue(mid);
}

void MainWindow::selection_rightValue_changed(double value)
{
    double mid = (value + m_ui.selection.leftValue->value()) / 2;
    m_ui.selection.centerValue->setValue(mid);
}

void MainWindow::selection_tryLeft_clicked(bool)
{
    double val = m_ui.selection.leftValue->value();
    switch(m_kselector)
    {
    case PID_K_Selector::Kp: m_ui.pidK->setValueKp(val); break;
    case PID_K_Selector::Ki: m_ui.pidK->setValueKi(val); break;
    case PID_K_Selector::Kd: m_ui.pidK->setValueKd(val); break;
    }
}

void MainWindow::selection_tryCenter_clicked(bool)
{
    double val = m_ui.selection.centerValue->value();
    switch(m_kselector)
    {
    case PID_K_Selector::Kp: m_ui.pidK->setValueKp(val); break;
    case PID_K_Selector::Ki: m_ui.pidK->setValueKi(val); break;
    case PID_K_Selector::Kd: m_ui.pidK->setValueKd(val); break;
    }
}

void MainWindow::selection_tryRight_clicked(bool)
{
    double val = m_ui.selection.rightValue->value();
    switch(m_kselector)
    {
    case PID_K_Selector::Kp: m_ui.pidK->setValueKp(val); break;
    case PID_K_Selector::Ki: m_ui.pidK->setValueKi(val); break;
    case PID_K_Selector::Kd: m_ui.pidK->setValueKd(val); break;
    }
}

void MainWindow::selection_setCenterToLeft_clicked(bool)
{
    m_ui.selection.leftValue->setValue(
                m_ui.selection.centerValue->value()
                );
}

void MainWindow::selection_setCenterToRight_clicked(bool)
{
    m_ui.selection.rightValue->setValue(
                m_ui.selection.centerValue->value()
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
    m_ui.pausePushButton->setText(tr(m_paused ? "UNPAUSE" : "PAUSE"));
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
    int len_max = (AXIS_X_MAX - AXIS_X_MIN) * 1000 / INTERVAL_MS;

    bool autoScale = (m_ui.cplotAutoScale->checkState() != Qt::Unchecked);

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

        ++m_accumulated_values;
        m_valuesSetpoint.push_back(m_setpoint);
        m_valuesPV.push_back(m_processVariable);
        m_time += INTERVAL_MS;

        if(m_accumulated_values > len_max)
        {
            --m_accumulated_values;
            /* erase old values */
            m_valuesSetpoint.pop_front();
            m_valuesPV.pop_front();

            /* shift the axis-X */
            m_axis_x_shift += (INTERVAL_MS / 1000.0);
        }

        double sp_pv_diff = (double)((double)m_setpoint - m_processVariable);
        double sp_pv_diff_abs = fabs(sp_pv_diff);
        m_ui.processVariable_indicator->setText(QString("process variable = %1").arg(m_processVariable));
        m_ui.SPminusPV_indicator->setText(QString("SP - PV = %1 (%2% SP) (SP %3 PV)")
                                     .arg(sp_pv_diff_abs, 0, 'g', 9)
                                     .arg(100.0 * (sp_pv_diff_abs / (double)m_setpoint) , 0, 'g', 2)
                                     .arg(
                                         (sp_pv_diff < 0.0) ? "<" :
                                                            (sp_pv_diff > 0.0) ? ">" :
                                                                               "="
                                                                               )
                                     );
        m_ui.output_indicator->setText(QString("PID output = %1").arg(power));

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

    m_ui.plotSetpoint->setData(axis_x, m_valuesSetpoint);
    m_ui.plotPV->setData(axis_x, m_valuesPV);

    /* scale setup */
    if(autoScale)
    {
        double half = (max - min) / 2.0;
        double mid = (max + min) / 2.0;

        if(half == 0.0) half = 1;
        QCPRange range(mid - half * 1.3, mid + half * 1.3);
        m_ui.cplot->yAxis->setRange(range);
    }
    else
    {
        m_ui.cplot->yAxis->setRange(AXIS_Y_MIN, AXIS_Y_MAX);
    }
    m_ui.cplot->xAxis->setRange(AXIS_X_MIN + m_axis_x_shift, AXIS_X_MAX + m_axis_x_shift);

    m_ui.cplot->replot();

#ifdef APP_USE_CGRAPH
    plotSetpoint->updatePlotValues(valuesSetpoint);
    plotMain->updatePlotValues(valuesMain);
#endif
}
