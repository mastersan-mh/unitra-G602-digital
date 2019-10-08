/**
 * @file Blinker.cpp
 */

#include "Blinker.hpp"

Blinker::Blinker()
{
    m_pattern = nullptr;
    m_size = 0;
    m_infinite = false;
    m_index = 0;
    m_light = false;
}

Blinker::~Blinker()
{
}

void Blinker::start(unsigned long * pattern, unsigned size, bool infinite)
{
    m_pattern = pattern;
    m_size = size;
    m_infinite = infinite;
    m_index = 0;
    m_light = true;
}

void Blinker::stop()
{
    m_pattern = nullptr;
}

void Blinker::task(bool * end, bool * light, unsigned long * wait_time)
{
    if(m_pattern == nullptr)
    {
        (*end) = true;
        return;
    }

    if(m_index == m_size)
    {
        if(m_infinite)
        {
            (*end) = false;
            m_index = 0;
        }
        else
        {
            (*end) = true;
            return;
        }
    }

    (*light) = m_light;
    (*wait_time) = m_pattern[m_index];
    ++m_index;
    m_light = !m_light;
}

bool Blinker::isStarted()
{
    return (m_pattern != nullptr);
}
