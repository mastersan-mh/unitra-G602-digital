
#include "GDInputDebounced.hpp"

GDInputDebounced::GDInputDebounced(
        bool initState,
        void (*onTriggeredOn)(void * args),
        void (*onTriggeredOff)(void * args),
        void * args,
        GTime_t debounceTime
)
: GDInput(initState, onTriggeredOn, onTriggeredOff, args)
, m_debounceTime(debounceTime)
, m_bouncedStatePrev(initState)
{}

void GDInputDebounced::stateSet(bool state, void * args, GTime_t time_current)
{

    if(state != m_bouncedStatePrev)
    {
#ifdef GDInputDebounced_DEBUG
        if(m_debug_bounces == 0)
        {
            m_debug_bouncesStartTime = time_current;
        }
        ++m_debug_bounces;
#endif
        m_debounceLastTime = time_current;
        m_bouncedStatePrev = state;
        return;
    }

    long dt = time_current - m_debounceLastTime;
    if(dt < 0)
    {
        dt = ((unsigned long) - 1L) - (unsigned long)dt + 1;
    }

    if(dt > (long)m_debounceTime)
    {
#ifdef GDInputDebounced_DEBUG
        m_debug_bouncesTime = time_current - m_debug_bouncesStartTime;
#endif
        GDInput::stateSet(state, args);
#ifdef GDInputDebounced_DEBUG
        m_debug_bounces = 0;
#endif
    }
}
