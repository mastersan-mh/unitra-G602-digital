#ifndef MAINWINDOW_UI_H
#define MAINWINDOW_UI_H

#include "defs.h"

#include "console.h"

#ifdef APP_USE_CGRAPH
#   include "cgraph.h"
#else
#   include "qcustomplot.h"
#endif

#include <QMainWindow>

#include <QLayout>
#include <QHBoxLayout>

#include <QTabBar>

#include <QGroupBox>

#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>

#include <QTimer>
#include <QTime>

#include <QGraphicsScale>

#include <QWidget>
#include <QLabel>

#include <QListView>
#include <QStandardItemModel>
#include <DeviceViewModel.hpp>

#define UI_LEVEL_BEGIN(xlayout) do
#define UI_LEVEL_END(xlayout) while(0)

QT_BEGIN_NAMESPACE

static const QSizePolicy::Policy sp = QSizePolicy::Maximum;

class CPIDK: public QGroupBox
{
    Q_OBJECT
public:
    explicit CPIDK(QWidget* parent=0)
        : QGroupBox(parent)
    {
        init();
    }

    explicit CPIDK(const QString &title, QWidget* parent)
        :QGroupBox(title, parent)
    {
        init();
    }

    ~CPIDK(){}

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
    void K_changed(double)
    {
        emit valuesChanged(
                    Kp_spinBox->value(),
                    Ki_spinBox->value(),
                    Kd_spinBox->value()
                    );
    }

private:
    void init()
    {
        QGridLayout * mainLayout = new QGridLayout(this);
        this->setLayout(mainLayout);

        QLabel * Kp_label = new QLabel(tr("Kp:"), this);
        QLabel * Ki_label = new QLabel(tr("Ki:"), this);
        QLabel * Kd_label = new QLabel(tr("Kd:"), this);

        mainLayout->addWidget(Kp_label, 0, 0, 1, 1);
        mainLayout->addWidget(Ki_label, 1, 0, 1, 1);
        mainLayout->addWidget(Kd_label, 2, 0, 1, 1);

        Kp_spinBox = new QDoubleSpinBox(this);
        Ki_spinBox = new QDoubleSpinBox(this);
        Kd_spinBox = new QDoubleSpinBox(this);

        mainLayout->addWidget(Kp_spinBox, 0, 2, 1, 1);
        mainLayout->addWidget(Ki_spinBox, 1, 2, 1, 1);
        mainLayout->addWidget(Kd_spinBox, 2, 2, 1, 1);

        QObject::connect(
                    Kp_spinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(K_changed(double))
                    );
        QObject::connect(
                    Ki_spinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(K_changed(double))
                    );
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

        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }

    // Kp -  proportional gain
    QDoubleSpinBox * Kp_spinBox;
    // Ki -  Integral gain
    QDoubleSpinBox * Ki_spinBox;
    // Kd -  derivative gain
    QDoubleSpinBox * Kd_spinBox;
};

class TabDataSourceDevice : public QWidget
{
    Q_OBJECT

public:
    QLabel * m_devicePPR;
    QLabel * m_deviceStatusLabel;
    Console *m_console;
    QPushButton * m_buttonReq1;
    QPushButton * m_buttonReq2;
    QPushButton * m_buttonReq3;
    QPushButton * m_buttonReq4;
    QPushButton * m_buttonReq5;
    QPushButton * m_buttonReq6;
    QPushButton * m_buttonReq7;
    QPushButton * m_buttonReq8;

    DeviceViewModel *m_reqstat_model;
    QPushButton * m_buttonClearBad;
    QPushButton * m_buttonClearAll;

