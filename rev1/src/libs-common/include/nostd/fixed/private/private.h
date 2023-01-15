/**
 * @file private.h
 */

#ifndef NOSTD_FIXED_PRIVATE_PRIVATE_H_
#define NOSTD_FIXED_PRIVATE_PRIVATE_H_

#include <nostd/fixed/private/common.h>

/**
 * @param xtype                 Container scalar type.
 * @param xfracbits             Number of bits in fractional part.
 * @param xenclosingtype        Enclosing scalar type in mul/div operations to save the precision. Desirable, should be wider than Tfixed_t.
 */

#define FIXED_FRACUNIT(xtype, xfracbits) \
    ( (xtype)1 << (xfracbits) )

#define FIXED_FRACMASK(xtype, xfracbits) \
    ( FIXED_FRACUNIT(xtype, xfracbits) - 1 )

#define FIXED_ABS(xtype, x) \
    ( (xtype)( { xtype tmp = (x), sign = tmp >> ( 8 * sizeof(xtype) - 1); (tmp ^ sign) - sign; } ) )

/**
 * @param xenclosingtype        Enclosing scalar type in mul/div operations to save the precision. Desirable, should be wider than Tfixed_t.
 */
#define FIXED_MUL(xtype, xfracbits, xenclosingtype, a, b) \
    ( (xtype)( (xenclosingtype)(a) * (b) >> (xfracbits) ) )

/**
 * @brief Multiplication by integer value
 */
#define FIXED_MUL_BY_INT(xtype, a, b_integer) \
    ( (xtype)( (a) * (b_integer) ) )

#define FIXED_DIV(xtype, xfracbits, xenclosingtype, a, b) \
    ( (xtype)( ( ((xenclosingtype)(a)) << (xfracbits) ) / (b) ) )

/**
 * @brief Division by integer value
 */
#define FIXED_DIV_BY_INT(xtype, a, b_integer) \
    ( (xtype)( (a) / (b_integer) ) )

/**
 * @brief Init FIXED value by integer part.
 * @param xtype    Fixed-pointer type
 */
#define FIXED__FROM_INT(xtype, xfracbits, xv_int) \
    ( (xtype) ( (xtype)(xv_int) << (xfracbits) ) )

/**
 * @brief Init FIXED value by integer and frac parts.
 * @param xv_numerator    is a numerator of fraction  ( xv_numerator / FIXED_FRACUNIT() )
 * @note Value = ( xv_int + xv_numerator / FIXED_FRACUNIT() )
 */
#define FIXED__FROM_INT_FRAC(xtype, xfracbits, xv_int, xv_numerator) \
        ( \
                FIXED__FROM_INT(xtype, xfracbits, xv_int) | \
                ( FIXED_FRACMASK(xtype, xfracbits) & (xv_numerator) ) \
        )

/** @brief Integer part */
#define FIXED__INT(xtype, xfracbits, xval) \
        ((xtype)(xval) >> (xfracbits))

/** @brief Fractional part, numerator */
#define FIXED__FRAC(xtype, xfracbits, xval) \
        ((xtype)(xval) & FIXED_FRACMASK(xtype, xfracbits))

#define FIXED__TO_DOUBLE(xtype, xfracbits, xval) \
    ( (double)(xval) / FIXED_FRACUNIT(xtype, xfracbits) )

/**
 * @brief Init FIXED value by double.
 */
#define FIXED__FROM_DOUBLE(xtype, xfracbits, xval) \
    ( (xtype)( (xval) * FIXED_FRACUNIT(xtype, xfracbits) ) )

#endif /* NOSTD_FIXED_PRIVATE_PRIVATE_H_ */
