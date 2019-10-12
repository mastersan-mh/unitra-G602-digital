#include "CSlidingWindow.hpp"

CSlidingWindow::CSlidingWindow(int capacity)
    : m_capacity(capacity)
    , m_vec()
{
    m_vec.reserve(capacity);
}

CSlidingWindow::~CSlidingWindow()
{

}

void CSlidingWindow::capacitySet(int capacity)
{
    m_capacity = capacity;
    m_vec.reserve(capacity);
    if(m_vec.size() > capacity)
    {
        /* cut-off */
        m_vec.resize(capacity);
    }
}

bool CSlidingWindow::append(double val)
{
    m_vec.append(val);
    if(m_vec.size() > m_capacity)
    {
        m_vec.pop_front();
        return true;
    }
    return false;
}

bool CSlidingWindow::isFull()
{
    return (m_vec.size() == m_capacity);
}

void CSlidingWindow::clear()
{
    m_vec.clear();
    m_vec.reserve(m_capacity);
}

const QVector <double> &CSlidingWindow::get()
{
    return m_vec;
}