    TabDataSourceDevice(QWidget *parent)
        : QWidget(parent)
    {

        m_devicePPR = new QLabel(this);
        m_devicePPR->setText("0");
        m_devicePPR->setToolTip("Pulses Per Revolution");
        m_devicePPR->setFrameStyle(QFrame::Box | QFrame::Sunken);

        m_deviceStatusLabel = new QLabel(this);
        m_deviceStatusLabel->setText("");
        m_deviceStatusLabel->setToolTip("Device state");
        m_deviceStatusLabel->setFrameStyle(QFrame::Box | QFrame::Sunken);

        m_console = new Console(this);
        m_console->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        m_console->setEnabled(false);

        m_buttonReq1 = new QPushButton("01_MODE_CURRENT_R", this);
        m_buttonReq2 = new QPushButton("02_KOEF_R", this);
        m_buttonReq3 = new QPushButton("03_KOEF_W", this);
        m_buttonReq4 = new QPushButton("04_SPEED_SP_R", this);
        m_buttonReq5 = new QPushButton("05_SPEED_SP_W", this);
        m_buttonReq6 = new QPushButton("06_SPEED_PV_R", this);
        m_buttonReq7 = new QPushButton("07_PROCESS_START", this);
        m_buttonReq8 = new QPushButton("08_PROCESS_STOP", this);

        m_reqstat = new QListView(this);

        m_reqstat_model = new DeviceViewModel(this);
        m_reqstat->setModel(m_reqstat_model);

        m_buttonClearBad = new QPushButton("Clear bad", this);
        m_buttonClearAll = new QPushButton("Clear all", this);

        QGridLayout *mainLayout = new QGridLayout;

        mainLayout->addWidget(m_devicePPR        , 0, 0, 1, 1);
        mainLayout->addWidget(m_deviceStatusLabel, 0, 1, 1, 1);
        mainLayout->addWidget(m_console          , 1, 0, 8, 2);

        mainLayout->addWidget(m_buttonReq1, 0, 2, 1, 1);
        mainLayout->addWidget(m_buttonReq2, 1, 2, 1, 1);
        mainLayout->addWidget(m_buttonReq3, 2, 2, 1, 1);
        mainLayout->addWidget(m_buttonReq4, 3, 2, 1, 1);
        mainLayout->addWidget(m_buttonReq5, 4, 2, 1, 1);
        mainLayout->addWidget(m_buttonReq6, 5, 2, 1, 1);
        mainLayout->addWidget(m_buttonReq7, 6, 2, 1, 1);
        mainLayout->addWidget(m_buttonReq8, 7, 2, 1, 1);

        mainLayout->addWidget(m_reqstat       , 0, 3, 7, 2);
        mainLayout->addWidget(m_buttonClearBad, 7, 3, 1, 1);
        mainLayout->addWidget(m_buttonClearAll, 7, 4, 1, 1);

        setLayout(mainLayout);
    }

private:
    QListView *m_reqstat;

};

class TabDataSourceSimulation : public QWidget
{
    Q_OBJECT
public:

    QPushButton * pausePushButton;

    QRadioButton *setpointMode_radio1;
    QRadioButton *setpointMode_radio2;

    TabDataSourceSimulation(QWidget *parent)
        : QWidget(parent)
    {

        pausePushButton = new QPushButton(this);
        pausePushButton->setSizePolicy(sp, QSizePolicy::Preferred);
        pausePushButton->setText(QStringLiteral("PAUSE"));

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addWidget(pausePushButton);

        QGroupBox *setpointMode = init_UI_setpointMode(this);
        mainLayout->addWidget(setpointMode);

        mainLayout->addStretch(1);
        setLayout(mainLayout);
    }

private:

    QGroupBox *init_UI_setpointMode(QWidget * parent)
    {
        Q_UNUSED(parent);
        QGroupBox *groupBox = new QGroupBox(QStringLiteral("Setpoint"), this);

        setpointMode_radio1 = new QRadioButton(QStringLiteral("&Manual"), this);
        setpointMode_radio2 = new QRadioButton(QStringLiteral("&Auto"), this);

        QVBoxLayout *vbox = new QVBoxLayout(this);
        vbox->addWidget(setpointMode_radio1);
        vbox->addWidget(setpointMode_radio2);
        vbox->addStretch(1);
        groupBox->setLayout(vbox);

        groupBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        return groupBox;
    }

};

