
#include "GRPC.hpp"

#include "utils.hpp"

#include "config.hpp"

GRPC::GRPC(
        void (*send)(const uint8_t * data, unsigned data_size, void * extraargs),
        void * extraargs
)
: m_extraargs(extraargs)
, m_send(send)
, m_procedures()
, m_func_num()
, m_argv()
, m_resv()
{
}

GRPC::~GRPC()
{

}

void GRPC::procedures_register(const procedure_t procs[])
{
    m_procedures = procs;
    m_func_num = 0;
    while(m_procedures[m_func_num] != nullptr) m_func_num++;
}

int GRPC::P_u8_get(const uint8_t * data, unsigned size, uint8_t * value)
{
    if(size < 1) return -1;
    (*value) = (data[0]);
    return 0;
}

int GRPC::P_u16_get(const uint8_t * data, unsigned size, uint16_t * value)
{
    if(size < 2) return -1;
    (*value) = (data[1] | (data[0] << 8));
    return 0;
}

int GRPC::P_u8_set(uint8_t value, uint8_t * data, unsigned size)
{
    if(size < 1) return -1;
    data[0] = value;
    return 0;
}

int GRPC::P_u16_set(uint16_t value, uint8_t * data, unsigned size)
{
    if(size < 2) return -1;
    data[0] = (value >> 8);
    data[1] = (value & 0x00ff);
    return 0;
}

GRPC::Error GRPC::handle(const uint8_t * data, unsigned size)
{

    int res;

    uint16_t ruid;
    uint8_t procid;
    uint8_t argc;
    unsigned resc = 0;

    res = P_decode(
            data,
            size,
            &ruid,
            &procid,
            &argc,
            m_argv
    );

    if(res)
    {
        return Error::DECODE;
    }

    procedure_t proc = m_procedures[procid];
    proc(ruid, argc, m_argv, &resc, m_resv, m_extraargs);

    uint8_t reply[GRPC_SENDBUF_SIZE];
    unsigned reply_size;

    res = P_encode(
            ruid,
            GRPC_ERR_OK,
            resc,
            m_resv,
            reply,
            GRPC_SENDBUF_SIZE,
            &reply_size
    );

    if(res)
    {
        return Error::ENCODE;
    }

    m_send(reply, reply_size, m_extraargs);

    return Error::OK;
}

GRPC::Error GRPC::send(
    uint16_t ruid,
    unsigned resc,
    const uint16_t * resv
)
{
    uint8_t reply[GRPC_SENDBUF_SIZE];
    unsigned reply_size;

    int res = P_encode(
            ruid,
            GRPC_ERR_OK,
            resc,
            resv,
            reply,
            GRPC_SENDBUF_SIZE,
            &reply_size
    );

    if(res)
    {
        return Error::ENCODE;
    }

    m_send(reply, reply_size, m_extraargs);
    return Error::OK;
}

int GRPC::P_decode(
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

    if((*argc) > GRPC_ARGV_SIZE)
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


int GRPC::P_encode(
        uint16_t ruid,
        uint8_t err,
        unsigned resc,
        const uint16_t * resv,
        uint8_t *reply,
        unsigned reply_capacity,
        unsigned * reply_size
)
{
    int res;

    unsigned size = 0;

    res = P_u16_set(ruid, reply, reply_capacity);
    if(res)
    {
        return -1;
    }
    reply += 2;
    size += 2;

    res = P_u8_set(err, reply, reply_capacity);
    if(res)
    {
        return -1;
    }
    reply += 1;
    size += 1;

    res = P_u8_set(resc, reply, reply_capacity);
    if(res)
    {
        return -1;
    }
    reply += 1;
    size += 1;

    unsigned i;
    for(i = 0; i < resc; ++i)
    {
        res = P_u16_set(resv[i], reply, reply_capacity);
        if(res)
        {
            return -1;
        }
        reply += 2;
        size += 2;
    }

    (*reply_size) = size;

    return 0;
}
