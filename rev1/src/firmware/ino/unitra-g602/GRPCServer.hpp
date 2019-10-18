/**
 * @file GRPCServer.hpp
 * @brief Remote procedure call class
 */

#ifndef GRPCSERVER_HPP_INCLUDED_
#define GRPCSERVER_HPP_INCLUDED_

#include "GRPCDefs.hpp"

#include <stdint.h>

#define GRPCSERVER_ARGV_SIZE 8
#define GRPCSERVER_RESV_SIZE 8
#define GRPCSERVER_SENDBUF_SIZE \
        ( \
        sizeof(uint8_t) + /* <type> */ \
        sizeof(uint8_t) + /* <error> / event */ \
        sizeof(uint16_t) + /* <ruid> */ \
        sizeof(uint8_t) + /* <resc> */ \
        GRPCSERVER_RESV_SIZE * sizeof(uint16_t) /* <resv> */ \
        )

class GRPCServer
{
public:
    enum class Error
    {
        OK,
        DECODE,
        ENCODE,
    };

    /**
     * @return err
     */
    typedef uint8_t func_t(
            unsigned argc,
            uint16_t * argv,
            unsigned * resc,
            uint16_t * resv,
            void * extraargs
    );

    GRPCServer() = delete;
    GRPCServer(
            void (*send)(const uint8_t * data, unsigned data_size, void * extraargs),
            void * extraargs
    );
    virtual ~GRPCServer();
    GRPCServer(const GRPCServer &) = delete;
    GRPCServer& operator=(const GRPCServer &) = delete;
    void funcs_register(const func_t *funcs[]);
    /**
     * @brief Receive the character and call the command
     */
    Error handle(const uint8_t * req, unsigned req_size);
    Error event(
        uint8_t eventId,
        unsigned resc,
        const uint16_t * resv
    );

private:

    void * m_extraargs;

    void (*m_send)(const uint8_t * data, unsigned data_size, void * extraargs);

    const func_t **m_funcs;
    unsigned m_func_num;

    uint16_t m_argv[GRPCSERVER_ARGV_SIZE];
    uint16_t m_resv[GRPCSERVER_RESV_SIZE];

    uint8_t m_buf_reply[GRPCSERVER_SENDBUF_SIZE];

    static int P_u8_get(const uint8_t * data, unsigned size, uint8_t * value);
    static int P_u16_get(const uint8_t * data, unsigned size, uint16_t * value);
    static int P_u8_set(uint8_t value, uint8_t * data, unsigned size);
    static int P_u16_set(uint16_t value, uint8_t * data, unsigned size);

    int P_decode(
            const uint8_t * data,
            unsigned size,
            uint16_t * ruid,
            uint8_t * funcid,
            uint8_t * argc,
            uint16_t * argv
    );

    int P_encode_reply(
            uint8_t err,
            uint16_t ruid,
            unsigned resc,
            const uint16_t * resv,
            uint8_t * reply,
            unsigned reply_capacity,
            unsigned * reply_size
    );

    int P_encode_event(
            uint8_t eventId,
            unsigned resc,
            const uint16_t * resv,
            uint8_t * reply,
            unsigned reply_capacity,
            unsigned * reply_size
    );

};

#endif /* GRPCSERVER_HPP_INCLUDED_ */