class CSetpointWidget: public QWidget
{
    Q_OBJECT
public:
    CSetpointWidget(QWidget * parent = 0)
        : m_layout(this)
        , m_slider(Qt::Vertical, this)
        , m_spinBox(this)
    {
        Q_UNUSED(parent);

        m_slider.setTickPosition(QSlider::TicksRight);
        m_spinBox.setMaximumWidth(18*3);

        m_layout.addWidget(&m_slider);
        m_layout.addWidget(&m_spinBox);

        m_spinBox.setMinimum(m_slider.minimum());
        m_spinBox.setMaximum(m_slider.maximum());

        setLayout(&m_layout);

        QObject::connect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(P_sliderChangeValue(int)));
        QObject::connect(&m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(P_spinBoxChangeValue(int)));
    }
    virtual ~CSetpointWidget()
    {

    }

    void setValue(int value)
    {
        m_slider.setValue(value);
        m_spinBox.setValue(value);
    }

    void setMinimum(int value)
    {
        m_slider.setMinimum(value);
        m_spinBox.setMinimum(value);
    }

    void setMaximum(int value)
    {
        m_slider.setMaximum(value);
        m_spinBox.setMaximum(value);
    }

signals:
    void valueChanged(int);
private:
    QVBoxLayout m_layout;
    QSlider m_slider;
    QSpinBox m_spinBox;

private slots:
    void P_sliderChangeValue(int value)
    {
        m_spinBox.setValue(value);
        emit valueChanged(value);
    }

    void P_spinBoxChangeValue(int value)
    {
        m_slider.setValue(value);
        emit valueChanged(value);
    }

};

class MainWindow_Ui
{
public:

    QWidget * central;

    struct
    {
        QMenuBar *menuBar;
        QMenu *menuCalls;
        QMenu *menuTools;
        QMenu *menuView;

        QAction *actionConnect;
        QAction *actionDisconnect;
        QAction *actionQuit;
        QAction *actionConfigure;
        QAction *actionClear;
        QAction *actionRunMode; /* режим исполнение: true | false = симуляция | устройство */
        QAction *actionAutoscale;

    } mainMenu;

    QStatusBar *statusBar;

    QHBoxLayout *centralWLayout;

    QGridLayout *ctrl1_Layout;

    struct
    {
        QGroupBox * main;

        QGridLayout * mainLayout;

        QDoubleSpinBox * leftValue;
        QPushButton * tryLeft;
        QPushButton * setCenterToLeft;

        QDoubleSpinBox * centerValue;
        QPushButton * tryCenter;

        QDoubleSpinBox * rightValue;
        QPushButton * tryRight;
        QPushButton * setCenterToRight;

        QGroupBox * Kselector;
    } selection;

    struct
    {
        QTabWidget * ownerWidget;
        TabDataSourceDevice * device;
        TabDataSourceSimulation * simulation;
    } tab;

    CPIDK * pidK;

    struct
    {
        QLabel * processVariable;
        QLabel * PV_amplitude;
        QLabel * diff_SP_PV;
        QLabel * PID_power;
    } indication;

    QRadioButton *Kselector_radio1_Kp;
    QRadioButton *Kselector_radio2_Ki;
    QRadioButton *Kselector_radio3_Kd;

    CSetpointWidget * m_setpoint;

#ifdef APP_USE_CGRAPH
    CGraph * graph;
    CGraphPlot * plotSetpoint;
    CGraphPlot * plotMain;
#endif

