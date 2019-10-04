/**
 * @file GComm.hpp
 * @brief communication class
 */

#ifndef GCOMM_HPP_INCLUDED_
#define GCOMM_HPP_INCLUDED_

class GComm
{
public:
    GComm();
    virtual ~GComm();
    GComm(const GComm &) = delete;
    GComm& operator=(const GComm &) = delete;
    void recv();
    void send();
};

#endif /* GCOMM_HPP_INCLUDED_ */
