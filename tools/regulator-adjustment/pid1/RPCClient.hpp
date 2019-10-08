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

    void requestSend(uint8_t funcId, QVector<uint16_t> argv);
signals:
    void readyDataToSend(const QByteArray & request);
    void packetRuidSended(unsigned ruid);
    void packetRuidReceived(unsigned ruid);
    /** @brief Time is out since sent the request. */
    void timedout(unsigned ruid);
public slots:
    void replyReceived(const QByteArray & reply);
private slots:
    void P_timeoutEvent();
private:

    class IncomingMsg;

    uint16_t m_ruid_last;

    int m_timeout; /**< msec */

    typedef QMap<uint16_t, QTimer*> request_t;
    request_t m_requests; /**< sended requests */

    int P_encode(
            uint16_t ruid,
            uint8_t procId,
            const QVector<uint16_t> & argv,
            QByteArray & data
            );
    int P_decode(const QByteArray & data, IncomingMsg &msg);

    void P_request_pop(uint16_t ruid);

    /** @brief Get first timedout timer */
    long P_request_timedout_get() const;

    class IncomingMsg
    {
    public:
        enum class Error
        {
        INVALID_MSG_TYPE
        };

        enum class Type
        {
            REPLY,
            EVENT,
        };

        IncomingMsg();
        ~IncomingMsg();
        Type typeGet() const;
        uint8_t errorGet() const;
        uint16_t ruidGet() const;
        const QVector<uint16_t> & resvGet() const;
    private:
        Type m_type;

        union
        {
            struct
            {
                uint8_t m_error;
                uint16_t m_ruid;
            };
            struct
            {
                uint8_t m_eventId;
            };
        };

        QVector<uint16_t> m_resv;

        friend int RPCClient::P_decode(const QByteArray & data, IncomingMsg &msg);
    };

};

#endif // RPCCLIENT_H
