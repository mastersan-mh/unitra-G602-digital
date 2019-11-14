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

static const QSizePolicy::Policy defaultSizePolicy = QSizePolicy::Maximum;

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
    QPushButton * m_buttonReq2;
    QPushButton * m_buttonReq3;
    QPushButton * m_buttonReq4;
    QPushButton * m_buttonReq5;
    QPushButton * m_buttonReq6;
    QPushButton * m_buttonReq7;
    QPushButton * m_buttonReq8;
    QPushButton * m_buttonReq9;

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

        m_buttonReq2 = new QPushButton(QStringLiteral("KOEF_R"), this);
        m_buttonReq3 = new QPushButton(QStringLiteral("KOEF_W"), this);
        m_buttonReq4 = new QPushButton(QStringLiteral("SPEED_SP_R"), this);
        m_buttonReq5 = new QPushButton(QStringLiteral("SPEED_SP_W"), this);
        m_buttonReq6 = new QPushButton(QStringLiteral("SPEED_PV_R"), this);
        m_buttonReq7 = new QPushButton(QStringLiteral("PROCESS_START"), this);
        m_buttonReq8 = new QPushButton(QStringLiteral("PROCESS_STOP"), this);
        m_buttonReq9 = new QPushButton(QStringLiteral("CONF_STORE"), this);

        m_reqstat = new QListView(this);

        m_reqstat_model = new DeviceViewModel(this);
        m_reqstat->setModel(m_reqstat_model);

        m_buttonClearBad = new QPushButton("Clear bad", this);
        m_buttonClearAll = new QPushButton("Clear all", this);

        QGridLayout *mainLayout = new QGridLayout;

        mainLayout->addWidget(m_devicePPR        , 0, 0, 1, 1);
        mainLayout->addWidget(m_deviceStatusLabel, 0, 1, 1, 2);
        mainLayout->addWidget(m_console          , 1, 0, 8, 3);

        mainLayout->addWidget(m_buttonReq2, 0, 3, 1, 1);
        mainLayout->addWidget(m_buttonReq3, 1, 3, 1, 1);
        mainLayout->addWidget(m_buttonReq4, 2, 3, 1, 1);
        mainLayout->addWidget(m_buttonReq5, 3, 3, 1, 1);
        mainLayout->addWidget(m_buttonReq6, 4, 3, 1, 1);
        mainLayout->addWidget(m_buttonReq7, 5, 3, 1, 1);
        mainLayout->addWidget(m_buttonReq8, 6, 3, 1, 1);
        mainLayout->addWidget(m_buttonReq9, 7, 3, 1, 1);

        mainLayout->addWidget(m_reqstat       , 0, 4, 7, 2);
        mainLayout->addWidget(m_buttonClearBad, 7, 4, 1, 1);
        mainLayout->addWidget(m_buttonClearAll, 7, 5, 1, 1);

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
        pausePushButton->setSizePolicy(defaultSizePolicy, QSizePolicy::Preferred);
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

    QHBoxLayout *centralHLayout;

    QGridLayout *ctrl1_Layout;

    struct
    {
        QRadioButton *Kp;
        QRadioButton *Ki;
        QRadioButton *Kd;
        QGroupBox * Kselector;
    } Kswitch;

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

    } Ktry;

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


    struct
    {
        QVBoxLayout * layout;
        QLabel * setpoint_actual;
        CSetpointWidget * setpoint;
        QPushButton * set45;
        QPushButton * set33;
        QPushButton * set0;
    } m_sp;

#ifdef APP_USE_CGRAPH
    CGraph * graph;
    CGraphPlot * plotSetpoint;
    CGraphPlot * plotMain;
#endif

    QCustomPlot * cplot;
    QCPGraph * plotSetpoint;
    QCPGraph * plotPV;
    QCPGraph * plotOut;

