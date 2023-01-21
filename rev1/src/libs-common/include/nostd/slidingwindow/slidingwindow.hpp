#ifndef NOSTD_SLIDINGWINDOW_SLIDINGWINDOW_H_
#define NOSTD_SLIDINGWINDOW_SLIDINGWINDOW_H_

#include <nostd/fatal>
#include <nostd/types>

namespace nostd
{

/**
 * Window of items
 */
template <
         class value_type,
         nostd::size_t window_size
         >
class SlidingWindow
{
public:

    SlidingWindow() = default;
    ~SlidingWindow() = default;

    void append(value_type x)
    {

        if(m_length > 0)
        {
            ++m_last;
        }

        if(m_last >= window_size)
        {
            m_last = 0;
        }

        m_items[m_last] = x;

        if(m_length < window_size)
        {
            ++m_length;
        }

    }

    void reset()
    {
        m_length = 0;
        m_last = 0;
    }

    inline nostd::size_t capacity()
    {
        return window_size;
    }

    inline nostd::size_t length()
    {
        return m_length;
    }

    /* Iterators */
    typedef const value_type* const_pointer;


    class const_iterator
    {
    public:
        enum Wherence
        {
            BEGIN,
            END
        };

        const_iterator() = default;
        ~const_iterator() = default;

        const_iterator(const SlidingWindow &w, Wherence wherence)
        {
            m_window = (SlidingWindow*)&w;

            switch(wherence)
            {
                case BEGIN:
                {
                    m_index = m_window->P_first();
                    m_ring_index = 0;
                    break;
                }
                case END:
                {
                    m_index = m_window->P_last();
                    m_ring_index = m_window->m_length;
                    break;
                }
            }
        }

        const value_type& operator*()
        {
            if (m_ring_index >= m_window->m_length)
            {
                FATAL(NOSTD_ERR_OUT_OF_RANGE);
            }
            return m_window->m_items[m_index];
        }
        const value_type* operator->()
        {
            if (m_ring_index >= m_window->m_length)
            {
                FATAL(NOSTD_ERR_OUT_OF_RANGE);
            }
            return m_window->m_items[m_index];
        }

        const value_type* operator++()
        {
            if(m_ring_index >= m_window->m_length)
            {
                return nullptr;
            }

            value_type * tmp = &m_window->m_items[m_index++];

            if(m_index >= m_window->m_length)
            {
                m_index = 0;
            }

            ++m_ring_index;

            return tmp;
        }

        bool operator==(const const_iterator& other) const
        {
            return m_ring_index == other.m_ring_index;
        }

        bool operator!=(const const_iterator& other) const
        {
            return !(*this == other);
        }

        bool operator<(const const_iterator& other) const
        {
            return m_ring_index < other.m_ring_index;
        }

        bool operator>(const const_iterator& other) const
        {
            return m_ring_index > other.m_ring_index;
        }
    private:

        SlidingWindow * m_window = nullptr;
        nostd::size_t m_index = 0;  /* absolete index in array items[] */
        nostd::size_t m_ring_index = 0;
    };

    class reverse_const_iterator
    {
    private:

        SlidingWindow * m_window;
        nostd::size_t m_index = 0;  /* absolete index in array items[] */
        nostd::size_t m_ring_index = 0;

    public:
        enum Wherence
        {
            BEGIN,
            END
        };

        reverse_const_iterator()
        {
            m_window = NULL;
            m_index = 0;
            m_ring_index = 0;
        }

        reverse_const_iterator(const SlidingWindow &w, Wherence wherence)
        {
            m_window = (SlidingWindow*)&w;

            switch(wherence)
            {
                case BEGIN:
                {
                    m_index = m_window->P_last();
                    m_ring_index = 0;
                    break;
                }
                case END:
                {
                    m_index = m_window->P_first();
                    m_ring_index = m_window->m_length;
                    break;
                }
            }
        }

        const value_type& operator*()
        {
            if (m_ring_index >= m_window->m_length)
            {
                FATAL(NOSTD_ERR_OUT_OF_RANGE);
            }
            return m_window->m_items[m_index];
        }
        const value_type* operator->()
        {
            if (m_ring_index >= m_window->m_length)
            {
                FATAL(NOSTD_ERR_OUT_OF_RANGE);
            }
            return m_window->m_items[m_index];
        }

        const value_type* operator--()
        {
            if(m_ring_index >= m_window->m_length)
            {
                return nullptr;
            }

            value_type * tmp = &m_window->m_items[m_index];

            if(m_index == 0)
            {
                m_index = m_window->m_length - 1;
            }
            else
            {
                --m_index;
            }

            ++m_ring_index;

            return tmp;
        }

        bool operator==(const reverse_const_iterator& other) const
        {
            return m_ring_index == other.m_ring_index;
        }

        bool operator!=(const reverse_const_iterator& other) const
        {
            return !(*this == other);
        }

        bool operator<(const reverse_const_iterator& other) const
        {
            return m_ring_index < other.m_ring_index;
        }

        bool operator>(const reverse_const_iterator& other) const
        {
            return m_ring_index > other.m_ring_index;
        }
    };

    const_iterator cbegin() noexcept
    {
        return const_iterator(*this, const_iterator::BEGIN);
    }

    const_iterator cend() noexcept
    {
        return const_iterator(*this, const_iterator::END);
    }

    reverse_const_iterator rcbegin() noexcept
    {
        return reverse_const_iterator(*this, reverse_const_iterator::BEGIN);
    }

    reverse_const_iterator rcend() noexcept
    {
        return reverse_const_iterator(*this, reverse_const_iterator::END);
    }

    value_type& at(nostd::size_t i)
    {
        if(
                m_length == 0 ||
                i < 0 ||
                i >= m_length
        )
        {
            FATAL(NOSTD_ERR_OUT_OF_RANGE);
        }

        return operator[](i);
    }

    value_type& operator[](nostd::size_t i)
    {
        nostd::size_t index = P_last() - i;
        while(index >= m_length) index -= m_length;
        return m_items[index];
    }

private:

    nostd::size_t P_first() noexcept
    {
        nostd::size_t tmp_first = m_last + 1;
        return (tmp_first >= m_length) ? 0 : tmp_first;
    }

    nostd::size_t P_last() noexcept
    {
        return m_last;
    }

    nostd::size_t m_length = 0;
    nostd::size_t m_last = 0;
    value_type m_items[window_size]{};
};


}

#endif /* NOSTD_SLIDINGWINDOW_SLIDINGWINDOW_H_ */
