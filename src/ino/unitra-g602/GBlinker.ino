/**
 * @file Blinker.cpp
 */

#include "GBlinker.hpp"

struct blink_descr
{
    const unsigned long * pattern;
    unsigned size;
    bool infinite;
    unsigned char priority; /**< Priority of the blinker. Blink the blinker, which have higher priority. */
};

#define INTERMISSION_TIME 1000

static const unsigned long P_blink_pattern_braking[] =
{
        100,
        100,
        250,
        250,
        500,
        500,
        750,
        750,
        1000,
};

static const unsigned long P_blink_pattern_acceleration[] =
{
        1000,
        1000,
        750,
        750,
        500,
        500,
        250,
        250,
        100,
};

static const unsigned long P_blink_pattern_5_250[5] =
{
        250, /* [ ] (initial) */
        250, /* [*] */
        250, /* [ ] */
        250, /* [*] */
        250, /* [ ] */
};

static const unsigned long P_blink_pattern_11_100[11] =
{
        100, /* [ ] (initial) */
        100, /* [*] */
        100, /* [ ] */
        100, /* [*] */
        100, /* [ ] */
        100, /* [*] */
        100, /* [ ] */
        100, /* [*] */
        100, /* [ ] */
        100, /* [*] */
        100, /* [ ] */
};

static const unsigned long P_blink_pattern_1_500[1] =
{
        500,
};

static const unsigned long P_blink_pattern_1_100[1] =
{
        100,
};


static const struct blink_descr P_blink_descrs[GBLINK_BLINKS_NUM] =
{
        [ARRAY_INDEX(GBlinker::BlinkType::ON_START         )] = { P_blink_pattern_acceleration, 9, false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::ON_STOP          )] = { P_blink_pattern_braking     , 9, false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::ON_AUTO          )] = { P_blink_pattern_5_250       , 1, false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::ON_MANUAL        )] = { P_blink_pattern_5_250       , 5, false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::ON_TOO_LOW_SPEED )] = { P_blink_pattern_1_500       , 1, true , 0 },
        [ARRAY_INDEX(GBlinker::BlinkType::ON_TOO_HIGH_SPEED)] = { P_blink_pattern_1_100       , 1, true , 0 },
        [ARRAY_INDEX(GBlinker::BlinkType::ON_ENTER_SERVICE_MODE)] = { P_blink_pattern_11_100  , 11, false , 1 },
};

GBlinker::GBlinker()
: m_blinker()
, m_index(-1)
, m_intermission(false)
{
    unsigned i;
    for(i = 0; i < GBLINK_BLINKS_NUM; ++i)
    {
        m_on[i] = false;
    }
}

GBlinker::~GBlinker()
{
}

int GBlinker::P_blinker_max_priority_get()
{
    unsigned i;
    int priority = -1;
    int index = -1;
    for(i = 0; i < GBLINK_BLINKS_NUM; ++i)
    {
        if(m_on[i])
        {
            const struct blink_descr * blink_descrs = &P_blink_descrs[i];
            if((int)blink_descrs->priority > priority)
            {
                priority = blink_descrs->priority;
                index = i;
            }
        }
    }

    return index;
}

/**
 * @param mode  Mode of blinking
 * @param on    Switch-on the mode or switch-off
 */
unsigned GBlinker::typeSet(BlinkType type, bool on)
{
    unsigned actionFlags = 0;
    int index_prev = m_index;

    m_on[ARRAY_INDEX(type)] = on;

    int index = P_blinker_max_priority_get();

    if(index_prev != index)
    {
        if(index_prev >= 0)
        {
            m_blinker.stop();
            actionFlags |= GBLINKER_ACTIONFLAG_UNSCHEDULE;
        }

        if(index >= 0)
        {
            const struct blink_descr * blink_descr = &P_blink_descrs[index];
            m_blinker.start(blink_descr->pattern, blink_descr->size, blink_descr->infinite);
            actionFlags |= GBLINKER_ACTIONFLAG_SCHEDULE;
        }

        m_index = index;
    }

    return actionFlags;
}

void GBlinker::scheduledPartGet(bool * end, bool * light, unsigned long * wait_time)
{
    if(!m_intermission)
    {
        m_blinker.partGet(end, light, wait_time);
        if((*end) && m_index >= 0)
        {
            m_blinker.stop();
            m_on[m_index] = false;

            int index = P_blinker_max_priority_get();
            m_index = index;
            if(index < 0)
            {
                return;
            }
            (*end) = false;
            (*light) = false;
            (*wait_time) = INTERMISSION_TIME;
            m_intermission = true;
        }
    }
    else
    {
        /* resume a pending blinks */
        if(m_index >= 0)
        {
            const struct blink_descr * blink_descr = &P_blink_descrs[m_index];
            m_blinker.start(blink_descr->pattern, blink_descr->size, blink_descr->infinite);
            m_blinker.partGet(end, light, wait_time);
        }
        m_intermission = false;
    }
}
