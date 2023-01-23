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
 * @param TFixed              Container scalar type.
 * @param TFracbits             Number of bits in fractional part.
 * @param TEnclosingtype      Enclosing scalar type in mul/div operations to save the precision. Desirable, should be wider than TFixed.
 */
template <
        typename TFixed,
        unsigned char TFracbits,
        typename TEnclosingtype
        >
class FixedPoint
{
public:
    struct tag_raw_t{ explicit tag_raw_t() = default; };
    struct tag_double_t{ explicit tag_double_t() = default; };
    struct tag_int_t{ explicit tag_int_t() = default; };
    struct tag_int_frac_t{ explicit tag_int_frac_t() = default; };

    static constexpr tag_raw_t tag_raw { };
    static constexpr tag_double_t tag_double { };
    static constexpr tag_int_t tag_int { };
    static constexpr tag_int_frac_t tag_int_frac { };

    constexpr FixedPoint() noexcept
    : m_value(0)
    {
    }

    constexpr FixedPoint(const FixedPoint& other) noexcept
    : m_value(other.m_value)
    {}

    constexpr FixedPoint(TFixed val, tag_raw_t) noexcept
    : m_value(val)
    {}

    constexpr FixedPoint(double val, tag_double_t) noexcept
    : m_value(FIXED__FROM_DOUBLE(TFixed, TFracbits, val))
    {}

    /**
     * @brief Init integer part of FIXED by integer value, frac set to 0.
     */
    template<typename TInt>
    constexpr FixedPoint(TInt vint, tag_int_t) noexcept
    : m_value(FIXED__FROM_INT(TFixed, TFracbits, vint))
    {}

    template<typename TInt, typename TFrac>
    constexpr FixedPoint(TInt vint, TFrac vfrac, tag_int_frac_t) noexcept
    : m_value(FIXED__FROM_INT_FRAC(TFixed, TFracbits, vint, vfrac))
    {}

    ~FixedPoint() = default;

    /* operators: unary */
    const FixedPoint& operator+() const noexcept
    {
        return *this;
    }

    const FixedPoint operator-() const noexcept
    {
        return FixedPoint(-m_value);
    }

    /* operators: binary */
    FixedPoint& operator=(const FixedPoint& right) noexcept
    {
        m_value = right.m_value;
        return *this;
    }

    FixedPoint& operator=(TFixed right) noexcept
    {
        m_value = right;
        return *this;
    }

    FixedPoint& operator=(double right) noexcept
    {
        m_value = FIXED__FROM_DOUBLE(TFixed, TFracbits, right);
        return *this;
    }

    /* <FixedPoint> <op> <FixedPoint> */
    constexpr FixedPoint operator+(const FixedPoint& right) const noexcept
    {
        return FixedPoint(m_value + right.m_value, tag_raw);
    }

    constexpr FixedPoint operator-(const FixedPoint& right) const noexcept
    {
        return FixedPoint(m_value - right.m_value, tag_raw);
    }

    constexpr FixedPoint operator*(const FixedPoint& right) const noexcept
    {
        return FixedPoint(FIXED_MUL(TFixed, TFracbits, TEnclosingtype, m_value, right.m_value), tag_raw);
    }

    template<typename Tright_integer>
    constexpr FixedPoint operator*(const Tright_integer& right) const noexcept
    {
        return FixedPoint(FIXED_MUL_BY_INT(TFixed, m_value, right), tag_raw);
    }

    constexpr FixedPoint operator/(const FixedPoint& right) const noexcept
    {
        return FixedPoint(FIXED_DIV(TFixed, TFracbits, TEnclosingtype, m_value, right.m_value), tag_raw);
    }

    template<typename Tright_integer>
    constexpr FixedPoint operator/(const Tright_integer& right) const noexcept
    {
        return FixedPoint(FIXED_DIV_BY_INT(TFixed, m_value, right), tag_raw);
    }

    /* <FixedPoint> <op>= <FixedPoint> */
    FixedPoint& operator+=(const FixedPoint& right) noexcept
    {
        m_value += right.m_value;
        return *this;
    }

    FixedPoint& operator-=(const FixedPoint& right) noexcept
    {
        m_value -= right.m_value;
        return *this;
    }

    FixedPoint& operator*=(const FixedPoint& right) noexcept
    {
        m_value = FIXED_MUL(TFixed, TFracbits, TEnclosingtype, m_value, right.m_value);
        return *this;
    }

    FixedPoint& operator/=(const FixedPoint& right) noexcept
    {
        m_value = FIXED_DIV(TFixed, TFracbits, TEnclosingtype, m_value, right.m_value);
        return *this;
    }

    /* <FixedPoint> <eq-op> <FixedPoint> */
    constexpr bool operator==(const FixedPoint& right) const noexcept
    {
        return m_value == right.m_value;
    }

    constexpr bool operator!=(const FixedPoint& right) const noexcept
    {
        return m_value != right.m_value;
    }

    constexpr bool operator>=(const FixedPoint& right) const noexcept
    {
        return m_value >= right.m_value;
    }

    constexpr bool operator<=(const FixedPoint& right) const noexcept
    {
        return m_value <= right.m_value;
    }

    constexpr bool operator>(const FixedPoint& right) const noexcept
    {
        return m_value > right.m_value;
    }

    constexpr bool operator<(const FixedPoint& right) const noexcept
    {
        return m_value < right.m_value;
    }

    /* methods */

    void setFixed(const FixedPoint & other) noexcept
    {
        m_value = other.m_value;
    }

    void setRawFixed(TFixed val) noexcept
    {
        m_value = val;
    }

    void setDouble(double val) noexcept
    {
        m_value = FIXED__FROM_DOUBLE(TFixed, TFracbits, val);
    }

    /**
     * @brief Init integer part of FIXED by integer value, frac set to 0.
     */
    template<typename TInt>
    void setInt(TInt vint) noexcept
    {
        m_value = FIXED__FROM_INT(TFixed, TFracbits, vint);
    }

    template<typename TInt, typename TFrac>
    void setIntFrac(TInt vint, TFrac vfrac) noexcept
    {
        m_value = FIXED__FROM_INT_FRAC(TFixed, TFracbits, vint, vfrac);
    }

    TFixed toRawFixed() const noexcept
    {
        return m_value;
    }

    /** @brief Numerator */
    TFixed integer() const noexcept
    {
        return FIXED__INT(TFixed, TFracbits, m_value);
    }

    /** @brief Numerator */
    TFixed frac() const noexcept
    {
        return FIXED__FRAC(TFixed, TFracbits, m_value);
    }

    /** @brief 1 / fracunit() = minimal fractional value */
    TFixed fracunit() const noexcept
    {
        return FIXED_FRACUNIT(TFixed, TFracbits);
    }

    double toDouble() const noexcept
    {
        return FIXED__TO_DOUBLE(TFixed, TFracbits, m_value);
    }

    FixedPoint toAbs() const noexcept
    {
        return FixedPoint(FIXED_ABS(TFixed, m_value));
    }

private:
    TFixed m_value;
};

}

#endif /* NOSTD_FIXED_PRIVATE_PRIVATE_HPP_ */
