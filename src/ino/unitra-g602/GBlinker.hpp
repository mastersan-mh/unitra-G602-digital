/**
 * @file GBlinker.hpp
 */

#ifndef GBLINKER_HPP_INCLUDED_
#define GBLINKER_HPP_INCLUDED_

#include "GBlinkerPatterned.hpp"

#define ARRAY_INDEX(x) static_cast<unsigned int>(x)


class GBlinker
{
public:
    enum class BlinkType
    {
        ON_START,
        ON_STOP,
        ON_AUTO,
        ON_MANUAL,
        ON_TOO_LOW_SPEED,
        ON_TOO_HIGH_SPEED,
        ON_ENTER_SERVICE_MODE,
    };

#define GBLINK_BLINKS_NUM (ARRAY_INDEX(GBlinker::BlinkType::ON_ENTER_SERVICE_MODE) + 1)

#define GBLINKER_ACTIONFLAG_UNSCHEDULE 0x01
#define GBLINKER_ACTIONFLAG_SCHEDULE   0x02
    GBlinker();
    GBlinker(const GBlinker& blinker);
    virtual ~GBlinker();
    GBlinker& operator=(const GBlinker& blinker);

    unsigned typeSet(BlinkType type, bool on);
    void scheduledPartGet(bool * end, bool * light, unsigned long * wait_time);

private:
    int P_blinker_max_priority_get();

    GBlinkerPatterned m_blinker;
    int m_index; /** < Active blink type */
    bool m_intermission;
    bool m_on[GBLINK_BLINKS_NUM]; /**< on/off blink */
};
#endif /* GBLINKER_HPP_INCLUDED_ */
