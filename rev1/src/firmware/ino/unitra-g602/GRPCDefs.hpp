/**
 * @file GRPCDefs.hpp
 * @brief Remote procedure call class defines
 */

#ifndef GRPC_HPP_INCLUDED_
#define GRPC_HPP_INCLUDED_

#define GRPC_OUTMSG_TYPE_REPLY 0x00
#define GRPC_OUTMSG_TYPE_EVENT 0x01

#define GRPC_REPLY_ERR_OK 0
#define GRPC_REPLY_ERR_INVALID_MODE 1 /* The procedure is not available in the current mode. */
#define GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT 2
#define GRPC_REPLY_ERR_OUT_OF_RANGE 3

#endif /* GRPC_HPP_INCLUDED_ */
