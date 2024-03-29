#ifndef CTRL_HPP_INCLUDED_
#define CTRL_HPP_INCLUDED_

//#define CTRL_DEBUG

#define CTRL_WARNING_SPEED_TOO_LOW   (1U << 0)
#define CTRL_WARNING_SPEED_TOO_HIGH  (1U << 1)

#define CTRL_WARNING_ALL (CTRL_WARNING_SPEED_TOO_LOW | CTRL_WARNING_SPEED_TOO_HIGH)

#include "Speed.hpp"

#include <nostd.h>

/**
 * @brief Class-controller
 */
class Ctrl
{
public:
    enum class Error
    {
        OK,
        FATAL,
        INVALID_MODE, /* Unable to execute command in this mode */
    };

    enum class RunMode
    {
        NORMAL_STOPPED,
        NORMAL_STARTED_AUTO,
        NORMAL_STARTED_MANUAL,
        SERVICE_MODE1_STOPPED,
        SERVICE_MODE1_STARTED,
        SERVICE_MODE2_STOPPED,
        SERVICE_MODE2_STARTED,
        SERVICE_MODE3_STOPPED,
        SERVICE_MODE3_STARTED,
    };

    enum class BaselineSpeedMode
    {
        MODE_LOW,
        MODE_HIGH,
    };

    enum class Event
    {
        WARNINGS_UPDATE,
        MOTOR_ON,
        MOTOR_OFF,
        MOTOR_SETPOINT_UPDATE,
        LIFT_UP,
        LIFT_DOWN,
        RUNMODE_CHANGED,
    };

    struct EventData
    {
        ~EventData(){};
        union
        {
            struct
            {
                unsigned warnings{};
            } WARNINGS_UPDATE;
            struct
            {
                Speed setpoint;
            } MOTOR_SETPOINT_UPDATE;
            struct
            {
                RunMode runMode;
            } RUNMODE_CHANGED;
        };
    };

#define CTRL_BASESPEEDMODE__NUM (static_cast<int>(BaselineSpeedMode::MODE_HIGH) + 1)

    Ctrl() = delete;
    Ctrl(const Ctrl&) = delete;
    Ctrl& operator=(const Ctrl&) = delete;

    /**
     * @param baseSpeedLow      Базовая скорость: низкая
     * Gparam baseSpeedHigh     Базовая скорость: высокая
     */
    Ctrl(
            Speed baseSpeedLow,
            Speed baseSpeedHigh,
            void (*eventFunc)(Event event, const EventData& data, void * args),
            void * args
    );
    ~Ctrl() = default;
    /**
     * @brief Выбор базовой скорости
     */
    Error baselineSpeedModeSet(BaselineSpeedMode baselineSpeedMode, void * args);
    /**
     * @brief Вручную задать отклонение скорости от выбраной базовой скорости
     */
    Error speedManualSet(Speed speed, void * args);

    /**
     * @brief Get setpoint of freespeed
     */
    Speed speedFreeGet();
    /**
     * @brief Свободное задание скорости для SERVICE_MODE_3
     * @note не зависит от baseline
     */
    Error speedFreeSet(Speed speed, void * args);

    void motorGet(bool * motor_state, Speed * motor_setpoint);

    Error autostopAllowSet(bool allow_autostop, void * args);

    /** @breif Leave service mode, enter normal mode */
    Error mode_normal(void * args);
    /** @brief Needle setting: drive stopped, lift down */
    Error mode_service_1(void * args);
    Error mode_service_2(void * args);
    Error mode_service_3(void * args);
    /** @brief Start action in normal or service mode */
    Error start(void * args);
    /** @brief Stop current action in normal or service mode */
    Error stop(void * args);

    /**
     * @param speed     Current table speed
     */
    void actualSpeedUpdate(Speed speed, void * args);
    Speed actualSpeed()
    {
        return m_speed_PV;
    }

    /**
     * @brief сработал датчик автостопа?
     */
    Error stopTriggeredSet(bool triggered, void * args);

    RunMode runModeGet();

    int errorsGet() const;
    int warningsGet() const;

private:

    enum class Command
    {
        INIT,
        ENTER_MODE_NORMAL,
        ENTER_SERVICE_MODE1,
        ENTER_SERVICE_MODE2,
        ENTER_SERVICE_MODE3,
        START,
        STOP,
        SPEED_BASELINE_LOW,
        SPEED_BASELINE_HIGH,
        SPEED_MANUAL_UPDATE,
        SPEED_FREE_UPDATE,
        AUTOSTOP_ALLOW,
        AUTOSTOP_DENY,
        GAUGE_STOP_ON,
        GAUGE_STOP_OFF,
    };

    struct CommandData
    {
        ~CommandData(){};
        union
        {
            struct
            {
                Speed speed;
            } SPEED_MANUAL_UPDATE;

            struct
            {
                Speed speed;
            } SPEED_FREE_UPDATE;
        };
    };

    enum class State
    {
        INIT,
        NORMAL_STOPPED,
        NORMAL_STARTED_AUTO,
        NORMAL_STARTED_MANUAL,
        SERVICE_MODE1_STOPPED,
        SERVICE_MODE1_STARTED,
        SERVICE_MODE2_STOPPED,
        SERVICE_MODE2_STARTED,
        SERVICE_MODE3_STOPPED,
        SERVICE_MODE3_STARTED,
    };

    /* init vars */
    void (*m_eventFunc)(Event event, const EventData& data, void * args);
    Speed m_speed_baselines[CTRL_BASESPEEDMODE__NUM]{};

    State m_state = State::INIT; /**< Finite State Machine state */
    unsigned m_state_errors = 0; /**< bitmap of errors */
    unsigned m_state_warnings = 0; /**< bitmap of warnings */
    bool m_state_allowed_autostop = false;
    bool m_state_autostop_triggered = false;

    /* user control variables */
    Speed m_speed_SP_manual_delta{};
    Speed m_speed_SP_free{};
    Speed m_speed_PV{};
    BaselineSpeedMode m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
    bool m_motor_state_cached = false; /*on/off */
    Speed m_motor_speed_SP_cached{};

    CommandData m_cmdData{};
    EventData m_eventData{};

    Speed P_speed_baseline_get() const;
    void P_event(Event event, const EventData& data, void * args) const;
    void P_event_warnings_set(unsigned warn, void * args);
    void P_event_warnings_clean(unsigned warn, void * args);
    void P_event_motor_on(void * args);
    void P_event_motor_off(void * args);
    void P_event_motor_setpoint_update(Speed setpoint, void * args);
    void P_event_lift_up(void * args);
    void P_event_lift_down(void * args);
    void P_event_runmode_changed(RunMode runMode, void * args);
    RunMode P_runModeGet(State state);

    Error P_fsm(Command cmd, const CommandData & data, void * args);

#ifdef CTRL_DEBUG
public:
    typedef struct
    {
        State m_state;
        Speed m_speed_SP_manual_delta;
    } internal_state_t;
    void debug_get(internal_state_t * state) const;
#endif

};

#endif /* CTRL_HPP_INCLUDED_ */
