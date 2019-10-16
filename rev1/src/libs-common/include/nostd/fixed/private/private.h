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

#define FIXED_ABS(xtype, x) \
    ( (xtype)( { xtype tmp = (x), sign = tmp >> ( 8 * sizeof(xtype) - 1); (tmp ^ sign) - sign; } ) )

#define FIXED_MUL(xtype, xfracbits, xenclosingtype, a, b) \
    ( (xtype)( (xenclosingtype)(a) * (b) >> (xfracbits) ) )

#define FIXED_DIV(xtype, xfracbits, xenclosingtype, a, b) \
    ( (xtype)( ( (xenclosingtype)(a) << (xfracbits) ) / (b) ) )

/**
 * @param xenclosingtype    Eclosing type
 */
#define INT_TO_FIXED(xtype, xfracbits, xenclosingtype, xval) \
    ( (xtype)((xenclosingtype)(xval) << (xfracbits)) )

#define FIXED_TO_DOUBLE(xtype, xfracbits, xval) \
    ( (double)(xval) / FIXED_FRACUNIT(xtype, xfracbits) )

#define DOUBLE_TO_FIXED(xtype, xfracbits, xval) \
    ( (xtype)( (xval) * FIXED_FRACUNIT(xtype, xfracbits) ) )

#endif /* NOSTD_FIXED_PRIVATE_PRIVATE_H_ */
