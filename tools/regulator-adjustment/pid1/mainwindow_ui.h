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

class TabDataSourceSerail : public QWidget
{
    Q_OBJECT

public:
    Console *console;

    TabDataSourceSerail(QWidget *parent)
        : QWidget(parent)
    {

        console = new Console(parent);
        console->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        console->setEnabled(false);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addWidget(console);
        setLayout(mainLayout);
    }
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

        pausePushButton = new QPushButton();
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
        QGroupBox *groupBox = new QGroupBox(QStringLiteral("Setpoint"), parent);

        setpointMode_radio1 = new QRadioButton(QStringLiteral("&Manual"), parent);
        setpointMode_radio2 = new QRadioButton(QStringLiteral("&Auto"), parent);

        QVBoxLayout *vbox = new QVBoxLayout(parent);
        vbox->addWidget(setpointMode_radio1);
        vbox->addWidget(setpointMode_radio2);
        vbox->addStretch(1);
        groupBox->setLayout(vbox);

        groupBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        return groupBox;
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

        QAction *actionConnect;
        QAction *actionDisconnect;
        QAction *actionConfigure;
        QAction *actionClear;
        QAction *actionQuit;

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
        TabDataSourceSerail * serial;
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

private:
    QGroupBox *init_UI_Kselector(QWidget * parent)
    {
        QGroupBox *groupBox = new QGroupBox(QStringLiteral("Select to try"), parent);

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
            mainMenu.actionConfigure = new QAction(MainWindow);
            mainMenu.actionClear = new QAction(MainWindow);
            mainMenu.actionQuit = new QAction(MainWindow);

            mainMenu.menuBar = new QMenuBar(MainWindow);
            mainMenu.menuCalls = new QMenu(mainMenu.menuBar);
            mainMenu.menuTools = new QMenu(mainMenu.menuBar);

            mainMenu.menuCalls->addAction(mainMenu.actionConnect);
            mainMenu.menuCalls->addAction(mainMenu.actionDisconnect);
            mainMenu.menuCalls->addSeparator();
            mainMenu.menuCalls->addAction(mainMenu.actionQuit);

            mainMenu.menuTools->addAction(mainMenu.actionConfigure);
            mainMenu.menuTools->addAction(mainMenu.actionClear);

            mainMenu.menuBar->setGeometry(QRect(0, 0, 400, 22));
            mainMenu.menuBar->addAction(mainMenu.menuCalls->menuAction());
            mainMenu.menuBar->addAction(mainMenu.menuTools->menuAction());

            /* set text */
            mainMenu.actionConnect->setText(QStringLiteral("C&onnect"));
            mainMenu.actionConnect->setToolTip(QStringLiteral("Connect to serial port"));
            mainMenu.actionConnect->setShortcut(QStringLiteral("Ctrl+O"));

            mainMenu.actionDisconnect->setText(QStringLiteral("&Disconnect"));
            mainMenu.actionDisconnect->setToolTip(QStringLiteral("Disconnect from serial port"));
            mainMenu.actionDisconnect->setShortcut(QStringLiteral("Ctrl+D"));

            mainMenu.actionConfigure->setText(QStringLiteral("&Configure"));
            mainMenu.actionConfigure->setToolTip(QStringLiteral("Configure serial port"));
            mainMenu.actionConfigure->setShortcut(QStringLiteral("Alt+C"));

            mainMenu.actionClear->setText(QStringLiteral("C&lear"));
            mainMenu.actionClear->setToolTip(QStringLiteral("Clear data"));
            mainMenu.actionClear->setShortcut(QStringLiteral("Alt+L"));

            mainMenu.actionQuit->setText(QStringLiteral("&Quit"));
            mainMenu.actionQuit->setShortcut(QStringLiteral("Ctrl+Q"));

            mainMenu.menuCalls->setTitle(QStringLiteral("Calls"));
            mainMenu.menuTools->setTitle(QStringLiteral("Tools"));

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

                    tab.serial = new TabDataSourceSerail(MainWindow);
                    tab.ownerWidget->addTab(tab.serial, QStringLiteral("Serial"));
                    tab.simulation = new TabDataSourceSimulation(MainWindow);
                    tab.ownerWidget->addTab(tab.simulation, QStringLiteral("Simulation"));
                    tab.ownerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                    ctrl1_Layout->addWidget(tab.ownerWidget, 2, 0, 1, 2);
                } UI_LEVEL_END(tab);

            } UI_LEVEL_END(ctrl1_Layout);

            /* setpoint */
            setpoint_Layout = new QVBoxLayout(central);
            centralWLayout->addLayout(setpoint_Layout);
            UI_LEVEL_BEGIN(setpoint_Layout)
            {
                setpoint_slider = new QSlider(Qt::Vertical, central);
                setpoint_Layout->addWidget(setpoint_slider);
                setpoint_slider->setMinimum(MANUAL_SETPOINT_MIN);
                setpoint_slider->setMaximum(MANUAL_SETPOINT_MAX);
                setpoint_spinBox = new QSpinBox(central);
                setpoint_Layout->addWidget(setpoint_spinBox);
                setpoint_spinBox->setMinimum(MANUAL_SETPOINT_MIN);
                setpoint_spinBox->setMaximum(MANUAL_SETPOINT_MAX);
                setpoint_spinBox->setMaximumWidth(12*10);
            } UI_LEVEL_END(setpoint_Layout);

            /* cplot ctrl */
            cplotCtrl_Layout = new QVBoxLayout(central);
            centralWLayout->addLayout(cplotCtrl_Layout);
            UI_LEVEL_BEGIN(cplotCtrl_Layout)
            {
                cplotAutoScale = new QCheckBox(QStringLiteral("autoscale"), central);
                cplotCtrl_Layout->addWidget(cplotAutoScale);
                cplotAutoScale->setMaximumWidth(8*15);
                cplotCtrl_Layout->addStretch();

            } UI_LEVEL_END(cplotCtrl_Layout);

            /* cplot widget */
            cplot = new QCustomPlot(central);
            centralWLayout->addWidget(cplot);

        } UI_LEVEL_END(m_central);

        MainWindow->setCentralWidget(central);
        MainWindow->setMenuBar(mainMenu.menuBar);

    }

};

#endif // MAINWINDOW_UI_H

