#ifndef NOSTD_FASTSUM_FASTSUM_H_
#define NOSTD_FASTSUM_FASTSUM_H_

#include <nostd/types>

namespace nostd
{

/**
 * @brief Fast sum
 * @note Summation occurs "on the fly".
 * @note Because error accumulation is possible for floating-point types,
 * @note you can use the FastSumCorrection class.
 * @param Tvalue_type        Type of value and sum
 * @param TCapacity         Capacity of array, items
 */
template <
typename Tvalue_type,
nostd::size_t Tcapacity
>
class FastSum
{
public:
    FastSum() = default;
    virtual ~FastSum() = default;

    void add(Tvalue_type x)
    {

        if(m_size >= Tcapacity)
        {
            Tvalue_type first_val = m_items[m_index];
            m_sum -= first_val;
        }

        m_items[m_index++] = x;

        if(m_size < Tcapacity)
        {
            ++m_size;
        }

        m_sum += x;

        if(m_index >= Tcapacity)
        {
            m_index = 0;
        }

    }

    constexpr nostd::size_t capacity() const noexcept
    {
        return Tcapacity;
    }

    nostd::size_t size() const noexcept
    {
        return m_size;
    }

    /**
     * @brief The value (sum) of the class
     */
    Tvalue_type sum() const noexcept
    {
        return m_sum;
    }

    void reset()
    {
        m_sum = Tvalue_type();
        m_size = 0;
        m_index = 0;
    }

private:
    nostd::size_t m_size = 0;
    nostd::size_t m_index = 0;
    Tvalue_type m_items[Tcapacity]{}; /**< values to sum */
    Tvalue_type m_sum{};
};


/**
 * @brief Fast sum
 * @note Since summation occurs "on the fly", when using floating point types, error accumulation is possible.
 * @note Therefore, in order to prevent the accumulation of an error,
 * @note the sum is recalculated according to the classical algorithm when the threshold of the number of summed elements is exceeded.
 * @param Tvalue_type        Type of value and sum
 * @param Tcapacity          Amount of items in array
 * @param Trecalc_num        The amount of values before the sum recalculate (0 - do not recalc). Used to avoid accumulating errors.
 */
template <
typename Tvalue_type,
nostd::size_t Tcapacity,
nostd::size_t Trecalc_num
>
class FastSumCorrection
{
public:
    FastSumCorrection() = default;
    virtual ~FastSumCorrection() = default;

    void add(Tvalue_type x)
    {
        const bool deny_recalc = (Trecalc_num == 0);

        if((deny_recalc || m_recalc_count < Trecalc_num) && m_array_full)
        {
            Tvalue_type first_val = m_sum_items[m_index];
            m_sum -= first_val;
        }

        m_sum_items[m_index++] = x;

        if(m_size < Tcapacity)
        {
            ++m_size;
        }

        if(!deny_recalc)
        {
            ++m_recalc_count;
        }

        if(deny_recalc || m_recalc_count < Trecalc_num)
        {
            m_sum += x;
        }
        else
        {
            m_recalc_count = 0;
            m_sum = Tvalue_type();
            nostd::size_t i;
            for(i = 0; i < m_size; ++i)
            {
                m_sum += m_sum_items[i];
            }
        }


        if(m_index >= Tcapacity)
        {
            m_index = 0;
            m_array_full = true;
        }

    }

    constexpr nostd::size_t capacity() const noexcept
    {
        return Tcapacity;
    }

    nostd::size_t size() const noexcept
    {
        return m_size;
    }

    inline Tvalue_type sum() const
    {
        return m_sum;
    }

    void reset()
    {
        m_size = 0;
        m_index = 0;
        m_recalc_count = 0;
        m_array_full = false;
        m_sum = Tvalue_type();
    }

private:
    nostd::size_t m_size = 0;
    nostd::size_t m_index = 0;
    nostd::size_t m_recalc_count = 0;
    bool m_array_full = false;
    Tvalue_type m_sum{};
    Tvalue_type m_sum_items[Tcapacity]{}; /**< values to sum */
};

}

#endif /* NOSTD_FASTSUM_FASTSUM_H_ */
