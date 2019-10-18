/**
 * @file GRPCServer.hpp
 * @brief Remote procedure call class
 */

#include "GRPCServer.hpp"

#include "utils.hpp"

#include "config.hpp"

GRPCServer::GRPCServer(
        void (*send)(const uint8_t * data, unsigned data_size, void * extraargs),
        void * extraargs
)
: m_extraargs(extraargs)
, m_send(send)
, m_funcs()
, m_func_num()
, m_argv()
, m_resv()
{
}

GRPCServer::~GRPCServer()
{

}

void GRPCServer::funcs_register(const func_t *funcs[])
{
    m_funcs = funcs;
    m_func_num = 0;
    while(m_funcs[m_func_num] != nullptr) m_func_num++;
}

int GRPCServer::P_u8_get(const uint8_t * data, unsigned size, uint8_t * value)
{
    if(size < 1) return -1;
    (*value) = (data[0]);
    return 0;
}

int GRPCServer::P_u16_get(const uint8_t * data, unsigned size, uint16_t * value)
{
    if(size < 2) return -1;
    (*value) = (uint16_t)((uint16_t)data[1] | (((uint16_t)data[0]) << 8));
    return 0;
}

int GRPCServer::P_u8_set(uint8_t value, uint8_t * data, unsigned size)
{
    if(size < 1) return -1;
    data[0] = value;
    return 0;
}

int GRPCServer::P_u16_set(uint16_t value, uint8_t * data, unsigned size)
{
    if(size < 2) return -1;
    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)(value & 0x00ff);
    return 0;
}

GRPCServer::Error GRPCServer::handle(const uint8_t * req, unsigned req_size)
{

    int res;

    uint16_t ruid;
    uint8_t funcid;
    uint8_t argc;
    unsigned resc = 0;

    //DEBUG_PRINTLN("GRPCServer::handle(): req:");
    //dump_u8(req, req_size);

    res = P_decode(
            req,
            req_size,
            &ruid,
            &funcid,
            &argc,
            m_argv
    );
    if(res)
    {
        return Error::DECODE;
    }

    //DEBUG_PRINTLN("GRPCServer::handle(): m_argv:");
    //dump_u16(m_argv, argc);

    uint8_t err = (*m_funcs[funcid])(argc, m_argv, &resc, m_resv, m_extraargs);

    //DEBUG_PRINTLN("GRPCServer::handle(): m_resv:");
    //dump_u16(m_resv, resc);

    unsigned reply_size;

    res = P_encode_reply(
            err,
            ruid,
            resc,
            m_resv,
            m_buf_reply,
            GRPCSERVER_SENDBUF_SIZE,
            &reply_size
    );

    if(res)
    {
        return Error::ENCODE;
    }

    //DEBUG_PRINTLN("GRPCServer::handle(): reply:");
    //dump_u8(m_buf_reply, reply_size);

    m_send(m_buf_reply, reply_size, m_extraargs);

    return Error::OK;
}

GRPCServer::Error GRPCServer::event(
    uint8_t eventId,
    unsigned resc,
    const uint16_t * resv
)
{
    uint8_t reply[GRPCSERVER_SENDBUF_SIZE];
    unsigned reply_size;

    int res = P_encode_event(
        eventId,
        resc,
        resv,
        reply,
        GRPCSERVER_SENDBUF_SIZE,
        &reply_size
    );

    if(res)
    {
        return Error::ENCODE;
    }

    m_send(reply, reply_size, m_extraargs);
    return Error::OK;
}

int GRPCServer::P_decode(
        const uint8_t * data,
        unsigned size,
        uint16_t * ruid,
        uint8_t * funcid,
        uint8_t * argc,
        uint16_t * argv
)
{

    int res;

    res = P_u16_get(data, size, ruid);
    if(res)
    {
        return -1;
    }
    data += 2;
    size -= 2;

    res = P_u8_get(data, size, funcid);
    if(res)
    {
        return -1;
    }
    data += 1;
    size -= 1;

    if((*funcid) >= m_func_num)
    {
        return -1;
    }

    res = P_u8_get(data, size, argc);
    if(res)
    {
        return -1;
    }
    data += 1;
    size -= 1;

    if((*argc) > GRPCSERVER_ARGV_SIZE)
    {
        return -1;
    }

    unsigned i;
    for(i = 0; i < (*argc); ++i)
    {
        res = P_u16_get(data, size, &argv[i]);
        if(res)
        {
            return -1;
        }
        data += 2;
        size -= 2;
    }

    return 0;
}


int GRPCServer::P_encode_reply(
        uint8_t err,
        uint16_t ruid,
        unsigned resc,
        const uint16_t * resv,
        uint8_t *outmsg,
        unsigned outmsg_capacity,
        unsigned * outmsg_size
)
{
    int res;

    if(resc > GRPCSERVER_RESV_SIZE)
    {
        return -1;
    }

    unsigned size = 0;
    res = P_u8_set(GRPC_OUTMSG_TYPE_REPLY, &outmsg[size], outmsg_capacity);
    if(res)
    {
        return -1;
    }
    size += 1;
    outmsg_capacity -= 1;

    res = P_u16_set(ruid, &outmsg[size], outmsg_capacity);
    if(res)
    {
        return -1;
    }
    size += 2;
    outmsg_capacity -= 2;

    res = P_u8_set(err, &outmsg[size], outmsg_capacity);
    if(res)
    {
        return -1;
    }
    size += 1;
    outmsg_capacity -= 1;

    res = P_u8_set((uint8_t)resc, &outmsg[size], outmsg_capacity);
    if(res)
    {
        return -1;
    }
    size += 1;
    outmsg_capacity -= 1;

    unsigned i;
    for(i = 0; i < resc; ++i)
    {
        res = P_u16_set(resv[i], &outmsg[size], outmsg_capacity);
        if(res)
        {
            return -1;
        }
        size += 2;
        outmsg_capacity -= 2;
    }

    (*outmsg_size) = size;

    return 0;
}

int GRPCServer::P_encode_event(
        uint8_t eventId,
        unsigned resc,
        const uint16_t * resv,
        uint8_t *outmsg,
        unsigned outmsg_capacity,
        unsigned * outmsg_size
)
{
    int res;

    if(resc > GRPCSERVER_RESV_SIZE)
    {
        return -1;
    }

    unsigned size = 0;
    res = P_u8_set(GRPC_OUTMSG_TYPE_EVENT, &outmsg[size], outmsg_capacity);
    if(res)
    {
        return -1;
    }
    size += 1;
    outmsg_capacity -= 1;

    res = P_u8_set(eventId, &outmsg[size], outmsg_capacity);
    if(res)
    {
        return -1;
    }
    size += 1;
    outmsg_capacity -= 1;

    res = P_u8_set((uint8_t)resc, &outmsg[size], outmsg_capacity);
    if(res)
    {
        return -1;
    }
    size += 1;
    outmsg_capacity -= 1;

    unsigned i;
    for(i = 0; i < resc; ++i)
    {
        res = P_u16_set(resv[i], &outmsg[size], outmsg_capacity);
        if(res)
        {
            return -1;
        }
        size += 2;
        outmsg_capacity -= 2;
    }

    (*outmsg_size) = size;

    return 0;
}
