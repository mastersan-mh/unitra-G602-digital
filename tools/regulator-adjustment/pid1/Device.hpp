#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

#include "RPCClient.hpp"

#include <QObject>

#include <QMap>

class Device : public QObject
{
    Q_OBJECT
public:
    enum class Error
    {
        InvalidDescr, /**< Invalid device descriptior */
    };

    enum class ReqMode
    {
        AUTO, /**< Запрос был выполнен в автоматическом режиме */
        MANUAL, /**< Запрос был выполнен в ручном режиме */
    };

    enum class ReqResult
    {
        AWAITING, /**< Ожидание */
        SUCCESS,  /**< Успешно */
        TIMEOUT,  /**< Таймаут */
    };

    struct req_status
    {
        uint8_t funcId;
        ReqMode reqmode;
        ReqResult res;
    };

    /** @brief Device run mode */
    enum class RunMode
    {
        UNKNOWN,
        NORMAL_STOPPED,
        NORMAL_STARTED_AUTO,
        NORMAL_STARTED_MANUAL,
        SERVICE_MODE1_STOPPED,
        SERVICE_MODE1_STARTED,
        SERVICE_MODE2_STOPPED,
        SERVICE_MODE2_STARTED,
        SERVICE_MODE3_STOPPED,
        SERVICE_MODE3_STARTED,
    };
    explicit Device(QObject *parent = 0);

    void requestsStatClearAll();
    /**
     * @brief Erase the SUCCESS statuses
     */
    void requestsStatClearSuccess();
    /**
     * @brief Get the statuses of the requests
     * @return ruid:stat
     */
    QMap<uint16_t, struct req_status> requestsStatGet() const;

    RunMode runModeGet() const;

    /* Manual requests */
    void request_00_ppr_r();
    void runModeRead();
    void runModeGet(RunMode &mode) const;
    void pidKoefRead();
    bool pidKoefGet(double &Kp, double &Ki, double &Kd) const;
    void pidKoefWrite(double Kp, double Ki, double Kd);
    void speedSetpointRead();
    void speedSetpointWrite(double rpm);
    void speedPVRead();
    void request_07_process_start();
    void request_08_process_stop();

signals:
    void ready_dataToSend(const QByteArray &);
    void ready_toDisconnect(bool timedout);
    void ready_runModeChanged(Device::RunMode mode);
    void ready_SPPV(unsigned long time_ms, double sp, double pv);

    void ready_runModeRead(bool timedout, Device::RunMode mode);
    void ready_pidKoefRead(bool timedout, double Kp, double Ki, double Kd);
    void ready_pidKoefWrite(bool timedout);
    void ready_speedSetpointRead(bool timedout, double sp);
    void ready_speedSetpointWrite(bool timedout);
    void ready_speedPVRead(bool timedout, double pv);
public slots:
    void devConnect();
    void devDisconnect();
    void dataReplyReceive(const QByteArray & reply);
private slots:
    void P_rpc_replyReceived(uint16_t ruid, uint8_t funcId, uint8_t err, const QVector<uint16_t> &resv);
    void P_rpc_eventReceived(uint8_t eventId, const QVector<uint16_t> &resv);
    /** @brief Time is out since sent the request. */
    void P_rpc_request_timedout(uint16_t ruid, uint8_t funcId);

private:
    RPCClient m_rpc;

    /** @brief pulses per revolution */
    struct
    {
        bool valid;
        unsigned value;
    } m_ppr;

    RunMode m_mode;

    struct
    {
        bool valid;
        double Kp;
        double Ki;
        double Kd;
    } m_koef;

    bool m_prosess_started;

    QMap<uint16_t, struct req_status> m_statuses;

    QMap<uint16_t /* ruid */ , uint8_t /* funcId, unused */ > m_autoqueue; /**< Очередь из автоматических запросов */

    void P_rpc_request_common(uint8_t funcId, const QVector<uint16_t> & argv, ReqMode reqmode);

    void P_rpc_request_00_ppr_r(ReqMode reqmode);
    void P_rpc_request_01_mode_r(ReqMode reqmode);
    void P_rpc_request_02_koef_r(ReqMode reqmode);
    void P_rpc_request_03_koef_w(double Kp, double Ki, double Kd, ReqMode reqmode);
    void P_rpc_request_04_speed_SP_r(ReqMode reqmode);
    /**
     * @param speed     speed, pulses/minute
     */
    void P_rpc_request_05_speed_SP_w(uint16_t speed, ReqMode reqmode);
    void P_rpc_request_06_speed_PV_r(ReqMode reqmode);
    void P_rpc_request_07_process_start(ReqMode reqmode);
    void P_rpc_request_08_process_stop(ReqMode reqmode);

    bool P_enabled_runMode_service3();
    void P_invalidate_all();
    void P_allowToDisconnect(bool timedout);

    void P_reload_00_ppr_r();
    void P_reload_01_mode_r();
    void P_reload_02_koef_r();
    void P_reload_07_process_start();

};

#endif // DEVICE_H
