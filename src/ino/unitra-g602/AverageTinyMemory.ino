/**
 * @file AverageTinyMemory.cpp
 */

#include "AverageTinyMemory.hpp"

#include <nostd/fatal>

AverageTinyMemory::AverageTinyMemory(unsigned char factor)
{
    this->mean = 0;
    factorSet(factor);
}

AverageTinyMemory::~AverageTinyMemory()
{
    /* nothing */
}

void AverageTinyMemory::factorSet(unsigned char factor)
{
    if(factor > AverageTinyMemory_FACTOR_MAX)
    {
        FATAL(NOSTD_ERR_OUT_OF_RANGE);
    }
    this->value_prev = 0;
    this->factor = factor;
    this->len = 0;
    this->value_weight = 0;
}

void AverageTinyMemory::appendValue(int value)
{
    if(factor == 0)
    {
        mean = value;
        len = 0;
        return;
    }

    if(value_prev == value)
    {
        ++value_weight;
    }
    else
    {
        value_weight = 1;
        value_prev = value;
    }

    if(len < factor)
    {
        ++len;
    }

    mean = (mean * (len - value_weight) + value * value_weight) / len;
}

int AverageTinyMemory::averageGet()
{
    return mean;
}
