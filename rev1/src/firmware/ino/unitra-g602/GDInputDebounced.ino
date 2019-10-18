
#include "GDInputDebounced.hpp"

GDInputDebounced::GDInputDebounced(
        bool initState,
        void (*onTriggeredOn)(void * args),
        void (*onTriggeredOff)(void * args),
        void * args,
        GTime_t debounceTime
)
: GDInput(initState, onTriggeredOn, onTriggeredOff, args)
{
    this->bouncedStatePrev = initState;
    this->debounceTime = debounceTime;
    this->debounceLastTime = 0;

#ifdef GDInputDebounced_DEBUG
    this->debug_bounces = 0;
    this->debug_bouncesStartTime = 0;
    this->debug_bouncesTime = 0;
#endif
}

GDInputDebounced::~GDInputDebounced()
{
    /* nothing */
}

void GDInputDebounced::stateSet(bool state, void * args, GTime_t time_current)
{

    if(state != bouncedStatePrev)
    {
#ifdef GDInputDebounced_DEBUG
        if(debug_bounces == 0)
        {
            debug_bouncesStartTime = time_current;
        }
        ++debug_bounces;
#endif
        debounceLastTime = time_current;
        bouncedStatePrev = state;
        return;
    }

    long dt = time_current - debounceLastTime;
    if(dt < 0)
    {
        dt = ((unsigned long) - 1L) - (unsigned long)dt + 1;
    }

    if(dt > (long)debounceTime)
    {
#ifdef GDInputDebounced_DEBUG
        debug_bouncesTime = time_current - debug_bouncesStartTime;
#endif
        GDInput::stateSet(state, args);
#ifdef GDInputDebounced_DEBUG
        debug_bounces = 0;
#endif
    }
}

#ifdef GDInputDebounced_DEBUG

unsigned int GDInputDebounced::debug_bouncesAmountGet() const
{
    return debug_bounces;
}

GTime_t GDInputDebounced::debug_bouncesTimeGet() const
{
    return debug_bouncesTime;
}

#endif
