#include "Device.hpp"

#include <nostd/fixed/fixed32>

#include <QVector>

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
{

    m_rpc.timeoutSet(5000);

    QObject::connect(&m_rpc, SIGNAL(ready_dataToSend(const QByteArray &))                                , this, SIGNAL(ready_dataToSend(const QByteArray&)));
    QObject::connect(&m_rpc, SIGNAL(replyReceived(uint16_t, uint8_t, uint8_t, const QVector<uint16_t> &)), this, SLOT(P_rpc_replyReceived(uint16_t, uint8_t, uint8_t, const QVector<uint16_t> &)));
    QObject::connect(&m_rpc, SIGNAL(eventReceived(uint8_t, const QVector<uint16_t> &))                   , this, SLOT(P_rpc_eventReceived(uint8_t, const QVector<uint16_t> &)));
    QObject::connect(&m_rpc, SIGNAL(replyTimeout(uint16_t, uint8_t))                                     , this, SLOT(P_rpc_request_timedout(uint16_t, uint8_t)));

}

void Device::requestsStatClearAll()
{
    m_statuses.clear();
}

void Device::requestsStatClear(ReqResult reqResult)
{
    for(
        QMap<uint16_t, struct req_status>::iterator it = m_statuses.begin();
        it != m_statuses.end();
        )
    {
        if(it->res == reqResult)
        {
            it = m_statuses.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

Device::ReqStatuses Device::requestsStatGet() const
{
    return m_statuses;
}

unsigned Device::pulsesGet() const
{
    if(!m_ppr.valid) throw Error::InvalidValue;
    return m_ppr.value;
}

void Device::runModeRead()
{
    P_rpc_request_01_mode_r(ReqMode::MANUAL);
}

Device::RunMode Device::runModeGet() const
{
    return m_mode;
}

void Device::pidKoefRead()
{
    P_rpc_request_02_koef_r(ReqMode::MANUAL);
}

void Device::pidKoefGet(double &Kp, double &Ki, double &Kd) const
{
    if(!m_koef.valid) throw Error::InvalidValue;
    Kp = m_koef.Kp;
    Ki = m_koef.Ki;
    Kd = m_koef.Kd;
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
    if(!m_ppr.valid) throw Error::InvalidValue;

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

void Device::confStore()
{
    P_rpc_request_09_conf_store(ReqMode::MANUAL);
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
    P_invalidate_all();
    m_rpc.flush();
    m_autoqueue.clear();
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
        if(err == 0)
        {
            stat.res = ReqResult::SUCCESS;
        }
        else
        {
            stat.res = ReqResult::ERROR;
        }
        reqmode = stat.reqmode;
    }

    if(m_autoqueue.contains(ruid))
    {
        m_autoqueue.remove(ruid);
    }

    bool success = false;
    switch(funcId)
    {
        case FUNC_00_PULSES_R:
        {
            if(resv.size() != 1)break;
            m_ppr.value = resv[0];
            m_ppr.valid = true;
            emit ready_pulsesRead(false, err, m_ppr.value);
            success = true;
            break;
        }
        case FUNC_01_MODE_R:
        {
            if(resv.size() != 1)
            {
                m_mode = RunMode::UNKNOWN;
            }
            else
            {
                m_mode = P_convert_device_mode(resv[0]);
                success = true;
            }
            emit ready_runModeRead(false, err, m_mode);
            break;
        }
        case FUNC_02_KOEF_R:
        {
            double Kp;
            double Ki;
            double Kd;

            if(resv.size() != 6)
            {
                 Kp = 0.0;
                 Ki = 0.0;
                 Kd = 0.0;
            }
            else
            {
                const nostd::Fixed32 f32_Kp(BUILD32(resv[0], resv[1]), nostd::Fixed32::tag_raw);
                const nostd::Fixed32 f32_Ki(BUILD32(resv[2], resv[3]), nostd::Fixed32::tag_raw);
                const nostd::Fixed32 f32_Kd(BUILD32(resv[4], resv[5]), nostd::Fixed32::tag_raw);
                Kp = f32_Kp.toDouble();
                Ki = f32_Ki.toDouble();
                Kd = f32_Kd.toDouble();
            }
            emit ready_pidKoefRead(false, err, Kp, Ki, Kd);
            success = true;
            break;
        }
        case FUNC_03_KOEF_W:
        {
            emit ready_pidKoefWrite(false, err);
            success = true;
            break;
        }
        case FUNC_04_SPEED_SP_R:
        {
            if(resv.size() != 1) break;
            if(m_ppr.valid)
            {
                double sp = resv[0] / m_ppr.value;
                emit ready_speedSetpointRead(false, err, sp);
            }
            success = true;
            break;
        }
        case FUNC_05_SPEED_SP_W:
        {
            emit ready_speedSetpointWrite(false, err);
            success = true;
            break;
        }
        case FUNC_06_SPEED_PV_R:
        {
            if(resv.size() != 1) break;
            if(m_ppr.valid)
            {
                double pv = resv[0] / m_ppr.value;
                emit ready_speedPVRead(false, err, pv);
            }
            success = true;
            break;
        }
        case FUNC_07_PROCESS_START:
        {
            if(resv.size() == 0)
            {
                m_prosess_started = true;
            }
            emit ready_processStart(false, err);
            success = true;
            break;
        }
        case FUNC_08_PROCESS_STOP:
        {
            emit ready_processStop(false, err);
            success = true;
            break;
        }
        case FUNC_09_CONF_STORE:
        {
            emit ready_confStored(false, err);
            success = true;
            break;
        }
        default: success = true;
    }

    if(!success && reqmode == ReqMode::AUTO)
    {
        switch(funcId)
        {
            case FUNC_00_PULSES_R     : P_reload_00_ppr_r(); break;
            case FUNC_01_MODE_R       : P_reload_01_mode_r(); break;
            case FUNC_02_KOEF_R       : P_reload_02_koef_r(); break;
            case FUNC_03_KOEF_W       : break;
            case FUNC_04_SPEED_SP_R   : break;
            case FUNC_05_SPEED_SP_W   : break;
            case FUNC_06_SPEED_PV_R   : break;
            case FUNC_07_PROCESS_START: P_reload_07_process_start(); break;
            case FUNC_08_PROCESS_STOP : break;
            case FUNC_09_CONF_STORE   : break;
            default: ;
        }
    }
}

void Device::P_rpc_eventReceived(uint8_t eventId, const QVector<uint16_t> &resv)
{
    switch(eventId)
    {
        case EVENT_MODE_CHANGED:
        {
            if(resv.size() != 1) break;
            m_mode = P_convert_device_mode(resv[0]);
            emit ready_runModeChanged(m_mode);
            break;
        }
        case EVENT_SPPV:
        {
            if(!m_ppr.valid) break;
            if(resv.size() != 6) break;
            unsigned long time_ms = ((resv[0] << 16) | resv[1]);
            double sp = (double)resv[2] / (double)m_ppr.value;
            double pv = (double)resv[3] / (double)m_ppr.value;
            nostd::Fixed32 f32_out(BUILD32(resv[4], resv[5]), nostd::Fixed32::tag_raw);
            double out = f32_out.toDouble();
            emit ready_SPPV(time_ms, sp, pv, out);
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
            case FUNC_00_PULSES_R     : emit ready_pulsesRead(true, 0, 0); break;
            case FUNC_01_MODE_R       : emit ready_runModeRead(true, 0, RunMode::UNKNOWN); break;
            case FUNC_02_KOEF_R       : emit ready_pidKoefRead(true, 0, 0.0, 0.0, 0.0); break;
            case FUNC_03_KOEF_W       : emit ready_pidKoefWrite(true, 0); break;
            case FUNC_04_SPEED_SP_R   : emit ready_speedSetpointRead(true, 0, 0.0); break;
            case FUNC_05_SPEED_SP_W   : emit ready_speedSetpointWrite(true, 0); break;
            case FUNC_06_SPEED_PV_R   : emit ready_speedPVRead(true, 0, 0.0); break;
            case FUNC_07_PROCESS_START: emit ready_processStart(true, 0); break;
            case FUNC_08_PROCESS_STOP : emit ready_processStop(true, 0); break;
            case FUNC_09_CONF_STORE   : emit ready_confStored(true, 0); break;
            default: ;
        }
    }

    /* Autoreload only for auto mode */
    if(m_autoqueue.contains(ruid))
    {
        m_autoqueue.remove(ruid);
        switch(funcId)
        {
            case FUNC_00_PULSES_R     : P_reload_00_ppr_r(); break;
            case FUNC_01_MODE_R       : P_reload_01_mode_r(); break;
            case FUNC_02_KOEF_R       : P_reload_02_koef_r(); break;
            case FUNC_03_KOEF_W       : break;
            case FUNC_04_SPEED_SP_R   : break;
            case FUNC_05_SPEED_SP_W   : break;
            case FUNC_06_SPEED_PV_R   : break;
            case FUNC_07_PROCESS_START: P_reload_07_process_start(); break;
            case FUNC_08_PROCESS_STOP : break;
            case FUNC_09_CONF_STORE   : break;
            default: ;
        }
    }
}

void Device::P_invalidate_all()
{
    m_ppr.valid  = false;
    m_mode = RunMode::UNKNOWN;
    m_koef.valid = false;
    m_prosess_started = false;
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

void Device::P_rpc_request_common(enum FuncId funcId, const QVector<uint16_t> & argv, ReqMode reqmode)
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
    enum FuncId funcId = FUNC_00_PULSES_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_01_mode_r(ReqMode reqmode)
{
    enum FuncId funcId = FUNC_01_MODE_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_02_koef_r(ReqMode reqmode)
{
    enum FuncId funcId = FUNC_02_KOEF_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_03_koef_w(double Kp, double Ki, double Kd, ReqMode reqmode)
{
    enum FuncId funcId = FUNC_03_KOEF_W;
    QVector<uint16_t> argv;

    const fixed32_t raw_Kp = nostd::Fixed32(Kp, nostd::Fixed32::tag_double).toRawFixed();
    const fixed32_t raw_Ki = nostd::Fixed32(Ki, nostd::Fixed32::tag_double).toRawFixed();
    const fixed32_t raw_Kd = nostd::Fixed32(Kd, nostd::Fixed32::tag_double).toRawFixed();

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
    enum FuncId funcId = FUNC_04_SPEED_SP_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_05_speed_SP_w(uint16_t speed, ReqMode reqmode)
{
    enum FuncId funcId = FUNC_05_SPEED_SP_W;
    QVector<uint16_t> argv;
    argv.append(speed);
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_06_speed_PV_r(ReqMode reqmode)
{
    enum FuncId funcId = FUNC_06_SPEED_PV_R;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_07_process_start(ReqMode reqmode)
{
    enum FuncId funcId = FUNC_07_PROCESS_START;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_08_process_stop(ReqMode reqmode)
{
    enum FuncId funcId = FUNC_08_PROCESS_STOP;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}

void Device::P_rpc_request_09_conf_store(ReqMode reqmode)
{
    enum FuncId funcId = FUNC_09_CONF_STORE;
    QVector<uint16_t> argv;
    return P_rpc_request_common(funcId, argv, reqmode);
}
