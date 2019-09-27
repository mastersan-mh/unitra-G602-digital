/**
 * @file private.hpp
 */

#ifndef NOSTD_FIXED_PRIVATE_PRIVATE_HPP_
#define NOSTD_FIXED_PRIVATE_PRIVATE_HPP_

#include <nostd/fixed/private/common.h>
#include <nostd/fixed/private/private.h>

namespace nostd
{

/**
 * @param Tfixed_t              Container scalar type.
 * @param Tfracbits             Number of bits in fractional part.
 * @param Tenclosingtype_t      Enclosing scalar type in mul/div operations to save the precision. Desirable, should be wider than Tfixed_t.
 */
template <
        typename Tfixed_t,
        unsigned char Tfracbits,
        typename Tenclosingtype_t
        >
class FixedPoint
{
public:
    FIXEDPOINT_INLINE FixedPoint() noexcept : m_value(0)
    {
    }

    FIXEDPOINT_INLINE FixedPoint(const FixedPoint & other) noexcept : m_value(other.m_value)
    {
    }

    FIXEDPOINT_INLINE explicit FixedPoint(Tfixed_t val) noexcept : m_value(val)
    {
    }

    FIXEDPOINT_INLINE explicit FixedPoint(double val) noexcept : m_value(DOUBLE_TO_FIXED(Tfixed_t, Tfracbits, val))
    {
    }

    FIXEDPOINT_INLINE ~FixedPoint() noexcept {}

    /* operators: unary */
    FIXEDPOINT_INLINE const FixedPoint& operator+() const noexcept
    {
        return *this;
    }

    FIXEDPOINT_INLINE const FixedPoint operator-() const noexcept
    {
        return FixedPoint(-m_value);
    }

    /* operators: binary */
    FIXEDPOINT_INLINE FixedPoint& operator=(const FixedPoint& right) noexcept
    {
        m_value = right.m_value;
        return *this;
    }

    FIXEDPOINT_INLINE FixedPoint& operator=(Tfixed_t right) noexcept
    {
        m_value = right;
        return *this;
    }

    FIXEDPOINT_INLINE FixedPoint& operator=(double right) noexcept
    {
        m_value = DOUBLE_TO_FIXED(Tfixed_t, Tfracbits, right);
        return *this;
    }

    /* <FixedPoint> <op> <FixedPoint> */
    FIXEDPOINT_INLINE FixedPoint operator+(const FixedPoint& right) const noexcept
    {
        return FixedPoint(m_value + right.m_value);
    }

    FIXEDPOINT_INLINE FixedPoint operator-(const FixedPoint& right) const noexcept
    {
        return FixedPoint(m_value - right.m_value);
    }

    FIXEDPOINT_INLINE FixedPoint operator*(const FixedPoint& right) const noexcept
    {
        return FixedPoint(FIXED_MUL(Tfixed_t, Tfracbits, Tenclosingtype_t, m_value, right.m_value));
    }

    FIXEDPOINT_INLINE FixedPoint operator/(const FixedPoint& right) const noexcept
    {
        return FixedPoint(FIXED_DIV(Tfixed_t, Tfracbits, Tenclosingtype_t, m_value, right.m_value));
    }

    /* <FixedPoint> <op>= <FixedPoint> */
    FIXEDPOINT_INLINE FixedPoint& operator+=(const FixedPoint& right) noexcept
    {
        m_value += right.m_value;
        return *this;
    }

    FIXEDPOINT_INLINE FixedPoint& operator-=(const FixedPoint& right) noexcept
    {
        m_value -= right.m_value;
        return *this;
    }

    FIXEDPOINT_INLINE FixedPoint& operator*=(const FixedPoint& right) noexcept
    {
        m_value = FIXED_MUL(Tfixed_t, Tfracbits, Tenclosingtype_t, m_value, right.m_value);
        return *this;
    }

    FIXEDPOINT_INLINE FixedPoint& operator/=(const FixedPoint& right) noexcept
    {
        m_value = FIXED_DIV(Tfixed_t, Tfracbits, Tenclosingtype_t, m_value, right.m_value);
        return *this;
    }

    /* <FixedPoint> <eq-op> <FixedPoint> */
    FIXEDPOINT_INLINE bool operator==(const FixedPoint& right) const noexcept
    {
        return m_value == right.m_value;
    }

    FIXEDPOINT_INLINE bool operator!=(const FixedPoint& right) const noexcept
    {
        return m_value != right.m_value;
    }

    FIXEDPOINT_INLINE bool operator>=(const FixedPoint& right) const noexcept
    {
        return m_value >= right.m_value;
    }

    FIXEDPOINT_INLINE bool operator<=(const FixedPoint& right) const noexcept
    {
        return m_value <= right.m_value;
    }

    FIXEDPOINT_INLINE bool operator>(const FixedPoint& right) const noexcept
    {
        return m_value > right.m_value;
    }

    FIXEDPOINT_INLINE bool operator<(const FixedPoint& right) const noexcept
    {
        return m_value < right.m_value;
    }

    /* methods */
    FIXEDPOINT_INLINE void setRawFixed(Tfixed_t val) noexcept
    {
        m_value = val;
    }

    FIXEDPOINT_INLINE void set(const FixedPoint & other) noexcept
    {
        m_value = other.m_value;
    }

    FIXEDPOINT_INLINE void set(double val) noexcept
    {
        m_value = DOUBLE_TO_FIXED(Tfixed_t, Tfracbits, val);
    }

    FIXEDPOINT_INLINE void set(int val)
    {
        m_value = INT_TO_FIXED(Tfixed_t, Tfracbits, Tenclosingtype_t, val);
    }

    FIXEDPOINT_INLINE Tfixed_t toRawFixed() const noexcept
    {
        return m_value;
    }

    FIXEDPOINT_INLINE double toDouble() const noexcept
    {
        return FIXED_TO_DOUBLE(Tfixed_t, Tfracbits, m_value);
    }

    FIXEDPOINT_INLINE FixedPoint toAbs() const noexcept
    {
        return FixedPoint(FIXED_ABS(Tfixed_t, m_value));
    }

private:
    Tfixed_t m_value;
};

}

#endif /* NOSTD_FIXED_PRIVATE_PRIVATE_HPP_ */
