#include "Device.hpp"

#include <nostd/fixed/fixed32>

#include <QVector>

#define RPC_FUNC_00_PULSES_R     0x00
#define RPC_FUNC_01_MODE_R 0x01
#define RPC_FUNC_02_KOEF_R         0x02
#define RPC_FUNC_03_KOEF_W         0x03
#define RPC_FUNC_04_SPEED_SP_R     0x04
#define RPC_FUNC_05_SPEED_SP_W     0x05
#define RPC_FUNC_06_SPEED_PV_R     0x06
#define RPC_FUNC_07_PROCESS_START  0x07
#define RPC_FUNC_08_PROCESS_STOP   0x08

#define RPC_EVENT_MODE_CHANGED 0x00
#define RPC_EVENT_SPPV 0x01

#define I32_LO(x) ((uint16_t)((uint32_t)(x)))
#define I32_HI(x) ((uint16_t)((uint32_t)(x) >> 16))
#define BUILD32(hi, lo) ((uint32_t)(hi) << 16 | (uint32_t)(lo))

static Device::RunMode P_convert_device_mode(uint16_t mode)
{
    static const QMap<uint16_t, Device::RunMode> modes
    {
        { 0x1000, Device::RunMode::NORMAL_STOPPED },
        { 0x1001, Device::RunMode::NORMAL_STARTED_AUTO },
        { 0x1002, Device::RunMode::NORMAL_STARTED_MANUAL },
        { 0x2100, Device::RunMode::SERVICE_MODE1_STOPPED },
        { 0x2101, Device::RunMode::SERVICE_MODE1_STARTED },
        { 0x2200, Device::RunMode::SERVICE_MODE2_STOPPED },
        { 0x2201, Device::RunMode::SERVICE_MODE2_STARTED },
        { 0x2300, Device::RunMode::SERVICE_MODE3_STOPPED },
        { 0x2301, Device::RunMode::SERVICE_MODE3_STARTED },
    };
    if(!modes.contains(mode)) return Device::RunMode::UNKNOWN;
    return modes[mode];
}


Device::Device(QObject *parent)
    : QObject(parent)
    , m_rpc()
    , m_ppr()
    , m_mode()
    , m_koef()
    , m_prosess_started()
    , m_statuses()
    , m_autoqueue()
{

    m_rpc.timeoutSet(5000);

    QObject::connect(&m_rpc, SIGNAL(ready_dataToSend(const QByteArray &))                                 , this, SIGNAL(ready_dataToSend(const QByteArray&)));
    QObject::connect(&m_rpc, SIGNAL(replyReceived(uint16_t, uint8_t, uint8_t, const QVector<uint16_t> &)), this, SLOT(P_rpc_replyReceived(uint16_t, uint8_t, uint8_t, const QVector<uint16_t> &)));
    QObject::connect(&m_rpc, SIGNAL(eventReceived(uint8_t, const QVector<uint16_t> &))                   , this, SLOT(P_rpc_eventReceived(uint8_t, const QVector<uint16_t> &)));
    QObject::connect(&m_rpc, SIGNAL(replyTimeout(uint16_t, uint8_t))                                     , this, SLOT(P_rpc_request_timedout(uint16_t, uint8_t)));

}

void Device::requestsStatClearAll()
{
    m_statuses.clear();
}

