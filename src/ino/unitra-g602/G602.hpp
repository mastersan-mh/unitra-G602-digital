#ifndef G602_H_
#define G602_H_

namespace app
{

//#define G602_DEBUG

/*
 Для уменьшения шага регулирования но при этом использовать целые числа,
 используются об/час вместо об/мин
*/

#define WARNING_SPEED_TOO_LOW   (1 << 0)
#define WARNING_SPEED_TOO_HIGH  (1 << 1)

class G602
{
public:
    typedef int speed_t;

    enum class Event
    {
        ERRORS_UPDATE,
        WARNINGS_UPDATE,
        MOTOR_ON,
        MOTOR_OFF,
        MOTOR_SETPOINT_UPDATE,
        LIFT_UP,
        LIFT_DOWN,
    };

    typedef union
    {
        struct
        {
            unsigned errors;
        } ERRORS_UPDATE;
        struct
        {
            unsigned warnings;
        } WARNINGS_UPDATE;
        struct
        {
            speed_t setpoint;
        } DRIVE_SETPOINT_UPDATE;
    }  EventData;

    enum class BaselineSpeedMode
    {
        MODE_LOW,
        MODE_HIGH,
    };
#define G602_BASESPEEDMODE__NUM (static_cast<int>(BaselineSpeedMode::MODE_HIGH) + 1)

    G602() = delete;
    /**
     * @param baseSpeedLow      Базовая скорость: низкая
     * Gparam baseSpeedHigh     Базовая скорость: высокая
     */
    G602(
            speed_t baseSpeedLow,
            speed_t baseSpeedHigh,
            void (*eventFunc)(Event event, const EventData& data)
    );
    G602(const G602&) = delete;
    G602& operator=(const G602&) = delete;

    ~G602();
    /**
     * @brief Выбор базовой скорости
     */
    void baselineSpeedModeSet(BaselineSpeedMode baselineSpeedMode);
    /**
     * @brief Вручную задать отклонение скорости от выбраной базовой скорости
     */
    void manualSpeedDeltaSet(speed_t speed);
    void autostopAllowSet(bool allow_autostop);

    void start();
    void stop();

    /**
     * @param speed     Current table speed
     */
    void actualSpeedUpdate(speed_t speed);

    /**
     * @brief сработал датчик автостопа?
     */
    void stopTriggeredSet(bool triggered);

    int errorsGet() const;
    int warningsGet() const;

private:

    enum class Command
    {
        INIT,
        SPEED_BASELINE_LOW,
        SPEED_BASELINE_HIGH,
        SPEED_MANUAL_UPDATE,
        AUTOSTOP_ALLOW,
        AUTOSTOP_DENY,
        START,
        STOP,
        GAUGE_STOP_ON,
        GAUGE_STOP_OFF,
    };

    typedef union
    {
        struct
        {
            speed_t speed;
        } SPEED_MANUAL_UPDATE;
    } CommandData;

    enum class State
    {
        INIT,
        STOPPED,
        STARTED,
        STARTED_MANUAL,
    };

    speed_t P_speed_baseline_get() const;

    void P_event(Event event, const EventData& data) const;
    void P_event_errors_set(unsigned err);
    void P_event_errors_clear(unsigned err);
    void P_event_warnings_set(unsigned warn);
    void P_event_warnings_clean(unsigned warn);
    void P_event_motor_on();
    void P_event_motor_off();
    void P_event_motor_setpoint_update(G602::speed_t setpoint);
    void P_event_lift_up();
    void P_event_lift_down();

    void P_fsm(Command cmd, const CommandData & data);

    /* init vars */
    void (*m_eventFunc)(Event event, const EventData& data);
    speed_t m_speed_baselines[G602_BASESPEEDMODE__NUM];

    State m_state;        /**< Finite State Machine state */
    unsigned m_state_errors; /**< bitmap of errors */
    unsigned m_state_warnings; /**< bitmap of warnings */
    bool m_state_allowed_autostop;
    bool m_state_autostop_triggered;

    /* user control variables */
    speed_t m_speed_manual_delta;

    BaselineSpeedMode m_speed_baseline_mode;

    CommandData m_cmdData;
    EventData m_eventData;

#ifdef G602_DEBUG
public:
    typedef struct
    {
        State m_state;
        speed_t m_speed_manual_delta;
    } internal_state_t;
    void debug_get(internal_state_t * state) const;
#endif

};

} /* namespace app */
#endif /* G602_H_ */
