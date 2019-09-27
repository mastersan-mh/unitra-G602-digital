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
            void (*onTriggeredOn)(),
            void (*onTriggeredOff)(),
            GTime_t debounceTime
    );
    virtual ~GDInputDebounced();
    /**
     * @param state         input is ON(true) or OFF(false)
     * @param time          time in msec, as returned by millis(). Should be monotonic.
     */
    virtual void stateSet(bool state, GTime_t time_current);

#ifdef GDInputDebounced_DEBUG
    virtual unsigned int debug_bouncesAmountGet() const;
    virtual GTime_t debug_bouncesTimeGet() const;
#endif

private:
    GTime_t debounceTime;
    GTime_t debounceLastTime;
    bool bouncedStatePrev;

#ifdef GDInputDebounced_DEBUG
    unsigned int debug_bounces;
    GTime_t debug_bouncesStartTime;
    GTime_t debug_bouncesTime;
#endif

};

#endif /* GDINPUTDEBOUNCED_H_ */
