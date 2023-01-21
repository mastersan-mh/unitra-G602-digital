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

static inline fixed32_t fixed32_abs(fixed32_t x) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_ABS(fixed32_t, x);
}

static inline fixed32_t fixed32_mul(fixed32_t a, fixed32_t b) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_MUL(fixed32_t, FIXED32_FRACBITS, int64_t, a, b);
}

static inline fixed32_t fixed32_div(fixed32_t a, fixed32_t b) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_DIV(fixed32_t, FIXED32_FRACBITS, int64_t, a, b);
}

static inline fixed32_t fixed32_from_int(int16_t val) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED__FROM_INT(fixed32_t, FIXED32_FRACBITS, val);
}

static inline fixed32_t fixed32_from_int_frac(int16_t vint, int16_t vfrac) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED__FROM_INT_FRAC(fixed32_t, FIXED32_FRACBITS, vint, vfrac);
}

static inline fixed32_t fixed32_from_double(double val) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED__FROM_DOUBLE(fixed32_t, FIXED32_FRACBITS, val);
}

static inline double fixed32_to_double(fixed32_t val) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED__TO_DOUBLE(fixed32_t, FIXED32_FRACBITS, val);
}

#ifdef __cplusplus
}
#endif

#endif /* NOSTD_FIXED_FIXED32_H_ */
