/*
 * Blinker.hpp
 *
 *  Created on: 12 нояб. 2018 г.
 *      Author: mastersan
 */

#ifndef BLINKER_HPP_
#define BLINKER_HPP_

class Blinker
{
public:
    Blinker();
    virtual ~Blinker();
    void start(const unsigned long * pattern, unsigned size, bool infinite);
    void stop();
    /**
     * @param end           if true, do nothing, just stop
     * @param light         true = light on, false = light off
     * @param next_time     if 0, stop the task, otherwise call it in next time again
     */
    void task(bool * end, bool * light, unsigned long * wait_time);
    bool isStarted();
private:
    const unsigned long * m_pattern;
    unsigned m_size;
    bool m_infinite;
    unsigned m_index;
    bool m_light;
};

#endif /* BLINKER_HPP_ */
