/*
 * exception.h
 *
 *  Created on: 3 нояб. 2018 г.
 *      Author: mastersan
 */

#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#ifdef __GNUC__
#   ifdef __EXCEPTIONS
#       define APP_ALLOW_EXCEPTIONS
#   endif
#endif



#ifdef APP_ALLOW_EXCEPTIONS
#   define THROW(x) throw(x)
#else

namespace app
{

typedef void (*terminate_handler) ();

void set_terminate(
        terminate_handler handler
);

void exception_throw_internal(const char * exception);
} /* namespace app */

#   define TRY
#   define CATCH
#   define THROW(x) app::exception_throw_internal(#x)
#endif


#endif /* EXCEPTION_HPP_ */
