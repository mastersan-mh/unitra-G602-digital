/**
 * @file fixed16.hpp
 * @brief Export class to manipulate fixedpoint numbers with 16 (8+8) bits width
 */

#ifndef NOSTD_FIXED_FIXED16_HPP_
#define NOSTD_FIXED_FIXED16_HPP_

#include <nostd/fixed/fixed16.h>

#include <nostd/fixed/fixedpoint>

namespace nostd
{

typedef
        FixedPoint
        <
        fixed16_t,
        8,
        int32_t
        > Fixed16;

}

#endif /* NOSTD_FIXED_FIXED16_HPP_ */
