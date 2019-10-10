#include "RPCClient.hpp"

#include "GClasses/GRPCDefs.hpp"
#include "defs.h"

#include <QVector>

#include <stdint.h>

static int P_u8_get(QByteArray::const_iterator &it, const QByteArray & data, uint8_t * value)
{
    uint8_t v8;

    if(it == data.cend()) return -1;
    v8 = (*it);
    ++it;
    (*value) = v8;
    return 0;
}

static int P_u16_get(QByteArray::const_iterator &it, const QByteArray & data, uint16_t * value)
{
    uint8_t v8;
    uint16_t v16;

    if(it == data.cend()) return -1;
    v8 = (*it);
    v16 = ((uint16_t)v8 << 8);
    ++it;
    if(it == data.cend()) return -1;
    v8 = (*it);
    v16 = v16 | (uint16_t)v8;
    ++it;
    (*value) = v16;
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

uint16_t RPCClient::requestSend(uint8_t funcId, const QVector<uint16_t> & argv)
{
    QByteArray data;
    uint16_t ruid = m_ruid_last;
    P_encode(ruid, funcId, argv, data);

    QTimer * timer = new QTimer();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(P_timeoutEvent()));
    timer->setSingleShot(true);
    timer->start(m_timeout);

    struct awaiting_request req;
    req.funcId = funcId;
    req.timer = timer;
    m_awaiting_requests[ruid] = req;

    emit ready_dataToSend(data);

    m_ruid_last = (m_ruid_last == 0xFFFF ? 0 : m_ruid_last + 1);

    return ruid;
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
        uint8_t funcId,
        const QVector<uint16_t> & argv,
        QByteArray & data
        ) const
{
    int argc = argv.size();
    if(argc > 255) return -1;
    P_u16_append(ruid, data);
    P_u8_append(funcId, data);
    P_u8_append(argc, data);
    for(QVector<uint16_t>::const_iterator it = argv.cbegin(); it != argv.cend(); ++it)
    {
        P_u16_append((*it), data);
    }
    return 0;
}

int RPCClient::P_decode(const QByteArray & data, IncomingMsg &msg) const
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

void RPCClient::P_timeoutEvent()
{
    for(request_t::iterator it = m_awaiting_requests.begin(); it != m_awaiting_requests.end(); )
    {
        struct awaiting_request & req = it.value();
        if(req.timer->isActive())
        {
            ++it;
        }
        else
        {
            emit replyTimeout(it.key(), req.funcId);
            P_awaiting_request_delete(req);
            it = m_awaiting_requests.erase(it);
        }
    }
}

/**
 * return true | false = Exists or not
 */
bool RPCClient::P_awaiting_request_pop(uint16_t ruid, uint8_t & funcId)
{
    if(m_awaiting_requests.contains(ruid))
    {
        struct awaiting_request req = m_awaiting_requests.take(ruid);
        P_awaiting_request_delete(req);
        funcId = req.funcId;
        return true;
    }
    return false;
}

void RPCClient::P_awaiting_request_delete(struct awaiting_request & req)
{
    QTimer * timer = req.timer;
    QObject::disconnect(timer, SIGNAL(timeout()), this, SLOT(P_timeoutEvent()));
    delete timer;
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
