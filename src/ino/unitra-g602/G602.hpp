/**
 * @file G602.hpp
 */

#ifndef G602_HPP_INCLUDED_
#define G602_HPP_INCLUDED_

#include "defs.hpp"

#include "GDInputDebounced.hpp"
#include "GTime.hpp"
#include "Ctrl.hpp"
#include "GBlinker.hpp"

#include <nostd.h>

typedef nostd::SchedulerSoft< G602_SHEDULER_TASKS__NUM, GTime_t > G602Scheduler;

class G602
{
#define G602_DEFINE_SELF() \
    G602 * self = static_cast<G602*>(args)

#define ARRAY_INDEX(x) static_cast<unsigned int>(x)

public:
    static const nostd::size_t shed_task_id_blinker = 0;
    static const nostd::size_t shed_task_id_service_mode_awaiting = 1;
    static const unsigned long service_mode_awaiting_tine = 5000; /**< time to wait to enter service mode, ms */

    G602() = delete;
    G602(
        int baselineSpeedLow,
        int baselineSpeedHigh,
        void (*event_strober)(bool on),
        void (*event_lift_up)(),
        void (*event_lift_down)(),
        void (*event_motorUpdate)(bool state, int setpoint)
    );
    ~G602();
    G602(const G602 &) = delete;
    G602& operator= (const G602 &) = delete;

    /** @brief Set current time */
    void timeSet(unsigned long time_now);
    /** @brief Call it on each loop */
    void loop();

    void notifyGaugeStopSet(bool state);
    void notifyButtonSpeedModeSet(bool state);
    void notifyButtonAutostopSet(bool state);
    void notifyButtonStartSet(bool state);
    void notifyButtonStopSet(bool state);
    /** @brief Set manual speed relative to base speed */
    void manualSpeedSet(int speed);
    /** @brief Update the actual speed */
    void actualSpeedUpdate(int speed);
private:
    void (*m_event_strober)(bool on);
    void (*m_event_lift_up)();
    void (*m_event_lift_down)();
    void (*m_event_motor_update)(bool state, int setpoint);

    unsigned long m_time_now;
    unsigned long m_time_next;

    unsigned m_service_mode; /**< index of service mode: 0 - no, 1 - service mode #1 an so on*/

    bool m_motor_on;
    int m_motor_setpoint;

public:
    G602Scheduler sched;
private:
    GBlinker m_blinker;
    app::Ctrl m_ctrl;
    GDInputDebounced m_di_gauge_stop;
    GDInputDebounced m_di_btn_speed_mode;
    GDInputDebounced m_di_btn_autostop;
    GDInputDebounced m_di_btn_start;
    GDInputDebounced m_di_btn_stop;

    unsigned long P_rtcNextTimeGet() const;
    void P_blinker_start(GBlinker::BlinkType type);
    void P_blinker_stop(GBlinker::BlinkType type);
    static void P_task_blinker(nostd::size_t id, unsigned long time, unsigned long now, G602Scheduler & sched, void * args);
    static void P_task_awaiting_service_mode(nostd::size_t id, unsigned long time, unsigned long now, G602Scheduler & sched, void * args);
    void P_measures_start();
    void P_measures_stop();

    void P_motor_update();
    static void P_ctrl_event(app::Ctrl::Event event, const app::Ctrl::EventData& data, void * args);
    static void P_event_stopSet(void * args);
    static void P_event_stopUnset(void * args);
    static void P_event_speedMode45(void * args);
    static void P_event_speedMode33(void * args);
    static void P_event_autostopEnable(void * args);
    static void P_event_autostopDisable(void * args);
    static void P_event_start(void * args);
    static void P_event_stop(void * args);
    static void P_event_stop_release(void * args);

};

#endif /* G602_HPP_INCLUDED_ */
