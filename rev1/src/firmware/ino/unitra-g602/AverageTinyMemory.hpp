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
    virtual ~AverageTinyMemory();
/**
 * @param factor    фактор усреднения, [0; 63]
 */
    void factorSet(unsigned char factor);
    void appendValue(int value);
    int averageGet();
private:
    int mean;
    int value_prev;
    unsigned char factor;
    unsigned char len;
    unsigned char value_weight;
};



#endif /* AVERAGE_TINY_MEMORY_HPP_ */
