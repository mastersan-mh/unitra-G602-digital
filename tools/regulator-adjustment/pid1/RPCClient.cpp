#include "RPCClient.hpp"

#include "GClasses/GRPCDefs.hpp"
#include "defs.h"

#include <QVector>

#include <stdint.h>

static int P_u8_get(QByteArray::const_iterator &it, const QByteArray & data, uint8_t * value)
{
    uint16_t v;

    if(it == data.cend()) return -1;
    v = (*it);
    ++it;
    (*value) = v;
    return 0;
}

static int P_u16_get(QByteArray::const_iterator &it, const QByteArray & data, uint16_t * value)
{
    uint16_t v;

    if(it == data.cend()) return -1;
    v = ((uint16_t)(*it) << 8);
    ++it;
    if(it == data.cend()) return -1;
    v = v | (uint16_t)(*it);
    ++it;
    (*value) = v;
    return 0;
}

static void P_u8_append(uint8_t value, QByteArray & data)
{
    data.append((char)value);
}

static void P_u16_append(uint16_t value, QByteArray & data)
{
    data.append((char)(value >> 8));
    data.append((char)(value & 0x00ff));
}

RPCClient::RPCClient(QObject *parent)
    : QObject(parent)
    , m_ruid_last(0)
    , m_timeout(1000)
    , m_awaiting_requests()
{
}

void RPCClient::timeoutSet(int timeout)
{
    m_timeout = timeout;
}

void RPCClient::requestSend(uint8_t funcId, const QVector<uint16_t> & argv)
{
    QByteArray data;
    P_encode(m_ruid_last, funcId, argv, data);

    QTimer * timer = new QTimer();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(P_timeoutEvent()));
    timer->setSingleShot(true);
    timer->start(m_timeout);

    struct awaiting_request req;
    req.funcId = funcId;
    req.timer = timer;
    m_awaiting_requests[m_ruid_last] = req;

    emit dataReadyToSend(data);
    emit requestSended(m_ruid_last, funcId, argv);

    if(m_ruid_last == 0xFFFF)
    {
        m_ruid_last = 0;
    }
    else
    {
        ++m_ruid_last;
    }
}

void RPCClient::dataReplyReceive(const QByteArray & reply)
{
    int res;
    IncomingMsg msg;
    res = P_decode(reply, msg);
    if(res == 0)
    {
        switch(msg.type)
        {
            case GRPC_OUTMSG_TYPE_REPLY:
            {
                uint16_t ruid = msg.ruid;
                uint8_t funcId;
                if(P_awaiting_request_pop(ruid, funcId))
                {
                    emit replyReceived(
                                ruid,
                                funcId,
                                msg.error,
                                msg.resv
                                );
                }
                break;
            }
            case GRPC_OUTMSG_TYPE_EVENT:
            {
                emit eventReceived(
                            msg.eventId,
                            msg.resv
                            );
                break;
            }

        }
    }
}

int RPCClient::P_encode(
        uint16_t ruid,
        uint8_t procId,
        const QVector<uint16_t> & argv,
        QByteArray & data
        )
{
    int argc = argv.size();
    if(argc > 255) return -1;
    P_u16_append(ruid, data);
    P_u8_append(procId, data);
    P_u8_append(argc, data);
    for(QVector<uint16_t>::const_iterator it = argv.cbegin(); it != argv.cend(); ++it)
    {
        P_u16_append((*it), data);
    }
    return 0;
}

int RPCClient::P_decode(const QByteArray & data, IncomingMsg &msg)
{
    QByteArray::const_iterator it = data.cbegin();

    uint8_t resc;

    if(P_u8_get(it, data, &msg.type)) return -1;

    switch(msg.type)
    {
        case GRPC_OUTMSG_TYPE_REPLY:
        {
            if(P_u16_get(it, data, &msg.ruid)) return -1;
            if(P_u8_get(it, data, &msg.error)) return -1;
            if(P_u8_get(it, data, &resc)) return -1;
            unsigned i;
            for(i = 0; i < resc; ++i)
            {
                uint16_t res;
                if(P_u16_get(it, data, &res)) return -1;
                msg.resv.push_back(res);
            }

            break;
        }
        case GRPC_OUTMSG_TYPE_EVENT:
        {
            if(P_u8_get(it, data, &msg.eventId)) return -1;
            if(P_u8_get(it, data, &resc)) return -1;
            unsigned i;
            for(i = 0; i < resc; ++i)
            {
                uint16_t res;
                if(P_u16_get(it, data, &res)) return -1;
                msg.resv.push_back(res);
            }
            break;
        }
        default: return -1;
    }
    return 0;
}

long RPCClient::P_request_timedout_get() const
{
    for(request_t::const_iterator it = m_awaiting_requests.cbegin(); it != m_awaiting_requests.cend(); ++it)
    {
        if(!it.value().timer->isActive()) return it.key();
    }
    return -1;
}

void RPCClient::P_timeoutEvent()
{
    while(1)
    {
        long ruid = P_request_timedout_get();
        if(ruid < 0)
        {
            break;
        }
        else
        {
            uint8_t funcId;
            P_awaiting_request_pop(ruid, funcId);
            emit timedout(ruid);
        }
    }
}

/**
 * return true | false = Exists or not
 */
bool RPCClient::P_awaiting_request_pop(uint16_t ruid, uint8_t & funcId)
{
    if(m_awaiting_requests.count(ruid) > 0)
    {
        struct awaiting_request req = m_awaiting_requests.take(ruid);
        QTimer * timer = req.timer;
        QObject::disconnect(timer, SIGNAL(timeout()), this, SLOT(P_timeoutEvent()));
        delete timer;
        funcId = req.funcId;
        return true;
    }
    return false;
}


RPCClient::IncomingMsg::IncomingMsg()
    : type()
    , error()
    , ruid()
    , resv()
{
}

RPCClient::IncomingMsg::~IncomingMsg()
{
}
