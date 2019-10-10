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
    /**
     * @brief Send request to call the function
     * @param funcId        Function Id
     * @param argv          Arguments vector
     * @return ruid
     */
    uint16_t requestSend(uint8_t funcId, const QVector<uint16_t> &argv);
signals:
    void ready_dataToSend(const QByteArray & request);
    void replyReceived(uint16_t ruid, uint8_t funcId, uint8_t err, const QVector<uint16_t> &resv);
    void eventReceived(uint8_t eventId, const QVector<uint16_t> &resv);
    /** @brief Time is out since sent the request. */
    void replyTimeout(uint16_t ruid, uint8_t funcId);
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
            uint8_t funcId,
            const QVector<uint16_t> & argv,
            QByteArray & data
            ) const;
    int P_decode(const QByteArray & data, IncomingMsg &msg) const;

    bool P_awaiting_request_pop(uint16_t ruid, uint8_t & funcId);
    void P_awaiting_request_delete(struct awaiting_request & req);

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
