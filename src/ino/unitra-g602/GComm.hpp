/**
 * @file GComm.hpp
 * @brief Communication class
 */

#ifndef GCOMM_HPP_INCLUDED_
#define GCOMM_HPP_INCLUDED_

#define GCOMM_BUF_SIZE 32

#include "GCommBase.hpp"

#include <stdint.h>

class GComm: public GCommBase
{
public:

    GComm();
    virtual ~GComm();
    GComm(const GComm &) = delete;
    GComm& operator=(const GComm &) = delete;

private:

    virtual unsigned bytesRawAvailable();
    virtual int  byteReadRaw();
    virtual void byteWriteRaw(uint8_t byte);

};

#endif /* GCOMM_HPP_INCLUDED_ */
