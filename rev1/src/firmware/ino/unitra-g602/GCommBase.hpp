/**
 * @file GCommBase.hpp
 * @brief Base communication class
 */

#ifndef GCOMMBASE_HPP_INCLUDED_
#define GCOMMBASE_HPP_INCLUDED_

#define GCOMMBASE_BUF_SIZE 32

#include <stdint.h>

class GCommBase
{
public:

    enum class Error
    {
        OK, /* frame is ready */
        BUFFER_TOO_SMALL, /* buffer too small to store entire frame, only part stored */
        NOT_READY, /* frame is not ready, only part readed */
        FAILED, /* error while frame reading */
    };

    GCommBase(const GCommBase &) = delete;
    GCommBase& operator=(const GCommBase &) = delete;

    GCommBase() = default;
    virtual ~GCommBase() = default;

    /**
     * @brief Read entire frame
     * @return Frame is ready?
     */
    Error readFrame(uint8_t * data, unsigned capacity, unsigned * size);
    void writeFrame(const uint8_t * data, unsigned size);


private:

    virtual unsigned bytesRawAvailable() = 0;
    virtual int  byteReadRaw() = 0;
    virtual void byteWriteRaw(uint8_t byte) = 0;

    enum class State
    {
        S0_AWAIT,
        S1_GET_HI,
        S2_GET_LO,
    };

    enum class fsmResult
    {
        SUCCESS,
        NEXT,
        FAILED,
    };

    State m_state = State::S0_AWAIT;

    uint8_t m_buf[GCOMMBASE_BUF_SIZE]{}; /** temporary frame buffer */
    unsigned m_buf_size = 0;

    fsmResult P_fsm(char ch);

    void P_writeByte(uint8_t value);

    static bool P_crc_check(const uint8_t * data, unsigned size);

};

#endif /* GCOMMBASE_HPP_INCLUDED_ */
