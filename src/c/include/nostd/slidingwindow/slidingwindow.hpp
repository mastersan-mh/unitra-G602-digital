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

    SlidingWindow()
    {
        reset();
    }

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

    inline nostd::size_t size()
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
    private:

        SlidingWindow * m_window;
        nostd::size_t m_index;  /* absolete index in array items[] */
        nostd::size_t m_round_index;

    public:
        enum Wherence
        {
            SlidingWindow_const_iterator_BEGIN,
            SlidingWindow_const_iterator_END
        };

        const_iterator()
        {
            m_window = NULL;
            m_index = 0;
            m_round_index = 0;
        }

        const_iterator(const SlidingWindow &w, Wherence wherence)
        {
            m_window = (SlidingWindow*)&w;

            switch(wherence)
            {
                case SlidingWindow_const_iterator_BEGIN:
                {
                    m_index = m_window->P_first();
                    m_round_index = 0;
                    break;
                }
                case SlidingWindow_const_iterator_END:
                {
                    m_index = m_window->m_last;
                    m_round_index = m_window->m_length;
                    break;
                }
            }
        }

        const value_type& operator*()
        {
            if (m_round_index >= m_window->m_length)
            {
                THROW(nostd::out_of_range);
            }
            return m_window->m_items[m_index];
        }
        const value_type* operator->()
        {
            if (m_round_index >= m_window->m_length)
            {
                THROW(nostd::out_of_range);
            }
            return m_window->m_items[m_index];
        }

        const value_type* operator++()
        {
            if(m_round_index >= m_window->m_length)
            {
                return nullptr;
            }

            value_type * tmp = &m_window->m_items[m_index++];

            if(m_index >= m_window->m_length)
            {
                m_index = 0;
            }

            ++m_round_index;

            return tmp;
        }

        bool operator==(const const_iterator& other) const
        {
            return m_round_index == other.m_round_index;
        }

        bool operator!=(const const_iterator& other) const
        {
            return !(*this == other);
        }

        bool operator<(const const_iterator& other) const
        {
            return m_round_index < other.m_round_index;
        }

        bool operator>(const const_iterator& other) const
        {
            return m_round_index > other.m_round_index;
        }
    };

    const_iterator cbegin() noexcept
    {
        return const_iterator(*this, const_iterator::SlidingWindow_const_iterator_BEGIN);
    }

    const_iterator cend() noexcept
    {
        return const_iterator(*this, const_iterator::SlidingWindow_const_iterator_END);
    }

    value_type& at(nostd::size_t i)
    {
        if(
                m_length == 0 ||
                i < 0 ||
                i >= m_length
        )
        {
            THROW(nostd::out_of_range);
        }

        return operator[](i);
    }

    value_type& operator[](nostd::size_t i)
    {
        nostd::size_t index = P_first() + i;
        while(index >= m_length) index -= m_length;
        return m_items[index];
    }

#if 0
    bool operator!=(SlidingWindow const& other) const;
    bool operator==(SlidingWindow const& other) const; //need for BOOST_FOREACH
    typename OwnIterator::reference operator*() const;
    OwnIterator& operator++();

    /* i++ */
    SlidingWindow operator++()
    {
        return SlidingWindow();
    }

    /* ++i */
    SlidingWindow & operator++()
    {
        return SlidingWindow();
    }

    void cbegincc()
    {
std::array<int> a;
a.begin();
    }
#endif

private:

    nostd::size_t P_first() noexcept
    {
        nostd::size_t tmp_first = m_last + 1;
        return (tmp_first >= m_length) ? 0 : tmp_first;
    }

    nostd::size_t m_length;
    nostd::size_t m_last;
    value_type m_items[window_size];
};


}

#endif /* NOSTD_SLIDINGWINDOW_SLIDINGWINDOW_H_ */
