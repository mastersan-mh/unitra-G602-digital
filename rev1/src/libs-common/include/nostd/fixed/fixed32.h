/**
 * @file fixed32.h
 * @brief Export functions to manipulate fixedpoint numbers with 32 (16+16) bits width
 */

#ifndef NOSTD_FIXED_FIXED32_H_
#define NOSTD_FIXED_FIXED32_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <nostd/fixed/fixedpoint.h>

#include <stdint.h>

#define FIXED32_FRACBITS 16

typedef int32_t fixed32_t;

static FIXEDPOINT_INLINE fixed32_t fixed32_abs(fixed32_t x) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_ABS(fixed32_t, x);
}

static FIXEDPOINT_INLINE fixed32_t fixed32_mul(fixed32_t a, fixed32_t b) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_MUL(fixed32_t, FIXED32_FRACBITS, int64_t, a, b);
}

static FIXEDPOINT_INLINE fixed32_t fixed32_div(fixed32_t a, fixed32_t b) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_DIV(fixed32_t, FIXED32_FRACBITS, int64_t, a, b);
}

static FIXEDPOINT_INLINE fixed32_t int_to_fixed32(int val) FIXEDPOINT_C_NOEXCEPT
{
    return INT_TO_FIXED(fixed32_t, FIXED32_FRACBITS, int64_t, val);
}

static FIXEDPOINT_INLINE fixed32_t double_to_fixed32(double val) FIXEDPOINT_C_NOEXCEPT
{
    return DOUBLE_TO_FIXED(fixed32_t, FIXED32_FRACBITS, val);
}

static FIXEDPOINT_INLINE double fixed32_to_double(fixed32_t val) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_TO_DOUBLE(fixed32_t, FIXED32_FRACBITS, val);
}

#ifdef __cplusplus
}
#endif

#endif /* NOSTD_FIXED_FIXED32_H_ */
