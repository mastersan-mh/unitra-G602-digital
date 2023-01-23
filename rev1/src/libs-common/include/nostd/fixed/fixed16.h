/**
 * @file fixed16.h
 * @brief Export functions to manipulate fixedpoint numbers with 16 (8+8) bits width
 */

#ifndef NOSTD_FIXED_FIXED16_H_
#define NOSTD_FIXED_FIXED16_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <nostd/fixed/fixedpoint.h>

#include <stdint.h>

#define FIXED16_FRACBITS 8

typedef int16_t fixed16_t;

static inline fixed16_t fixed16_abs(fixed16_t x) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_ABS(fixed16_t, x);
}

static inline fixed16_t fixed16_mul(fixed16_t a, fixed16_t b) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_MUL(fixed16_t, FIXED16_FRACBITS, int32_t, a, b);
}

static inline fixed16_t fixed16_div(fixed16_t a, fixed16_t b) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED_DIV(fixed16_t, FIXED16_FRACBITS, int32_t, a, b);
}

static inline fixed16_t fixed16_from_int(uint8_t val) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED__FROM_INT(fixed16_t, FIXED16_FRACBITS, val);
}

static inline fixed16_t fixed16_from_int_frac(int8_t vint, int8_t vfrac) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED__FROM_INT_FRAC(fixed16_t, FIXED16_FRACBITS, vint, vfrac);
}

static inline fixed16_t fixed16_from_double(double val) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED__FROM_DOUBLE(fixed16_t, FIXED16_FRACBITS, val);
}

static inline double fixed16_to_double(fixed16_t val) FIXEDPOINT_C_NOEXCEPT
{
    return FIXED__TO_DOUBLE(fixed16_t, FIXED16_FRACBITS, val);
}

#ifdef __cplusplus
}
#endif

#endif /* NOSTD_FIXED_FIXED16_H_ */
