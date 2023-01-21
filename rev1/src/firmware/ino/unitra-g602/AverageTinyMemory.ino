/**
 * @file AverageTinyMemory.cpp
 */

#include "AverageTinyMemory.hpp"

#include <nostd/fatal>

AverageTinyMemory::AverageTinyMemory(unsigned char factor)
: m_factor(factor)
{
    if(factor > AverageTinyMemory_FACTOR_MAX)
    {
        FATAL(NOSTD_ERR_OUT_OF_RANGE);
    }
}

void AverageTinyMemory::factorSet(unsigned char factor)
{
    if(factor > AverageTinyMemory_FACTOR_MAX)
    {
        FATAL(NOSTD_ERR_OUT_OF_RANGE);
    }
    m_value_prev = 0;
    m_factor = factor;
    m_len = 0;
    m_value_weight = 0;
}

void AverageTinyMemory::appendValue(int value)
{
    if(m_factor == 0)
    {
        m_mean = value;
        m_len = 0;
        return;
    }

    if(m_value_prev == value)
    {
        ++m_value_weight;
    }
    else
    {
        m_value_weight = 1;
        m_value_prev = value;
    }

    if(m_len < m_factor)
    {
        ++m_len;
    }

    m_mean = (m_mean * (m_len - m_value_weight) + value * m_value_weight) / m_len;
}

int AverageTinyMemory::averageGet()
{
    return m_mean;
}
