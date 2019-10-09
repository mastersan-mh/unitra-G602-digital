#include "Device.hpp"

#include <nostd/fixed/fixed32>

#define RPC_FUNC_00_PULSES_R     0x00
#define RPC_FUNC_01_MODE_CURRENT_R 0x01
#define RPC_FUNC_02_KOEF_R         0x02
#define RPC_FUNC_03_KOEF_W         0x03
#define RPC_FUNC_04_SPEED_SP_R     0x04
#define RPC_FUNC_05_SPEED_SP_W     0x05
#define RPC_FUNC_06_SPEED_PV_R     0x06
#define RPC_FUNC_07_PROCESS_START  0x07
#define RPC_FUNC_08_PROCESS_STOP   0x08

#define RPC_EVENT_MODE_CHANGED 0x00
#define RPC_EVENT_SPPV 0x01

static Device::Mode P_device_mode_get(uint16_t mode)
{
    static const QMap<uint16_t, Device::Mode> modes
    {
        { 0x1000, Device::Mode::NORMAL_STOPPED },
        { 0x1001, Device::Mode::NORMAL_STARTED_AUTO },
        { 0x1002, Device::Mode::NORMAL_STARTED_MANUAL },
        { 0x2100, Device::Mode::SERVICE_MODE1_STOPPED },
        { 0x2101, Device::Mode::SERVICE_MODE1_STARTED },
        { 0x2200, Device::Mode::SERVICE_MODE2_STOPPED },
        { 0x2201, Device::Mode::SERVICE_MODE2_STARTED },
        { 0x2300, Device::Mode::SERVICE_MODE3_STOPPED },
        { 0x2301, Device::Mode::SERVICE_MODE3_STARTED },
    };
    if(!modes.contains(mode)) return Device::Mode::UNKNOWN;
    return modes[mode];
}


Device::Device(QObject *parent)
    : QObject(parent)
    , m_rpc()
    , m_ppr()
    , m_mode()
    , m_koef()
    , m_prosess_started()
{

    m_rpc.timeoutSet(5000);

    QObject::connect(&m_rpc, SIGNAL(dataReadyToSend(const QByteArray &))                                 , this, SIGNAL(dataReadyToSend(const QByteArray&)));
    QObject::connect(&m_rpc, SIGNAL(requestSended(uint16_t, uint8_t, const QVector<uint16_t> &))         , this, SLOT(P_rpc_requestSended(uint16_t, uint8_t, const QVector<uint16_t> &)));
    QObject::connect(&m_rpc, SIGNAL(replyReceived(uint16_t, uint8_t, uint8_t, const QVector<uint16_t> &)), this, SLOT(P_rpc_replyReceived(uint16_t, uint8_t, uint8_t, const QVector<uint16_t> &)));
    QObject::connect(&m_rpc, SIGNAL(eventReceived(uint8_t, const QVector<uint16_t> &))                   , this, SLOT(P_rpc_eventReceived(uint8_t, const QVector<uint16_t> &)));
    QObject::connect(&m_rpc, SIGNAL(timedout(uint16_t))                                                  , this, SLOT(P_rpc_request_timedout(uint16_t)));

}

Device::Mode Device::modeGet() const
{
    return m_mode;
}

void Device::speedSetpointSet(double rpm)
{
    if(!m_ppr.valid) throw Error::InvalidDescr;

    P_rpc_request_5_speed_SP_w(rpm * m_ppr.value);
}

void Device::devConnect()
{
    P_invalidate_all();

    P_reload();
}

void Device::devDisconnect()
{
    P_invalidate_all();
}

void Device::dataReplyReceive(const QByteArray & reply)
{
    m_rpc.dataReplyReceive(reply);
}

void Device::P_rpc_requestSended(uint16_t ruid, uint8_t funcId, const QVector<uint16_t> &argv)
{

}

void Device::P_rpc_replyReceived(uint16_t ruid, uint8_t funcId, uint8_t err, const QVector<uint16_t> &resv)
{
    if(err == 0)
    {
        switch(funcId)
        {
            case RPC_FUNC_00_PULSES_R:
            {
                if(resv.size() != 1)break;
                m_ppr.value = resv[0];
                m_ppr.valid = true;
                break;
            }
            case RPC_FUNC_01_MODE_CURRENT_R:
            {
                if(resv.size() != 1) break;
                m_mode = P_device_mode_get(resv[0]);
                break;
            }
            case RPC_FUNC_02_KOEF_R        : break;
            case RPC_FUNC_03_KOEF_W        : break;
            case RPC_FUNC_04_SPEED_SP_R    : break;
            case RPC_FUNC_05_SPEED_SP_W    : break;
            case RPC_FUNC_06_SPEED_PV_R    : break;
            case RPC_FUNC_07_PROCESS_START :
            {
                if(resv.size() != 0) break;
                m_prosess_started = true;
                break;
            }
            case RPC_FUNC_08_PROCESS_STOP  :
            {
                if(resv.size() != 0) break;
                m_prosess_started = false;
                break;
            }
            default: ;
        }
    }
}

