/**
 * @file GBlinkerPatterned.hpp
 */

#ifndef GBLINKERPATTERNED_HPP_INCLUDED_
#define GBLINKERPATTERNED_HPP_INCLUDED_

class GBlinkerPatterned
{
public:
    GBlinkerPatterned();
    GBlinkerPatterned(const GBlinkerPatterned& blinker);
    virtual ~GBlinkerPatterned();
    GBlinkerPatterned& operator=(const GBlinkerPatterned& blinker);
    void start(const unsigned long * pattern, unsigned size, bool infinite);
    void stop();
    /**
     * @brief Get the part of pattern
     * @param end           if true, do nothing, just stop
     * @param light         true = light on, false = light off
     * @param wait_time     if 0, stop the task, otherwise call it in next time again
     */
    void partGet(bool * end, bool * light, unsigned long * wait_time);
    bool isStarted();
private:
    const unsigned long * m_pattern;
    unsigned m_size;
    bool m_infinite;
    unsigned m_index;
    bool m_light;
};

#endif /* GBLINKERPATTERNED_HPP_INCLUDED_ */
