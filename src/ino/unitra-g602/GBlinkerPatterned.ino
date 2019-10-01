/**
 * @file BlinkerPatterned.cpp
 */

#include "GBlinkerPatterned.hpp"

GBlinkerPatterned::GBlinkerPatterned()
: m_pattern(nullptr)
, m_size(0)
, m_infinite(false)
, m_index(0)
, m_light(false)
{
}

GBlinkerPatterned::GBlinkerPatterned(const GBlinkerPatterned& blinker)
: m_pattern (blinker.m_pattern)
, m_size    (blinker.m_size)
, m_infinite(blinker.m_infinite)
, m_index   (blinker.m_index)
, m_light   (blinker.m_light)
{
}

GBlinkerPatterned::~GBlinkerPatterned()
{
}

GBlinkerPatterned& GBlinkerPatterned::operator=(const GBlinkerPatterned& blinker)
{
    m_pattern  = blinker.m_pattern;
    m_size     = blinker.m_size;
    m_infinite = blinker.m_infinite;
    m_index    = blinker.m_index;
    m_light    = blinker.m_light;
    return *this;
}


void GBlinkerPatterned::start(const unsigned long * pattern, unsigned size, bool infinite)
{
    m_pattern = pattern;
    m_size = size;
    m_infinite = infinite;
    m_index = 0;
    m_light = true;
}

void GBlinkerPatterned::stop()
{
    m_pattern = nullptr;
}

void GBlinkerPatterned::partGet(bool * end, bool * light, unsigned long * wait_time)
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
            m_index = 0;
        }
        else
        {
            (*end) = true;
            return;
        }
    }
    (*end) = false;

    (*light) = m_light;
    (*wait_time) = m_pattern[m_index];
    ++m_index;
    m_light = !m_light;
}

bool GBlinkerPatterned::isStarted()
{
    return (m_pattern != nullptr);
}