    QCustomPlot * cplot;
    QCPGraph * plotSetpoint;
    QCPGraph * plotPV;

private:
    QGroupBox *init_UI_Kselector(QWidget * parent)
    {
        QGroupBox *groupBox = new QGroupBox(QStringLiteral("Select to try"), parent);

        groupBox->setTitle("");
        Kselector_radio1_Kp = new QRadioButton(QStringLiteral("K&p"), parent);
        Kselector_radio1_Kp->setShortcut(QStringLiteral("Alt+P"));
        Kselector_radio2_Ki = new QRadioButton(QStringLiteral("K&i"), parent);
        Kselector_radio2_Ki->setShortcut(QStringLiteral("Alt+I"));
        Kselector_radio3_Kd = new QRadioButton(QStringLiteral("K&d"), parent);
        Kselector_radio3_Kd->setShortcut(QStringLiteral("Alt+D"));

        QVBoxLayout *vbox = new QVBoxLayout(parent);
        vbox->addWidget(Kselector_radio1_Kp);
        vbox->addWidget(Kselector_radio2_Ki);
        vbox->addWidget(Kselector_radio3_Kd);
        vbox->addStretch(1);
        groupBox->setLayout(vbox);

        return groupBox;
    }

public:
    void init_UI(QMainWindow *MainWindow)
    {

        MainWindow->setWindowTitle(QStringLiteral("PID adjusting"));

        UI_LEVEL_BEGIN(mainMenu)
        {
            mainMenu.actionConnect = new QAction(MainWindow);
            mainMenu.actionDisconnect = new QAction(MainWindow);
            mainMenu.actionQuit = new QAction(MainWindow);
            mainMenu.actionConfigure = new QAction(MainWindow);
            mainMenu.actionClear = new QAction(MainWindow);
            mainMenu.actionRunMode = new QAction(MainWindow);
            mainMenu.actionAutoscale = new QAction(MainWindow);

            mainMenu.menuBar = new QMenuBar(MainWindow);
            mainMenu.menuCalls = new QMenu(mainMenu.menuBar);
            mainMenu.menuTools = new QMenu(mainMenu.menuBar);
            mainMenu.menuView = new QMenu(mainMenu.menuBar);

            mainMenu.menuCalls->setTitle(QStringLiteral("Calls"));
            mainMenu.menuTools->setTitle(QStringLiteral("Tools"));
            mainMenu.menuView->setTitle(QStringLiteral("View"));

            mainMenu.menuCalls->addAction(mainMenu.actionConnect);
            mainMenu.menuCalls->addAction(mainMenu.actionDisconnect);
            mainMenu.menuCalls->addSeparator();
            mainMenu.menuCalls->addAction(mainMenu.actionQuit);

            mainMenu.menuTools->addAction(mainMenu.actionConfigure);
            mainMenu.menuTools->addAction(mainMenu.actionClear);
            mainMenu.menuTools->addSeparator();
            mainMenu.menuTools->addAction(mainMenu.actionRunMode);

            mainMenu.menuView->addAction(mainMenu.actionAutoscale);

            mainMenu.menuBar->setGeometry(QRect(0, 0, 400, 22));
            mainMenu.menuBar->addAction(mainMenu.menuCalls->menuAction());
            mainMenu.menuBar->addAction(mainMenu.menuTools->menuAction());
            mainMenu.menuBar->addAction(mainMenu.menuView->menuAction());

            /* set text */
            mainMenu.actionConnect->setText(QStringLiteral("C&onnect"));
            mainMenu.actionConnect->setToolTip(QStringLiteral("Connect to serial port"));
            mainMenu.actionConnect->setShortcut(QStringLiteral("Ctrl+O"));

            mainMenu.actionDisconnect->setText(QStringLiteral("&Disconnect"));
            mainMenu.actionDisconnect->setToolTip(QStringLiteral("Disconnect from serial port"));
            mainMenu.actionDisconnect->setShortcut(QStringLiteral("Ctrl+D"));

            mainMenu.actionQuit->setText(QStringLiteral("&Quit"));
            mainMenu.actionQuit->setShortcut(QStringLiteral("Ctrl+Q"));

            mainMenu.actionConfigure->setText(QStringLiteral("&Configure"));
            mainMenu.actionConfigure->setToolTip(QStringLiteral("Configure serial port"));
            mainMenu.actionConfigure->setShortcut(QStringLiteral("Alt+C"));

            mainMenu.actionClear->setText(QStringLiteral("C&lear"));
            mainMenu.actionClear->setToolTip(QStringLiteral("Clear data"));
            mainMenu.actionClear->setShortcut(QStringLiteral("Alt+L"));

            mainMenu.actionRunMode->setText(QStringLiteral("Si&mulation"));
            mainMenu.actionRunMode->setToolTip(QStringLiteral("Run in simulation or real mode"));
            mainMenu.actionRunMode->setShortcut(QStringLiteral("Alt+M"));
            mainMenu.actionRunMode->setCheckable(true);

            mainMenu.actionAutoscale->setText(QStringLiteral("Auto&scale"));
            mainMenu.actionAutoscale->setToolTip(QStringLiteral("Autoscale the plots"));
            mainMenu.actionAutoscale->setShortcut(QStringLiteral("Alt+S"));
            mainMenu.actionAutoscale->setCheckable(true);

        } UI_LEVEL_END(mainMenu);

        UI_LEVEL_BEGIN(statusBar)
        {
            statusBar = new QStatusBar(MainWindow);
            MainWindow->setStatusBar(statusBar);
        } UI_LEVEL_END(statusBar);

        central = new QWidget(MainWindow);
        centralWLayout = new QHBoxLayout(central);
        central->setLayout(centralWLayout);
        UI_LEVEL_BEGIN(central)
        {
            ctrl1_Layout = new QGridLayout(central);
            centralWLayout->addLayout(ctrl1_Layout);
            UI_LEVEL_BEGIN(ctrl1_Layout)
            {
                selection.main = new QGroupBox("K select", central);
                ctrl1_Layout->addWidget(selection.main, 0, 0, 1, 2);
                UI_LEVEL_BEGIN(selection)
                {

                    /*
                      LLLL          CCCC            RRRR  SEL
                      <try-left>             <try-right>  SEL
                      <left-to-center> <right-to-center>  SEL
                    */

                    selection.mainLayout = new QGridLayout(selection.main);
                    selection.main->setLayout(selection.mainLayout);
                    selection.main->setSizePolicy(sp, sp);

                    /* column 0 */
                    selection.leftValue = new QDoubleSpinBox(selection.main);
                    selection.leftValue->setMinimum(PIDK_SELECTION_MIN);
                    selection.leftValue->setMaximum(PIDK_SELECTION_MAX);
                    selection.leftValue->setDecimals(9);
                    selection.mainLayout->addWidget(selection.leftValue, 0, 0, 1, 2);

                    selection.tryLeft = new QPushButton("Try", selection.main);
                    selection.mainLayout->addWidget(selection.tryLeft, 1, 0, 2, 2);

                    selection.setCenterToLeft = new QPushButton("Set <", selection.main);
                    selection.mainLayout->addWidget(selection.setCenterToLeft, 3, 0, 1, 3);

                    /* column 1 */
                    selection.centerValue = new QDoubleSpinBox(selection.main);
                    selection.centerValue->setMinimum(PIDK_SELECTION_MIN);
                    selection.centerValue->setMaximum(PIDK_SELECTION_MAX);
                    selection.centerValue->setDecimals(9);
                    selection.mainLayout->addWidget(selection.centerValue, 0, 2, 1, 2);

                    selection.tryCenter = new QPushButton("Try", selection.main);
                    selection.mainLayout->addWidget(selection.tryCenter, 1, 2, 2, 2);

                    /* column 2 */
                    selection.rightValue = new QDoubleSpinBox(selection.main);
                    selection.rightValue->setMinimum(PIDK_SELECTION_MIN);
                    selection.rightValue->setMaximum(PIDK_SELECTION_MAX);
                    selection.rightValue->setDecimals(9);
                    selection.mainLayout->addWidget(selection.rightValue, 0, 4, 1, 2);

                    selection.tryRight = new QPushButton("Try", selection.main);
                    selection.mainLayout->addWidget(selection.tryRight, 1, 4, 2, 2);

                    selection.setCenterToRight = new QPushButton("> Set", selection.main);
                    selection.mainLayout->addWidget(selection.setCenterToRight, 3, 3, 1, 3);

                    /* column 3 */
                    selection.Kselector = init_UI_Kselector(central);
                    selection.Kselector->setSizePolicy(sp, QSizePolicy::Preferred);
                    selection.mainLayout->addWidget(selection.Kselector, 0, 6, 4, 1);


                } UI_LEVEL_END(selection);

                /* pid */
                UI_LEVEL_BEGIN(pidK)
                {

    #define PIDK_MAXIMUM_WIDTH (28 + 8 + 10 * 12)
                    pidK = new CPIDK("PID", central);
                    pidK->setMaximumWidth(PIDK_MAXIMUM_WIDTH);
                    ctrl1_Layout->addWidget(pidK, 1, 0, 1, 1);
                } UI_LEVEL_END(pidK);

                UI_LEVEL_BEGIN(indication)
                {
                    QGroupBox *groupBox = new QGroupBox(QStringLiteral("Output"), MainWindow);
                    QVBoxLayout *vbox = new QVBoxLayout(MainWindow);

                    indication.processVariable = new QLabel(MainWindow);
                    indication.processVariable->setFrameStyle(QFrame::Box | QFrame::Sunken);
                    indication.processVariable->setText(QString("process variable = %1").arg(0));
                    vbox->addWidget(indication.processVariable);

                    indication.PV_amplitude = new QLabel(MainWindow);
                    indication.PV_amplitude->setFrameStyle(QFrame::Box | QFrame::Sunken);
                    indication.PV_amplitude->setText(QString("PV amplitude = %1").arg(0.0));
                    vbox->addWidget(indication.PV_amplitude);

                    indication.diff_SP_PV = new QLabel(MainWindow);
                    indication.diff_SP_PV->setFrameStyle(QFrame::Box | QFrame::Sunken);
                    indication.diff_SP_PV->setText(QString("SP - PV = %1").arg(0.0));
                    vbox->addWidget(indication.diff_SP_PV);

                    indication.PID_power = new QLabel(MainWindow);
                    indication.PID_power->setFrameStyle(QFrame::Box | QFrame::Sunken);
                    indication.PID_power->setText(QString("PID output = %1").arg(0.0));
                    vbox->addWidget(indication.PID_power);

                    vbox->addStretch(1);
                    groupBox->setLayout(vbox);
                    groupBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

                    ctrl1_Layout->addWidget(groupBox, 1, 1, 1, 1);
                } UI_LEVEL_END(indication);

                UI_LEVEL_BEGIN(tab)
                {
                    tab.ownerWidget = new QTabWidget(MainWindow);
                    tab.ownerWidget->setSizePolicy(sp, sp);

                    tab.device = new TabDataSourceDevice(MainWindow);
                    tab.ownerWidget->addTab(tab.device, QStringLiteral("Serial"));
                    tab.simulation = new TabDataSourceSimulation(MainWindow);
                    tab.ownerWidget->addTab(tab.simulation, QStringLiteral("Simulation"));
                    tab.ownerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                    ctrl1_Layout->addWidget(tab.ownerWidget, 2, 0, 1, 2);
                } UI_LEVEL_END(tab);

            } UI_LEVEL_END(ctrl1_Layout);

            /* setpoint */
            m_setpoint = new CSetpointWidget(MainWindow);
            centralWLayout->addWidget(m_setpoint);

            m_setpoint->setMinimum(MANUAL_SETPOINT_MIN);
            m_setpoint->setMaximum(MANUAL_SETPOINT_MAX);


            /* cplot widget */
            cplot = new QCustomPlot(central);
            cplot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            centralWLayout->addWidget(cplot);

        } UI_LEVEL_END(m_central);

        MainWindow->setCentralWidget(central);
        MainWindow->setMenuBar(mainMenu.menuBar);

    }

};

#endif // MAINWINDOW_UI_H

