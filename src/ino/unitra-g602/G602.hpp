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
#include "GComm.hpp"
#include "GRPCServer.hpp"

#include <nostd.h>
#include <stdint.h>

typedef nostd::SchedulerSoft< G602_SHEDULER_TASKS__NUM, GTime_t > G602Scheduler;

class G602
{
#define G602_DEFINE_SELF() \
    G602 * self = static_cast<G602*>(args)

#define ARRAY_INDEX(x) static_cast<unsigned int>(x)

public:
    static const nostd::size_t shed_task_id_blinker = 0;
    static const nostd::size_t shed_task_id_service_mode_awaiting = 1;
    static const unsigned long service_mode_enter_awaiting_time = 5000; /**< time to wait to enter service mode, ms */

    G602() = delete;
    G602(
        int baselineSpeedLow,
        int baselineSpeedHigh,
        void (*event_config_store)(const uint8_t * conf, size_t size),
        void (*event_config_load)(uint8_t * conf, size_t size),
        void (*event_strober)(bool on),
        void (*event_lift_up)(),
        void (*event_lift_down)(),
        void (*event_motor_update)(bool state, int setpoint)
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
    void eventModeChanged(app::Ctrl::RunMode runMode);
    void eventSPPV(GTime_t time, uint16_t sp, uint16_t pv);
private:
    void (*m_event_config_store)(const uint8_t * conf, size_t size);
    void (*m_event_config_load)(uint8_t * conf, size_t size);
    void (*m_event_strober)(bool on);
    void (*m_event_lift_up)();
    void (*m_event_lift_down)();
    void (*m_event_motor_update)(bool state, int output);

    unsigned long m_time_now;
    unsigned long m_time_next;

    bool m_motor_on_prev;
    bool m_motor_on;

public:
    G602Scheduler sched;
private:
    typedef nostd::Fixed32 Fixed;
    typedef nostd::PidRecurrent<Fixed> PID;

    GBlinker m_blinker;
    app::Ctrl m_ctrl;
    GDInputDebounced m_di_gauge_stop;
    GDInputDebounced m_di_btn_speed_mode;
    GDInputDebounced m_di_btn_autostop;
    GDInputDebounced m_di_btn_start;
    GDInputDebounced m_di_btn_stop;
    GComm m_comm;
    GRPCServer m_rpc;

    /* buffer to recieve request */
#define CAPACITY 32
    uint8_t m_buf_frame[CAPACITY];

    bool m_permanent_process_send;

    Fixed m_Kp;
    Fixed m_Ki;
    Fixed m_Kd;

    PID m_pid;

    void P_config_store();
    void P_config_load();

    unsigned long P_rtcNextTimeGet() const;
    void P_blinker_start(GBlinker::BlinkType type);
    void P_blinker_stop(GBlinker::BlinkType type);
    static void P_task_blinker(nostd::size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args);
    static void P_task_awaiting_service_mode(nostd::size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args);
    static void P_task_rotator_handler(nostd::size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args);
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
    void P_comm_reader();
    static void P_rpc_send(const uint8_t * data, unsigned data_size, void * args);
    static uint8_t P_rpc_func_00_pulses_r(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    static uint8_t P_rpc_func_01_mode_current_r(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    /** @brief koeffizient read */
    static uint8_t P_rpc_func_02_koef_r(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    /** @brief koeffizient write */
    static uint8_t P_rpc_func_03_koef_w(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    static uint8_t P_rpc_func_04_speed_SP_r(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    static uint8_t P_rpc_func_05_speed_SP_w(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    static uint8_t P_rpc_func_06_speed_PV_r(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    static uint8_t P_rpc_func_07_process_start(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    static uint8_t P_rpc_func_08_process_stop(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);
    static uint8_t P_rpc_func_09_conf_store(unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args);

};

#endif /* G602_HPP_INCLUDED_ */
