#ifndef RPCCLIENT_H
#define RPCCLIENT_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QTimer>

class RPCClient : public QObject
{
    Q_OBJECT
public:

    explicit RPCClient(QObject *parent = 0);

    void timeoutSet(int timeout);
    void requestSend(uint8_t funcId, const QVector<uint16_t> &argv);
signals:
    void dataReadyToSend(const QByteArray & request);
    void requestSended(uint16_t ruid, uint8_t funcId, const QVector<uint16_t> &argv);
    void replyReceived(uint16_t ruid, uint8_t funcId, uint8_t err, const QVector<uint16_t> &resv);
    void eventReceived(uint8_t eventId, const QVector<uint16_t> &resv);
    /** @brief Time is out since sent the request. */
    void timedout(uint16_t ruid);
public slots:
    void dataReplyReceive(const QByteArray & reply);
private slots:
    void P_timeoutEvent();
private:

    struct awaiting_request
    {
        uint8_t funcId;
        QTimer * timer;
    };

    class IncomingMsg;

    uint16_t m_ruid_last;

    int m_timeout; /**< msec */

    typedef QMap<uint16_t, struct awaiting_request> request_t;
    request_t m_awaiting_requests; /**< sended requests */

    int P_encode(
            uint16_t ruid,
            uint8_t procId,
            const QVector<uint16_t> & argv,
            QByteArray & data
            );
    int P_decode(const QByteArray & data, IncomingMsg &msg);

    bool P_awaiting_request_pop(uint16_t ruid, uint8_t & funcId);

    /** @brief Get first timedout timer */
    long P_request_timedout_get() const;

    class IncomingMsg
    {
    public:
        IncomingMsg();
        ~IncomingMsg();

        uint8_t type;

        union
        {
            struct
            {
                uint8_t error;
                uint16_t ruid;
            };
            struct
            {
                uint8_t eventId;
            };
        };

        QVector<uint16_t> resv;

    };

};

#endif // RPCCLIENT_H
