/**
 * @file AverageTinyMemory.hpp
 */

#ifndef AVERAGE_TINY_MEMORY_HPP_
#define AVERAGE_TINY_MEMORY_HPP_

#define AverageTinyMemory_FACTOR_MAX 32

class AverageTinyMemory
{
public:

    AverageTinyMemory(unsigned char factor);
    ~AverageTinyMemory() = default;
/**
 * @param factor    фактор усреднения, [0; 63]
 */
    void factorSet(unsigned char factor);
    void appendValue(int value);
    int averageGet();
private:
    int m_mean = 0;
    int m_value_prev = 0;
    unsigned char m_factor;
    unsigned char m_len = 0;
    unsigned char m_value_weight = 0;
};



#endif /* AVERAGE_TINY_MEMORY_HPP_ */
