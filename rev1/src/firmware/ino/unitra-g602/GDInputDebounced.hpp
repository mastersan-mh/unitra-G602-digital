#ifndef GDINPUTDEBOUNCED_H_
#define GDINPUTDEBOUNCED_H_

#include "config.hpp"

#include "GTime.hpp"
#include "GDInput.hpp"

/* If you want to use this debug, you MUST compile the library with this define */
/* #define GDInputDebounced_DEBUG */

class GDInputDebounced : public GDInput
{
public:
    GDInputDebounced() = delete;
    GDInputDebounced(const GDInputDebounced&) = delete;
    GDInputDebounced& operator=(const GDInputDebounced&) = delete;
    /**
     * @param initState     Input is ON(true) or OFF(false)
     */
    GDInputDebounced(
        bool initState,
        void (*onTriggeredOn)(void * args),
        void (*onTriggeredOff)(void * args),
        void * args,
        GTime_t debounceTime
    );
    virtual ~GDInputDebounced() = default;
    /**
     * @param state         input is ON(true) or OFF(false)
     * @param time          time in msec, as returned by millis(). Should be monotonic.
     */
    void stateSet(bool state, void * args, GTime_t time_current);

#ifdef GDInputDebounced_DEBUG
    virtual unsigned int debug_bouncesAmountGet() const;
    {
        return m_debug_bounces;
    }

    virtual GTime_t debug_bouncesTimeGet() const;
    {
        return m_debug_bouncesTime;
    }
#endif

private:
    GTime_t m_debounceTime;
    GTime_t m_debounceLastTime = 0;
    bool m_bouncedStatePrev;

#ifdef GDInputDebounced_DEBUG
    unsigned int m_debug_bounces = 0;
    GTime_t m_debug_bouncesStartTime = 0;
    GTime_t m_debug_bouncesTime = 0;
#endif

};

#endif /* GDINPUTDEBOUNCED_H_ */
