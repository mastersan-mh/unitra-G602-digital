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
 * @param value_type        Type of value and sum
 * @param sum_items_num     Amount of items in array
 */
template <
class value_type,
nostd::size_t sum_items_num
>
class FastSum
{
public:
    FastSum()
    {
        reset();
    }

    virtual ~FastSum() noexcept
    {

    }

    void add(value_type x)
    {

        if(m_len >= sum_items_num)
        {
            value_type first_val = m_items[m_current];
            m_sum -= first_val;
        }

        m_items[m_current++] = x;

        if(m_len < sum_items_num)
        {
            ++m_len;
        }

        m_sum += x;

        if(m_current >= sum_items_num)
        {
            m_current = 0;
        }

    }

    inline nostd::size_t length()
    {
        return m_len;
    }

    /**
     * @brief The value (sum) of the class
     */
    inline value_type value()
    {
        return m_sum;
    }

    void reset()
    {
        m_sum = value_type();
        m_len = 0;
        m_current = 0;
    }

private:
    /* values to sum */
    value_type m_items[sum_items_num];
    value_type m_sum;
    nostd::size_t m_len;
    nostd::size_t m_current;
};


/**
 * @brief Fast sum
 * @note Since summation occurs "on the fly", when using floating point types, error accumulation is possible.
 * @note Therefore, in order to prevent the accumulation of an error,
 * @note the sum is recalculated according to the classical algorithm when the threshold of the number of summed elements is exceeded.
 * @param value_type        Type of value and sum
 * @param sum_items_num     Amount of items in array
 * @param recalc_num        The amount of values before the sum recalculate (0 - do not recalc). Used to avoid accumulating errors.
 */
template <
class value_type,
nostd::size_t sum_items_num,
nostd::size_t recalc_num
>
class FastSumCorrection
{
public:
    FastSumCorrection()
    {
        reset();
    }

    virtual ~FastSumCorrection() noexcept
    {

    }

    void add(value_type x)
    {
        bool deny_recalc = (recalc_num == 0);

        if((deny_recalc || recalc_count < recalc_num) && array_full)
        {
            value_type first_val = sum_items[sum_current];
            sum -= first_val;
        }

        sum_items[sum_current++] = x;

        if(sum_len < sum_items_num)
        {
            ++sum_len;
        }


        if(!deny_recalc)
        {
            ++recalc_count;
        }

        if(deny_recalc || recalc_count < recalc_num)
        {
            sum += x;
        }
        else
        {
            recalc_count = 0;
            sum = value_type();
            nostd::size_t i;
            for(i = 0; i < sum_len; ++i)
            {
                sum += sum_items[i];
            }
        }


        if(sum_current >= sum_items_num)
        {
            sum_current = 0;
            array_full = true;
        }

    }

    inline nostd::size_t length() const
    {
        return sum_len;
    }

    inline value_type value() const
    {
        return sum;
    }

    void reset()
    {
        sum = value_type();
        sum_len = 0;
        sum_current = 0;
        recalc_count = 0;
        array_full = false;
    }

private:
    /* values to sum */
    value_type sum_items[sum_items_num];
    value_type sum;
    nostd::size_t sum_len;
    nostd::size_t sum_current;
    nostd::size_t recalc_count;
    bool array_full;
};

}

#endif /* NOSTD_FASTSUM_FASTSUM_H_ */
