#ifndef NOSTD_FIXEDPOINT_FIXED32_H_
#define NOSTD_FIXEDPOINT_FIXED32_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <fixed/private/private.h>
#include <stdint.h>

#define FIXED32_BITS 32
#define FIXED32_FRACBITS 16
#define FIXED32_FRACUNIT ((int32_t)1 << FIXED32_FRACBITS)

typedef int32_t fixed32_t;

static inline fixed32_t fixed32_abs(fixed32_t x)
{
    fixed32_t sign = x >> (FIXED32_BITS - 1);
    return (x ^ sign) - sign;
}

static inline fixed32_t fixed32_mul(fixed32_t a, fixed32_t b)
{
  return (fixed32_t)((int64_t)a * b >> FIXED32_FRACBITS);
}

static inline fixed32_t fixed32_div(fixed32_t a, fixed32_t b)
{
  return (fixed32_t)( ((int64_t)a << FIXED32_FRACBITS) / b );
}

inline static fixed32_t FixedDiv(fixed32_t a, fixed32_t b, bool * overflow)
{
    if( ( (unsigned)fixed32_abs(a)>>14) >= (unsigned)fixed32_abs(b) )
    {
        *overflow = true;
        return ((a^b)>>31) ^ INT32_MAX;
    }
    else
    {
        *overflow = false;
        return (fixed32_t)((int64_t) a << FIXED32_FRACBITS) / b;
    }
/*
    return ((unsigned)fixed32_abs(a)>>14) >= (unsigned)fixed32_abs(b) ?
            ((a^b)>>31) ^ INT32_MAX :
            (fixed32_t)(((int64_t) a << FIXED32_FRACBITS) / b);
 */
}

static inline fixed32_t fixed32_div_o(fixed32_t a, fixed32_t b, bool * overflow)
{
    int64_t res = ((int64_t)a << FIXED32_FRACBITS) / b;
    *overflow = (abs(res) > (((int64_t)1 << (FIXED32_BITS + 1)) - 1));
    return (fixed32_t)( res );
}

/*
inline static fixed32_t FixedDiv(fixed32_t a, fixed32_t b)
{
    return ((unsigned)fixed32_abs(a)>>14) >= (unsigned)fixed32_abs(b) ?
                ((a^b)>>31) ^ INT_MAX :
                (fixed32_t)(((int64_t) a << FRACBITS) / b);
}
*/




static inline fixed32_t double_to_fixed32(double val)
{
    return (val * FIXED32_FRACUNIT);
}

static inline double fixed32_to_double(fixed32_t val)
{
    return ((double)val / FIXED32_FRACUNIT);
}

#ifdef __cplusplus
}
#endif

#endif /* NOSTD_FIXEDPOINT_FIXED32_H_ */
