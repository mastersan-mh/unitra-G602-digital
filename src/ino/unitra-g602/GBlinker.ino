/**
 * @file Blinker.cpp
 */

#include "GBlinker.hpp"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

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
        12,
        12,
        17,
        17,
        26,
        26,
        39,
        39,
        59,
        59,
        88,
        88,
        132,
        132,
        198,
        198,
        296,
        296,
        444,
        444,
        667,
        667,
        1000,
};

static const unsigned long P_blink_pattern_acceleration[] =
{
        1000,
        1000,
        667,
        667,
        444,
        444,
        296,
        296,
        198,
        198,
        132,
        132,
        88,
        88,
        59,
        59,
        39,
        39,
        26,
        26,
        17,
        17,
        12,
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
        [ARRAY_INDEX(GBlinker::BlinkType::ACCELERATING)] = { P_blink_pattern_acceleration, ARRAY_SIZE(P_blink_pattern_acceleration), false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::BRAKING     )] = { P_blink_pattern_braking, ARRAY_SIZE(P_blink_pattern_braking), false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::B1TIME      )] = { P_blink_pattern_5_250  ,  1, false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::B2TIME      )] = { P_blink_pattern_5_250  ,  3, false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::B3TIME      )] = { P_blink_pattern_5_250  ,  5, false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::FAST6       )] = { P_blink_pattern_11_100 , 11, false, 1 },
        [ARRAY_INDEX(GBlinker::BlinkType::SLOW        )] = { P_blink_pattern_1_500  ,  1, true , 0 },
        [ARRAY_INDEX(GBlinker::BlinkType::FAST        )] = { P_blink_pattern_1_100  ,  1, true , 0 },
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
