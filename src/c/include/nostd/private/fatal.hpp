/**
 * @file fatal.hpp
 *
 */

#ifndef NOSTD_PRIVATE_FATAL_HPP_INCLUDED_
#define NOSTD_PRIVATE_FATAL_HPP_INCLUDED_

#include <nostd/fatal>

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

#endif /* NOSTD_PRIVATE_FATAL_HPP_INCLUDED_ */
