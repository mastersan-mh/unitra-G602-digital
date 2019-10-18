#ifndef CTRL_H_INCLUDED_
#define CTRL_H_INCLUDED_

namespace app
{

//#define CTRL_DEBUG

#define CTRL_WARNING_SPEED_TOO_LOW   (1U << 0)
#define CTRL_WARNING_SPEED_TOO_HIGH  (1U << 1)

#define CTRL_WARNING_ALL (CTRL_WARNING_SPEED_TOO_LOW | CTRL_WARNING_SPEED_TOO_HIGH)

/**
 * @brief Class-controller
 */
class Ctrl
{
public:
    typedef int speed_t;

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

    typedef union
    {
        struct
        {
            unsigned warnings;
        } WARNINGS_UPDATE;
        struct
        {
            speed_t setpoint;
        } MOTOR_SETPOINT_UPDATE;
        struct
        {
            RunMode runMode;
        } RUNMODE_CHANGED;
    }  EventData;

#define CTRL_BASESPEEDMODE__NUM (static_cast<int>(BaselineSpeedMode::MODE_HIGH) + 1)

    Ctrl() = delete;
    /**
     * @param baseSpeedLow      Базовая скорость: низкая
     * Gparam baseSpeedHigh     Базовая скорость: высокая
     */
    Ctrl(
            speed_t baseSpeedLow,
            speed_t baseSpeedHigh,
            void (*eventFunc)(Event event, const EventData& data, void * args),
            void * args
    );
    Ctrl(const Ctrl&) = delete;
    Ctrl& operator=(const Ctrl&) = delete;

    virtual ~Ctrl();
    /**
     * @brief Выбор базовой скорости
     */
    Error baselineSpeedModeSet(BaselineSpeedMode baselineSpeedMode, void * args);
    /**
     * @brief Вручную задать отклонение скорости от выбраной базовой скорости
     */
    Error speedManualSet(speed_t speed, void * args);

    /**
     * @brief Get setpoint of freespeed
     */
    speed_t speedFreeGet();
    /**
     * @brief Свободное задание скорости для SERVICE_MODE_3
     * @note не зависит от baseline
     */
    Error speedFreeSet(speed_t speed, void * args);

    void motorGet(bool * motor_state, speed_t * motor_setpoint);

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
    void actualSpeedUpdate(speed_t speed, void * args);

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

    typedef union
    {
        struct
        {
            speed_t speed;
        } SPEED_MANUAL_UPDATE;

        struct
        {
            speed_t speed;
        } SPEED_FREE_UPDATE;
    } CommandData;

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

    speed_t P_speed_baseline_get() const;

    void P_event(Event event, const EventData& data, void * args) const;
    void P_event_warnings_set(unsigned warn, void * args);
    void P_event_warnings_clean(unsigned warn, void * args);
    void P_event_motor_on(void * args);
    void P_event_motor_off(void * args);
    void P_event_motor_setpoint_update(Ctrl::speed_t setpoint, void * args);
    void P_event_lift_up(void * args);
    void P_event_lift_down(void * args);
    void P_event_runmode_changed(RunMode runMode, void * args);
    RunMode P_runModeGet(State state);

    Error P_fsm(Command cmd, const CommandData & data, void * args);

    /* init vars */
    void (*m_eventFunc)(Event event, const EventData& data, void * args);
    speed_t m_speed_baselines[CTRL_BASESPEEDMODE__NUM];

    State m_state;        /**< Finite State Machine state */
    unsigned m_state_errors; /**< bitmap of errors */
    unsigned m_state_warnings; /**< bitmap of warnings */
    bool m_state_allowed_autostop;
    bool m_state_autostop_triggered;

    /* user control variables */
    speed_t m_speed_SP_manual_delta;
    speed_t m_speed_SP_free;
    BaselineSpeedMode m_speed_baseline_mode;
    bool m_motor_state_cached; /*on/off */
    speed_t m_motor_speed_SP_cached;

    CommandData m_cmdData;
    EventData m_eventData;

#ifdef CTRL_DEBUG
public:
    typedef struct
    {
        State m_state;
        speed_t m_speed_SP_manual_delta;
    } internal_state_t;
    void debug_get(internal_state_t * state) const;
#endif

};

} /* namespace app */
#endif /* CTRL_H_INCLUDED_ */
