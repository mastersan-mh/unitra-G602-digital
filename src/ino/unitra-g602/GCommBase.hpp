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

    enum class State
    {
        S0_AWAIT,
        S1_GET_HI,
        S2_GET_LO,
    };

    GCommBase();
    virtual ~GCommBase();
    GCommBase(const GCommBase &) = delete;
    GCommBase& operator=(const GCommBase &) = delete;

    unsigned readFrame(uint8_t * data, unsigned capacity, unsigned * rest);
    void writeFrame(const uint8_t * data, unsigned size);


private:

    virtual unsigned bytesRawAvailable() = 0;
    virtual int  byteReadRaw() = 0;
    virtual void byteWriteRaw(uint8_t byte) = 0;

    enum class fsmResult
    {
        SUCCESS,
        NEXT,
        FAILED,
    };

    State m_state;

    uint8_t m_buf[GCOMMBASE_BUF_SIZE]; /** temporary frame buffer */
    unsigned m_buf_size;

    fsmResult P_fsm(char ch);

    void P_writeByte(uint8_t value);

    static bool P_crc_check(const uint8_t * data, unsigned size);

};

#endif /* GCOMMBASE_HPP_INCLUDED_ */