void Device::P_rpc_eventReceived(uint8_t eventId, const QVector<uint16_t> &resv)
{
    switch(eventId)
    {
        case RPC_EVENT_MODE_CHANGED:
        {
            if(resv.size() != 1) break;
            m_mode = P_device_mode_get(resv[0]);
            break;
        }
        case RPC_EVENT_SPPV:
        {
            if(!m_ppr.valid) break;
            if(resv.size() != 4) break;
            unsigned long time_ms = ((resv[0] << 16) | resv[1]);
            double sp = (double)resv[2] / (double)m_ppr.value;
            double pv = (double)resv[3] / (double)m_ppr.value;
            emit SPPVUpdated(time_ms, sp, pv);
            break;
        }
    }
}

void Device::P_rpc_request_timedout(uint16_t ruid)
{
//    QMessageBox::critical(this, tr("Error"), QString("Request timed out of RUID %1").arg(ruid));
//    m_ui->statusBar->showMessage(tr("Request timed out"));
}

bool Device::P_allow_process_to_start()
{
    return (
                m_mode == Mode::SERVICE_MODE3_STOPPED ||
                m_mode == Mode::SERVICE_MODE3_STARTED
                );
}

void Device::P_invalidate_all()
{
    m_ppr.valid  = false;
    m_mode = Mode::UNKNOWN;
    m_koef.valid = false;
    m_prosess_started = false;
}

bool Device::P_need_to_reload()
{
    return (
                !m_ppr.valid ||
                m_mode == Mode::UNKNOWN ||
                !m_koef.valid ||
                (P_allow_process_to_start() && !m_prosess_started)
                );
}

void Device::P_reload()
{
    if(!m_ppr.valid)
    {
        P_rpc_request_0_ppr_r();
    }

    if(m_mode == Mode::UNKNOWN)
    {
        P_rpc_request_1_mode_current_r();
    }

    if(!m_koef.valid)
    {
        P_rpc_request_2_koef_r();
    }

    if(P_allow_process_to_start() && !m_prosess_started)
    {
        P_rpc_request_7_process_start();
    }
}

void Device::P_device_init()
{
     P_rpc_request_0_ppr_r();
     P_rpc_request_1_mode_current_r();
     P_rpc_request_2_koef_r();
     P_rpc_request_4_speed_SP_r();
     P_rpc_request_7_process_start();
     P_rpc_request_8_process_stop();
}

void Device::P_device_done()
{
    m_ppr.valid = false;
}

void Device::P_rpc_request_0_ppr_r()
{
    uint8_t procId = RPC_FUNC_00_PULSES_R;
    QVector<uint16_t> argv;
    m_rpc.requestSend(procId, argv);
}

void Device::P_rpc_request_1_mode_current_r()
{
    uint8_t procId = RPC_FUNC_01_MODE_CURRENT_R;
    QVector<uint16_t> argv;
    m_rpc.requestSend(procId, argv);
}

void Device::P_rpc_request_2_koef_r()
{
    uint8_t procId = RPC_FUNC_02_KOEF_R;
    QVector<uint16_t> argv;
    m_rpc.requestSend(procId, argv);
}

void Device::P_rpc_request_3_koef_w(bool)
{
    uint8_t procId = RPC_FUNC_03_KOEF_W;
    QVector<uint16_t> argv;
    argv.append(1);
    argv.append(0);
    argv.append(2);
    argv.append(0);
    argv.append(3);
    argv.append(0);
    m_rpc.requestSend(procId, argv);
}

void Device::P_rpc_request_4_speed_SP_r()
{
    uint8_t procId = RPC_FUNC_04_SPEED_SP_R;
    QVector<uint16_t> argv;
    m_rpc.requestSend(procId, argv);
}

void Device::P_rpc_request_5_speed_SP_w(uint16_t speed)
{
    uint8_t procId = RPC_FUNC_05_SPEED_SP_W;
    QVector<uint16_t> argv;
    argv.append(speed);
    m_rpc.requestSend(procId, argv);
}

void Device::P_rpc_request_6_speed_PV_r()
{
    uint8_t procId = RPC_FUNC_06_SPEED_PV_R;
    QVector<uint16_t> argv;
    m_rpc.requestSend(procId, argv);
}

void Device::P_rpc_request_7_process_start()
{
    uint8_t procId = RPC_FUNC_07_PROCESS_START;
    QVector<uint16_t> argv;
    m_rpc.requestSend(procId, argv);
}

void Device::P_rpc_request_8_process_stop()
{
    uint8_t procId = RPC_FUNC_08_PROCESS_STOP;
    QVector<uint16_t> argv;
    m_rpc.requestSend(procId, argv);
}
