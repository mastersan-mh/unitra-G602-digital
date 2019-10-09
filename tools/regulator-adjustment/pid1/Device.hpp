#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

#include "RPCClient.hpp"

#include <QObject>

class Device : public QObject
{
    Q_OBJECT
public:
    enum class Error
    {
        InvalidDescr, /**< Invalid device descriptior */
    };

    enum class Mode
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

    Mode modeGet() const;

    void speedSetpointSet(double rpm);

    void P_rpc_request_0_ppr_r();
    void P_rpc_request_1_mode_current_r();
    void P_rpc_request_2_koef_r();
    void P_rpc_request_3_koef_w(bool);
    void P_rpc_request_4_speed_SP_r();
private:
    /**
     * @param speed     speed, pulses/minute
     */
    void P_rpc_request_5_speed_SP_w(uint16_t speed);
public:
    void P_rpc_request_6_speed_PV_r();
    void P_rpc_request_7_process_start();
    void P_rpc_request_8_process_stop();

signals:
    void dataReadyToSend(const QByteArray &);
    void SPPVUpdated(unsigned long time_ms, double sp, double pv);
public slots:
    void devConnect();
    void devDisconnect();
    void dataReplyReceive(const QByteArray & reply);
private slots:
    void P_rpc_requestSended(uint16_t ruid, uint8_t funcId, const QVector<uint16_t> &argv);
    void P_rpc_replyReceived(uint16_t ruid, uint8_t funcId, uint8_t err, const QVector<uint16_t> &resv);
    void P_rpc_eventReceived(uint8_t eventId, const QVector<uint16_t> &resv);
    /** @brief Time is out since sent the request. */
    void P_rpc_request_timedout(uint16_t ruid);

private:
    RPCClient m_rpc;

    struct
    {
        bool valid; /**< Характеристики устройства верны */
        unsigned value; /**< pulses per revolution */
    } m_ppr;

    Mode m_mode;

    struct
    {
        bool valid;
        double Kp;
        double Ki;
        double Kd;
    } m_koef;

    bool m_prosess_started;

    bool P_allow_process_to_start();
    void P_invalidate_all();
    bool P_need_to_reload();
    void P_reload();

    void P_device_init();
    void P_device_done();

};

#endif // DEVICE_H
