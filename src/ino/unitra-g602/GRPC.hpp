/**
 * @file GRPC.hpp
 * @brief Remote procedure call class
 */

#ifndef GRPC_HPP_INCLUDED_
#define GRPC_HPP_INCLUDED_

#include <stdint.h>

#define GRPC_ERR_OK 0
#define GRPC_ERR_PARSING 1
#define GRPC_ERR_INVALID_MODE 2 /* The procedure is not available in the current mode. */
#define GRPC_ERR_INVALID_ARGUMENTS 3
#define GRPC_ERR_OUT_OF_RANGE 4

#define GRPC_ARGV_SIZE 8
#define GRPC_RESV_SIZE 8
#define GRPC_SENDBUF_SIZE \
        ( \
                sizeof(uint16_t) + /* <ruid> */ \
        sizeof(uint8_t) + /* <error> */ \
        sizeof(uint16_t) + /* <resc> */ \
        GRPC_RESV_SIZE * sizeof(uint16_t) /* <resv> */ \
        )

class GRPC
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
    typedef uint8_t (*procedure_t)(
            uint16_t ruid,
            unsigned argc,
            uint16_t * argv,
            unsigned * resc,
            uint16_t * resv,
            void * extraargs
    );

    GRPC() = delete;
    GRPC(
            void (*send)(const uint8_t * data, unsigned data_size, void * extraargs),
            void * extraargs
    );
    virtual ~GRPC();
    GRPC(const GRPC &) = delete;
    GRPC& operator=(const GRPC &) = delete;
    void procedures_register(const procedure_t procs[]);
    /**
     * @brief Receive the character and call the command
     */
    Error handle(const uint8_t * data, unsigned size);
    Error send(
        uint16_t ruid,
        unsigned resc,
        const uint16_t * resv
    );

private:

    void * m_extraargs;

    void (*m_send)(const uint8_t * data, unsigned data_size, void * extraargs);

    const procedure_t *m_procedures;
    unsigned m_func_num;

    uint16_t m_argv[GRPC_ARGV_SIZE];
    uint16_t m_resv[GRPC_RESV_SIZE];

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

    int P_encode(
            uint16_t ruid,
            uint8_t err,
            unsigned resc,
            const uint16_t * resv,
            uint8_t * reply,
            unsigned reply_capacity,
            unsigned * reply_size
    );

};

#endif /* GRPC_HPP_INCLUDED_ */
