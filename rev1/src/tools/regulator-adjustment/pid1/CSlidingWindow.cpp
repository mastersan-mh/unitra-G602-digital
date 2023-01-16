#include "CSlidingWindow.hpp"

CSlidingWindow::CSlidingWindow(int capacity)
    : m_size(capacity)
{
    m_vec.reserve(capacity);
}

void CSlidingWindow::sizeSet(int size)
{
    m_size = size;
    m_vec.reserve(size);
    int vsize = m_vec.size();
    if(vsize > size)
    {
        /* cut-off begin */
        m_vec.remove(0, vsize - size);
    }
}

int CSlidingWindow::sizeGet() const
{
    return m_size;
}

bool CSlidingWindow::append(double val)
{
    m_vec.append(val);
    if(m_vec.size() > m_size)
    {
        m_vec.pop_front();
        return true;
    }
    return false;
}

bool CSlidingWindow::isFull() const
{
    return (m_vec.size() == m_size);
}

void CSlidingWindow::clear()
{
    m_vec.clear();
    m_vec.reserve(m_size);
}

const QVector <double> &CSlidingWindow::get()
{
    return m_vec;
}

