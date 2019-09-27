/**
 * @file exception.hpp
 *
 *  Created on: 3 дек. 2018 г.
 *      Author: mastersan
 */

#ifndef SRC_INCLUDE_NOSTD_EXCEPTION_MAIN_HPP_
#define SRC_INCLUDE_NOSTD_EXCEPTION_MAIN_HPP_

#include <nostd/fatal>

#ifndef NOSTD_ALLOW_EXCEPTIONS

namespace nostd
{

static fatal_handler thandler = nullptr;

}

void nostd::set_fatal(
        fatal_handler handler
)
{
    thandler = handler;
}

void nostd::fatal(const char * file, unsigned int line, int error)
{
    if(thandler)
    {
        thandler(file, line, error);
    }
    for(;;);
}

#endif /* NOSTD_ALLOW_EXCEPTIONS */

#endif /* SRC_INCLUDE_NOSTD_EXCEPTION_MAIN_HPP_ */
