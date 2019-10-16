
#include "GCommBase.hpp"

//#define SHOW_CRC
#ifdef SHOW_CRC
#   include "config.hpp"
#endif

#include <string.h>

#define CRC_SIZE 2 /* CRC size, bytes */

#define min(a,b) ((a)<(b)?(a):(b))

#define BUILD_16(hi, lo) \
        ((uint16_t)(((uint16_t)(hi) << 8)| (uint16_t)(lo)))

struct crc_ctx
{
    uint16_t reg_crc;
};

static void P_crc_16_ibm_reversive_init(struct crc_ctx * ctx)
{
    ctx->reg_crc = 0xFFFF;
}

static void P_crc_16_ibm_reversive_byte_add(struct crc_ctx * ctx, uint8_t byte)
{
    uint16_t tmp = (uint16_t)byte;
    ctx->reg_crc ^= tmp;
    int i;
    for(i = 0; i < 8; i++)
    {
        if(ctx->reg_crc & 0x01)
        {
            ctx->reg_crc = (uint16_t)((ctx->reg_crc >> 1) ^ 0xA001);
        }
        else
        {
            ctx->reg_crc = (uint16_t)(ctx->reg_crc >> 1);
        }
    }
}

static uint16_t P_crc_16_ibm_reversive_done(struct crc_ctx * ctx)
{
    return ctx->reg_crc;
}

/** @brief Is terminator character */
static bool P_char_is_terminal(char ch)
{
    return (ch == '\n' || ch == '\r');
}

int P_char_to_digit(char ch, uint8_t * half)
{
    uint8_t digit;
    if('0' <= ch && ch <= '9')
    {
        digit = (uint8_t)(ch - '0');
    }
    else if('A' <= ch && ch <= 'F')
    {
        digit = (uint8_t)(10 + ch - 'A');
    }
    else if('a' <= ch && ch <= 'f')
    {
        digit = (uint8_t)(10 + ch - 'a');
    }
    else
    {
        return -1;
    }
    (*half) = digit;
    return 0;
}

/**
 * @note 0x0 -> '0'
 * @note 0xA -> 'a'
 */
static uint8_t P_digit_to_char(uint8_t value)
{
    if(value <= 0x9)
    {
        return (uint8_t)(value + '0');
    }
    if(0xa <= value && value <= 0xf)
    {
        return (uint8_t)(value - 0xa + 'a');
    }
    return '*';
}

GCommBase::GCommBase()
: m_state(State::S0_AWAIT)
, m_buf()
, m_buf_size()
{
}

GCommBase::~GCommBase()
{

}

GCommBase::Error GCommBase::readFrame(uint8_t * data, unsigned capacity, unsigned * size)
{
    int avail;
    while((avail = bytesRawAvailable()) > 0)
    {
        int ch = byteReadRaw();
        fsmResult res = P_fsm((char)ch);
        switch(res)
        {
            case fsmResult::SUCCESS:
            {
                unsigned framesize = m_buf_size - CRC_SIZE;
                unsigned sz = min(capacity, framesize);
                memcpy(data, m_buf, sz);
                (*size) = sz;
                return ( capacity >= framesize ? Error::OK : Error::BUFFER_TOO_SMALL );
            }
            case fsmResult::NEXT:
            {
                break;
            }
            case fsmResult::FAILED:
            {
                return Error::FAILED;
            }
        }
    }

    return Error::NOT_READY;
}

void GCommBase::writeFrame(const uint8_t * data, unsigned size)
{
    byteWriteRaw(':');

    struct crc_ctx ctx;
    P_crc_16_ibm_reversive_init(&ctx);
    unsigned i;
    for(i = 0; i < size; ++i)
    {
        uint8_t part = data[i];
        P_crc_16_ibm_reversive_byte_add(&ctx, part);
        P_writeByte(part);
    }
    uint16_t crc = P_crc_16_ibm_reversive_done(&ctx);
    uint8_t hi = (uint8_t)(crc >> 8);
    uint8_t lo = (uint8_t)(crc & 0x00ff);
    P_writeByte(hi);
    P_writeByte(lo);

    byteWriteRaw('\n');
}

GCommBase::fsmResult GCommBase::P_fsm(char ch)
{
    fsmResult fsmRes = fsmResult::NEXT;
    int res;

    State next_state = m_state;
    switch(m_state)
    {
        case State::S0_AWAIT:
        {
            if(ch == ':')
            {
                m_buf_size = 0;
                next_state = State::S1_GET_HI;
            }
            else
            {
                fsmRes = fsmResult::FAILED;
            }
            break;
        }
        case State::S1_GET_HI:
        {
            if(P_char_is_terminal(ch))
            {
                if(!P_crc_check(m_buf, m_buf_size))
                {
                    m_buf_size = 0;
                    next_state = State::S0_AWAIT;
                    fsmRes = fsmResult::FAILED;
                }
                else
                {
                    next_state = State::S0_AWAIT;
                    fsmRes = fsmResult::SUCCESS;
                }
            }
            else
            {
                if(m_buf_size >= GCOMMBASE_BUF_SIZE)
                {
                    m_buf_size = 0;
                    next_state = State::S0_AWAIT;
                    fsmRes = fsmResult::FAILED;
                    break;
                }

                uint8_t half;
                res = P_char_to_digit(ch, &half);
                if(res)
                {
                    m_buf_size = 0;
                    next_state = State::S0_AWAIT;
                    fsmRes = fsmResult::FAILED;
                }
                else
                {
                    m_buf[m_buf_size] = (uint8_t)(half << 4);
                    next_state = State::S2_GET_LO;
                }
            }
            break;
        }
        case State::S2_GET_LO:
        {
            uint8_t half;
            res = P_char_to_digit(ch, &half);
            if(res)
            {
                m_buf_size = 0;
                next_state = State::S0_AWAIT;
                fsmRes = fsmResult::FAILED;
            }
            else
            {
                m_buf[m_buf_size] = m_buf[m_buf_size] | half;
                ++m_buf_size;
                next_state = State::S1_GET_HI;
            }
            break;
        }
    }

    m_state = next_state;
    return fsmRes;
}

void GCommBase::P_writeByte(uint8_t value)
{
    uint8_t hi4 = (uint8_t)(value >> 4);
    uint8_t lo4 = (uint8_t)(value & 0x0F);
    byteWriteRaw(P_digit_to_char(hi4));
    byteWriteRaw(P_digit_to_char(lo4));
}

bool GCommBase::P_crc_check(const uint8_t * data, unsigned size)
{
    if(size <= CRC_SIZE) return false;

    uint16_t crc = BUILD_16(data[size - 2], data[size - 1]);

    int rest = size - CRC_SIZE;
    struct crc_ctx ctx;
    P_crc_16_ibm_reversive_init(&ctx);
    while((rest--) > 0)
    {
        P_crc_16_ibm_reversive_byte_add(&ctx, *(data++));
    }
    uint16_t crc_expected = P_crc_16_ibm_reversive_done(&ctx);

#ifdef SHOW_CRC
    DEBUG_PRINT("crc = ");
    DEBUG_PRINTLN(crc);

    DEBUG_PRINT("crc_expected = ");
    DEBUG_PRINTLN(crc_expected);

    DEBUG_PRINT("eq? = ");
    DEBUG_PRINTLN(crc_expected == crc);
#endif

    return (crc_expected == crc);
}
