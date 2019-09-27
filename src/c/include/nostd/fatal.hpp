/**
 * @file exception.hpp
 */

#ifndef SRC_INCLUDE_NOSTD_EXCEPTION_HPP_
#define SRC_INCLUDE_NOSTD_EXCEPTION_HPP_

#ifdef __cplusplus

#ifdef __GNUC__
#   ifdef __EXCEPTIONS
#       define NOSTD_ALLOW_EXCEPTIONS
#   endif
#else
#   error Not the GNU C++ compiler
#endif

#ifdef NOSTD_ALLOW_EXCEPTIONS
#else

namespace nostd
{

typedef void (*fatal_handler)(const char * file, unsigned int line, int error);

void set_fatal(
        fatal_handler handler
);

void fatal(const char * file, unsigned int line, int error);

}

#   define FATAL(error) nostd::fatal(__FILE__, __LINE__, error)

#   define NOSTD_ERR_OUT_OF_RANGE 1

#endif

#endif /* __cplusplus */

#endif /* SRC_INCLUDE_NOSTD_EXCEPTION_HPP_ */
