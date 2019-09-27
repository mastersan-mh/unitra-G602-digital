/**
 * exception.cpp
 */

#include <stdlib.h>

#include "exception.hpp"

#ifndef APP_ALLOW_EXCEPTIONS

static app::terminate_handler thandler = nullptr;

void app::set_terminate(
        terminate_handler handler
)
{
    thandler = handler;
}

void app::exception_throw_internal(const char * exception)
{
    if(thandler)
    {
        thandler();
    }
    for(;;);
}

#endif