void Device::requestsStatClearSuccess()
{
    for(
        QMap<uint16_t, struct req_status>::iterator it = m_statuses.begin();
        it != m_statuses.end();
        )
    {
        if(it->res == ReqResult::SUCCESS)
        {
            it = m_statuses.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

QMap<uint16_t, struct Device::req_status> Device::requestsStatGet() const
{
    return m_statuses;
}

Device::RunMode Device::runModeGet() const
{
    return m_mode;
}

void Device::request_00_ppr_r()
{
    P_rpc_request_00_ppr_r(ReqMode::MANUAL);
}

void Device::runModeRead()
{
    P_rpc_request_01_mode_r(ReqMode::MANUAL);
}

void Device::runModeGet(RunMode &mode) const
{
    mode = m_mode;
}

void Device::pidKoefRead()
{
    P_rpc_request_02_koef_r(ReqMode::MANUAL);
}

bool Device::pidKoefGet(double &Kp, double &Ki, double &Kd) const
{
    Kp = m_koef.Kp;
    Ki = m_koef.Ki;
    Kd = m_koef.Kd;
    return m_koef.valid;
}

void Device::pidKoefWrite(double Kp, double Ki, double Kd)
{
    P_rpc_request_03_koef_w(Kp, Ki, Kd, ReqMode::MANUAL);
}

void Device::speedSetpointRead()
{
    P_rpc_request_04_speed_SP_r(ReqMode::MANUAL);
}

void Device::speedSetpointWrite(double rpm)
{
    if(!m_ppr.valid) throw Error::InvalidDescr;

    P_rpc_request_05_speed_SP_w(rpm * m_ppr.value, ReqMode::MANUAL);
}

void Device::speedPVRead()
{
    P_rpc_request_06_speed_PV_r(ReqMode::MANUAL);
}

void Device::request_07_process_start()
{
    P_rpc_request_07_process_start(ReqMode::MANUAL);
}

void Device::request_08_process_stop()
{
    P_rpc_request_08_process_stop(ReqMode::MANUAL);
}

void Device::devConnect()
{
    P_invalidate_all();

    P_reload_00_ppr_r();
    P_reload_01_mode_r();
    P_reload_02_koef_r();
    P_reload_07_process_start();
}

void Device::devDisconnect()
{
    P_rpc_request_08_process_stop(ReqMode::AUTO);
}

void Device::dataReplyReceive(const QByteArray & reply)
{
    m_rpc.dataReplyReceive(reply);
}

void Device::P_rpc_replyReceived(uint16_t ruid, uint8_t funcId, uint8_t err, const QVector<uint16_t> &resv)
{
    ReqMode reqmode;
    if(m_statuses.contains(ruid))
    {
        struct req_status & stat = m_statuses[ruid];
        stat.res = ReqResult::SUCCESS;
        reqmode = stat.reqmode;
    }

    if(m_autoqueue.contains(ruid))
    {
        m_autoqueue.remove(ruid);
    }

    bool good = false;
    if(err == 0)
    {
        switch(funcId)
        {
            case RPC_FUNC_00_PULSES_R:
            {
                if(resv.size() != 1)break;
                m_ppr.value = resv[0];
                m_ppr.valid = true;
                good = true;
                break;
            }
            case RPC_FUNC_01_MODE_R:
            {
                if(resv.size() != 1) break;
                m_mode = P_convert_device_mode(resv[0]);
                emit ready_runModeRead(false, m_mode);
                good = true;
                break;
            }
            case RPC_FUNC_02_KOEF_R:
            {
                if(resv.size() != 6) break;

                nostd::Fixed32 f32_Kp((fixed32_t)BUILD32(resv[0], resv[1]));
                nostd::Fixed32 f32_Ki((fixed32_t)BUILD32(resv[2], resv[3]));
                nostd::Fixed32 f32_Kd((fixed32_t)BUILD32(resv[4], resv[5]));
                double Kp = f32_Kp.toDouble();
                double Ki = f32_Ki.toDouble();
                double Kd = f32_Kd.toDouble();
                emit ready_pidKoefRead(false, Kp, Ki, Kd);
                good = true;
                break;
            }
            case RPC_FUNC_03_KOEF_W:
            {
                emit ready_pidKoefWrite(false);
                good = true;
                break;
            }
            case RPC_FUNC_04_SPEED_SP_R:
            {
                if(resv.size() != 1) break;
                if(m_ppr.valid)
                {
                    double sp = resv[0] / m_ppr.value;
                    emit ready_speedSetpointRead(false, sp);
                }
                good = true;
                break;
            }
            case RPC_FUNC_05_SPEED_SP_W:
            {
                emit ready_speedSetpointWrite(false);
                good = true;
                break;
            }
            case RPC_FUNC_06_SPEED_PV_R:
            {
                if(resv.size() != 1) break;
                if(m_ppr.valid)
                {
                    double pv = resv[0] / m_ppr.value;
                    emit ready_speedPVRead(false, pv);
                }
                good = true;
                break;
            }
            case RPC_FUNC_07_PROCESS_START :
            {
                if(resv.size() != 0) break;
                m_prosess_started = true;
                good = true;
                break;
            }
            case RPC_FUNC_08_PROCESS_STOP  :
            {
                /* do not check resv size */
                P_allowToDisconnect(false);
                good = true;
                break;
            }
            default: good = true;
        }
    }

    if(!good && reqmode == ReqMode::AUTO)
    {
        switch(funcId)
        {
            case RPC_FUNC_00_PULSES_R     : P_reload_00_ppr_r(); break;
            case RPC_FUNC_01_MODE_R       : P_reload_01_mode_r(); break;
            case RPC_FUNC_02_KOEF_R       : P_reload_02_koef_r(); break;
            case RPC_FUNC_03_KOEF_W       : break;
            case RPC_FUNC_04_SPEED_SP_R   : break;
            case RPC_FUNC_05_SPEED_SP_W   : break;
            case RPC_FUNC_06_SPEED_PV_R   : break;
            case RPC_FUNC_07_PROCESS_START: P_reload_07_process_start(); break;
            case RPC_FUNC_08_PROCESS_STOP : break;
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
            m_mode = P_convert_device_mode(resv[0]);
            emit ready_runModeChanged(m_mode);
            break;
        }
        case RPC_EVENT_SPPV:
        {
            if(!m_ppr.valid) break;
            if(resv.size() != 4) break;
            unsigned long time_ms = ((resv[0] << 16) | resv[1]);
            double sp = (double)resv[2] / (double)m_ppr.value;
            double pv = (double)resv[3] / (double)m_ppr.value;
            emit ready_SPPV(time_ms, sp, pv);
            break;
        }
    }
}

void Device::P_rpc_request_timedout(uint16_t ruid, uint8_t funcId)
{
    if(m_statuses.contains(ruid))
    {
        struct req_status & stat = m_statuses[ruid];
        stat.res = ReqResult::TIMEOUT;

        switch(funcId)
        {
            case RPC_FUNC_00_PULSES_R      : break;
            case RPC_FUNC_01_MODE_R        : ready_runModeRead(true, RunMode::UNKNOWN); break;
            case RPC_FUNC_02_KOEF_R        : ready_pidKoefRead(true, 0.0, 0.0, 0.0); break;
            case RPC_FUNC_03_KOEF_W        : ready_pidKoefWrite(true); break;
            case RPC_FUNC_04_SPEED_SP_R    : ready_speedSetpointRead(true, 0.0); break;
            case RPC_FUNC_05_SPEED_SP_W    : ready_speedSetpointWrite(true); break;
            case RPC_FUNC_06_SPEED_PV_R    : ready_speedPVRead(true, 0.0); break;
            case RPC_FUNC_07_PROCESS_START : break;
            case RPC_FUNC_08_PROCESS_STOP  : break;
            default: ;
        }
    }

    /* Autoreload only for auto mode */
    if(m_autoqueue.contains(ruid))
    {
        m_autoqueue.remove(ruid);
        switch(funcId)
        {
            case RPC_FUNC_00_PULSES_R      : P_reload_00_ppr_r(); break;
            case RPC_FUNC_01_MODE_R        : P_reload_01_mode_r(); break;
            case RPC_FUNC_02_KOEF_R        : P_reload_02_koef_r(); break;
            case RPC_FUNC_03_KOEF_W        : break;
            case RPC_FUNC_04_SPEED_SP_R    : break;
            case RPC_FUNC_05_SPEED_SP_W    : break;
            case RPC_FUNC_06_SPEED_PV_R    : break;
            case RPC_FUNC_07_PROCESS_START : P_reload_07_process_start(); break;
            case RPC_FUNC_08_PROCESS_STOP  : P_allowToDisconnect(true); break;
            default: ;
        }
    }
}

bool Device::P_enabled_runMode_service3()
{
    return (
                m_mode == RunMode::SERVICE_MODE3_STOPPED ||
                m_mode == RunMode::SERVICE_MODE3_STARTED
                );
}

void Device::P_invalidate_all()
{
    m_ppr.valid  = false;
    m_mode = RunMode::UNKNOWN;
    m_koef.valid = false;
    m_prosess_started = false;
}

void Device::P_allowToDisconnect(bool timedout)
{
    P_invalidate_all();
    emit ready_toDisconnect(timedout);
}

void Device::P_reload_00_ppr_r()
{
    if(!m_ppr.valid)
    {
        P_rpc_request_00_ppr_r(ReqMode::AUTO);
    }
}

void Device::P_reload_01_mode_r()
{
    if(m_mode == RunMode::UNKNOWN)
    {
        P_rpc_request_01_mode_r(ReqMode::AUTO);
    }
}

void Device::P_reload_02_koef_r()
{
    if(!m_koef.valid)
    {
        P_rpc_request_02_koef_r(ReqMode::AUTO);
    }
}

void Device::P_reload_07_process_start()
{
    if(!m_prosess_started)
    {
        P_rpc_request_07_process_start(ReqMode::AUTO);
    }
}

void Device::P_rpc_request_common(uint8_t funcId, const QVector<uint16_t> & argv, ReqMode reqmode)
{
    uint16_t ruid = m_rpc.requestSend(funcId, argv);
    struct req_status stat;
    stat.funcId = funcId;
    stat.reqmode = reqmode;
    stat.res = ReqResult::AWAITING;
    m_statuses[ruid] = stat;
    if(reqmode == ReqMode::AUTO)
    {
        m_autoqueue[ruid] = funcId;
    }
}

void Device::P_rpc_request_00_ppr_r(ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_00_PULSES_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_01_mode_r(ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_01_MODE_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_02_koef_r(ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_02_KOEF_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_03_koef_w(double Kp, double Ki, double Kd, ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_03_KOEF_W;
    QVector<uint16_t> argv;

    nostd::Fixed32 f32_Kp(Kp);
    nostd::Fixed32 f32_Ki(Ki);
    nostd::Fixed32 f32_Kd(Kd);

    fixed32_t raw_Kp = f32_Kp.toRawFixed();
    fixed32_t raw_Ki = f32_Ki.toRawFixed();
    fixed32_t raw_Kd = f32_Kd.toRawFixed();

    argv.append(I32_HI(raw_Kp));
    argv.append(I32_LO(raw_Kp));
    argv.append(I32_HI(raw_Ki));
    argv.append(I32_LO(raw_Ki));
    argv.append(I32_HI(raw_Kd));
    argv.append(I32_LO(raw_Kd));
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_04_speed_SP_r(ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_04_SPEED_SP_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_05_speed_SP_w(uint16_t speed, ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_05_SPEED_SP_W;
    QVector<uint16_t> argv;
    argv.append(speed);
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_06_speed_PV_r(ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_06_SPEED_PV_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_07_process_start(ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_07_PROCESS_START;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_08_process_stop(ReqMode reqmode)
{
    uint8_t funcId = RPC_FUNC_08_PROCESS_STOP;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}
