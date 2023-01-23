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
#include "Speed.hpp"

#include <nostd.h>
#include <stdint.h>

using G602Scheduler = nostd::SchedulerSoft< G602_SHEDULER_TASKS__NUM, GTime_t >;

class G602
{
#define ARRAY_INDEX(x) static_cast<unsigned int>(x)

    static constexpr Speed m_const_value1div2 = Speed(1, Speed::tag_int) / 2;

    static constexpr unsigned m_conf_table_pulses_per_revolution = G602_TABLE_PULSES_PER_REV;
    static constexpr Speed m_conf_baselineSpeedLow = Speed(G602_SPEED_BASELINE_LOW, Speed::tag_double);
    static constexpr Speed m_conf_baselineSpeedHigh = Speed(G602_SPEED_BASELINE_HIGH, Speed::tag_double);

    /* WARNING: Before add the new task_id, check the <G602_SHEDULER_TASKS__NUM> */
    static constexpr nostd::size_t shed_task_id_blinker = 0;
    static constexpr nostd::size_t shed_task_id_service_mode_awaiting = 1;
    static constexpr nostd::size_t shed_task_id_ctrl = 2;

    static constexpr unsigned long service_mode_enter_awaiting_time = 5000; /**< time to wait to enter service mode, ms */
    static constexpr unsigned long ctrl_handler_period = 2250; /**< Length of control tact, ms */
    static constexpr unsigned long ctrl_pulses_window_size = 1; /**< Window size, amount of elements */
    static constexpr unsigned ctrl_send_factor = 0; /**< Window size, amount of elements */

public:
    G602() = delete;
    G602(
            void (*event_config_store)(const uint8_t * conf, size_t size),
            void (*event_config_load)(uint8_t * conf, size_t size, bool * empty),
            void (*event_strober)(bool on),
            void (*event_lift_up)(),
            void (*event_lift_down)(),
            void (*event_motor_updatie)(bool state, int setpoint),
            void (*event_pulses_get)(unsigned * motor_pulses, unsigned * table_pulses)
    );
    virtual ~G602();
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
    void manualSpeedSet(Speed speed);
private:

    using PID = nostd::PidRecurrent<Speed>;
    using SWindowPulses = nostd::FastSum<unsigned, ctrl_pulses_window_size>;

    void (*m_event_config_store)(const uint8_t * conf, size_t size);
    void (*m_event_config_load)(uint8_t * conf, size_t size, bool * empty);
    void (*m_event_strober)(bool on);
    void (*m_event_lift_up)();
    void (*m_event_lift_down)();
    void (*m_event_motor_update)(bool state, int output);
    void (*m_event_pulses_get)(unsigned * motor_dpulses, unsigned * table_dpulses);

    unsigned long m_time_now  = 0;
    unsigned long m_time_next = 0;
    unsigned long m_send_counter = 0;

    G602Scheduler m_sched{};
    GBlinker m_blinker{};
    Ctrl m_ctrl;
    GDInputDebounced m_di_gauge_stop;
    GDInputDebounced m_di_btn_speed_mode;
    GDInputDebounced m_di_btn_autostop;
    GDInputDebounced m_di_btn_start;
    GDInputDebounced m_di_btn_stop;
    GComm m_comm{};
    GRPCServer m_rpc;

    /* buffer to recieve request */
#define CAPACITY 32
    uint8_t m_buf_frame[CAPACITY]{};

    bool m_permanent_process_send = false;

    Speed m_Kp{};
    Speed m_Ki{};
    Speed m_Kd{};

    PID m_pid{};

    SWindowPulses m_pulses_sum{}; /**< Amount of pulses per period <ctrl_handler_period>, sum for period */

    void P_config_store();
    void P_config_load();

    unsigned long P_rtcNextTimeGet() const;
    void P_blinker_start(GBlinker::BlinkType type);
    void P_blinker_stop(GBlinker::BlinkType type);
    static void P_task_blinker(nostd::size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args);
    static void P_task_awaiting_service_mode(nostd::size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args);
    static void P_task_ctrl(nostd::size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args);
    void P_ctrl_start();
    void P_ctrl_stop();

    void P_motor_update();
    static void P_ctrl_event(Ctrl::Event event, const Ctrl::EventData& data, void * args);
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

    /** @brief Update the actual speed */
    void P_rpc_eventModeChanged(Ctrl::RunMode runMode);
    void P_rpc_eventSPPV(GTime_t time, Speed sp, Speed pv, Speed out);

};

#endif /* G602_HPP_INCLUDED_ */