private:
    QGroupBox *init_UI_Kswitch(QWidget * parent)
    {
        QGroupBox *groupBox = new QGroupBox(QStringLiteral("K switch"),parent);
        Kswitch.Kp = new QRadioButton(QStringLiteral("K&p"), parent);
        Kswitch.Kp->setShortcut(QStringLiteral("Alt+P"));
        Kswitch.Ki = new QRadioButton(QStringLiteral("K&i"), parent);
        Kswitch.Ki->setShortcut(QStringLiteral("Alt+I"));
        Kswitch.Kd = new QRadioButton(QStringLiteral("K&d"), parent);
        Kswitch.Kd->setShortcut(QStringLiteral("Alt+D"));

        QVBoxLayout *vbox = new QVBoxLayout(parent);
        vbox->addWidget(Kswitch.Kp);
        vbox->addWidget(Kswitch.Ki);
        vbox->addWidget(Kswitch.Kd);
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
        centralHLayout = new QHBoxLayout(central);
        central->setLayout(centralHLayout);
        UI_LEVEL_BEGIN(central)
        {
            ctrl1_Layout = new QGridLayout(central);
            centralHLayout->addLayout(ctrl1_Layout);
            UI_LEVEL_BEGIN(ctrl1_Layout)
            {
                /* pid */
                UI_LEVEL_BEGIN(pidK)
                {

#define PIDK_MAXIMUM_WIDTH (28 + 8 + 10 * 12)
                    pidK = new CPIDK("PID", central);
                    pidK->setMaximumWidth(PIDK_MAXIMUM_WIDTH);
                    ctrl1_Layout->addWidget(pidK, 0, 0, 1, 1);
                } UI_LEVEL_END(pidK);


                UI_LEVEL_BEGIN(Kswitch)
                {
                    Kswitch.Kselector = init_UI_Kswitch(central);
                    Kswitch.Kselector->setSizePolicy(defaultSizePolicy, QSizePolicy::Preferred);
                    ctrl1_Layout->addWidget(Kswitch.Kselector, 0, 1, 1, 1);
                } UI_LEVEL_END(Kswitch);

                UI_LEVEL_BEGIN(Ktry)
                {
                    Ktry.main = new QGroupBox("K select", central);
                    ctrl1_Layout->addWidget(Ktry.main, 0, 2, 1, 2);

                    /*
                      LLLL          CCCC            RRRR
                      <try-left>             <try-right>
                      <left-to-center> <right-to-center>
                    */

                    Ktry.mainLayout = new QGridLayout(Ktry.main);
                    Ktry.main->setLayout(Ktry.mainLayout);
                    Ktry.main->setSizePolicy(defaultSizePolicy, defaultSizePolicy);

                    Ktry.leftValue = new QDoubleSpinBox(Ktry.main);
                    Ktry.leftValue->setMinimum(PIDK_SELECTION_MIN);
                    Ktry.leftValue->setMaximum(PIDK_SELECTION_MAX);
                    Ktry.leftValue->setDecimals(8);

                    Ktry.tryLeft = new QPushButton("Try", Ktry.main);

                    Ktry.setCenterToLeft = new QPushButton("<--- Set", Ktry.main);

                    Ktry.centerValue = new QDoubleSpinBox(Ktry.main);
                    Ktry.centerValue->setMinimum(PIDK_SELECTION_MIN);
                    Ktry.centerValue->setMaximum(PIDK_SELECTION_MAX);
                    Ktry.centerValue->setDecimals(8);

                    Ktry.tryCenter = new QPushButton("Try", Ktry.main);

                    Ktry.rightValue = new QDoubleSpinBox(Ktry.main);
                    Ktry.rightValue->setMinimum(PIDK_SELECTION_MIN);
                    Ktry.rightValue->setMaximum(PIDK_SELECTION_MAX);
                    Ktry.rightValue->setDecimals(8);

                    Ktry.tryRight = new QPushButton("Try", Ktry.main);

                    Ktry.setCenterToRight = new QPushButton("Set --->", Ktry.main);

                    Ktry.mainLayout->addWidget(Ktry.leftValue       , 0, 0, 1, 2);
                    Ktry.mainLayout->addWidget(Ktry.centerValue     , 0, 2, 1, 2);
                    Ktry.mainLayout->addWidget(Ktry.rightValue      , 0, 4, 1, 2);
                    Ktry.mainLayout->addWidget(Ktry.tryLeft         , 1, 0, 2, 2);
                    Ktry.mainLayout->addWidget(Ktry.tryCenter       , 1, 2, 2, 2);
                    Ktry.mainLayout->addWidget(Ktry.tryRight        , 1, 4, 2, 2);
                    Ktry.mainLayout->addWidget(Ktry.setCenterToLeft , 3, 0, 1, 3);
                    Ktry.mainLayout->addWidget(Ktry.setCenterToRight, 3, 3, 1, 3);

                } UI_LEVEL_END(Ktry);

                UI_LEVEL_BEGIN(indication)
                {
                    QGroupBox *groupBox = new QGroupBox(QStringLiteral("Process"), MainWindow);
                    QGridLayout *layout = new QGridLayout(MainWindow);

                    indication.processVariable = new QLabel(MainWindow);
                    indication.processVariable->setFrameStyle(QFrame::Box | QFrame::Sunken);
                    indication.processVariable->setToolTip(QStringLiteral("Process Variable"));

                    indication.PID_power = new QLabel(MainWindow);
                    indication.PID_power->setFrameStyle(QFrame::Box | QFrame::Sunken);
                    indication.PID_power->setText(QStringLiteral("PID output = ?"));
                    indication.PID_power->setToolTip(QStringLiteral("PID output"));

                    indication.PV_amplitude = new QLabel(MainWindow);
                    indication.PV_amplitude->setFrameStyle(QFrame::Box | QFrame::Sunken);
                    indication.PV_amplitude->setToolTip(QStringLiteral("Process Variable amplitude"));

                    indication.diff_SP_PV = new QLabel(MainWindow);
                    indication.diff_SP_PV->setFrameStyle(QFrame::Box | QFrame::Sunken);

                    layout->addWidget(indication.processVariable, 0, 0, 1, 1);
                    layout->addWidget(indication.PID_power      , 1, 0, 1, 1);
                    layout->addWidget(indication.PV_amplitude   , 0, 1, 1, 1);
                    layout->addWidget(indication.diff_SP_PV     , 1, 1, 1, 1);

                    groupBox->setLayout(layout);
                    groupBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

                    ctrl1_Layout->addWidget(groupBox, 1, 0, 1, 4);
                } UI_LEVEL_END(indication);

                UI_LEVEL_BEGIN(tab)
                {
                    tab.ownerWidget = new QTabWidget(MainWindow);
                    tab.ownerWidget->setSizePolicy(defaultSizePolicy, defaultSizePolicy);

                    tab.device = new TabDataSourceDevice(MainWindow);
                    tab.ownerWidget->addTab(tab.device, QStringLiteral("Unitra G602"));
                    tab.simulation = new TabDataSourceSimulation(MainWindow);
                    tab.ownerWidget->addTab(tab.simulation, QStringLiteral("Simulation"));
                    tab.ownerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                    ctrl1_Layout->addWidget(tab.ownerWidget, 2, 0, 1, 4);
                } UI_LEVEL_END(tab);

            } UI_LEVEL_END(ctrl1_Layout);

            /* setpoint */
            UI_LEVEL_BEGIN(m_sp)
            {
                m_sp.setpoint_actual = new QLabel(MainWindow);
                m_sp.setpoint_actual->setToolTip(QStringLiteral("Actual setpoint"));
                m_sp.setpoint_actual->setFrameStyle(QFrame::Box | QFrame::Sunken);
                m_sp.setpoint = new CSetpointWidget(MainWindow);
                m_sp.setpoint->setMinimum(MANUAL_SETPOINT_MIN);
                m_sp.setpoint->setMaximum(MANUAL_SETPOINT_MAX);
                m_sp.set0 = new QPushButton(QStringLiteral("0"), MainWindow);
                m_sp.set33 = new QPushButton(QStringLiteral("33"), MainWindow);
                m_sp.set45 = new QPushButton(QStringLiteral("45"), MainWindow);

                m_sp.layout = new QVBoxLayout();
                m_sp.layout->addWidget(m_sp.setpoint_actual);
                m_sp.layout->addWidget(m_sp.setpoint);
                m_sp.layout->addWidget(m_sp.set45);
                m_sp.layout->addWidget(m_sp.set33);
                m_sp.layout->addWidget(m_sp.set0);
            } UI_LEVEL_END(m_sp);

            centralHLayout->addLayout(m_sp.layout);

            /* cplot widget */
            cplot = new QCustomPlot(central);
            cplot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            centralHLayout->addWidget(cplot);

        } UI_LEVEL_END(m_central);

        MainWindow->setCentralWidget(central);
        MainWindow->setMenuBar(mainMenu.menuBar);

    }

};

#endif // MAINWINDOW_UI_H

