
#include "Ctrl.hpp"

#define ARRAY_INDEX(x) static_cast<unsigned int>(x)

namespace app
{

Ctrl::speed_t Ctrl::P_speed_baseline_get() const
{
    return m_speed_baselines[ARRAY_INDEX(m_speed_baseline_mode)];
}

void Ctrl::P_event(Event event, const EventData& data, void * args) const
{
    if(m_eventFunc)
    {
        m_eventFunc(event, data, args);
    }
}

void Ctrl::P_event_errors_set(unsigned err, void * args)
{
    unsigned old = m_state_errors;
    m_state_errors |= err;
    if(m_state_errors != old)
    {
        m_eventData.ERRORS_UPDATE.errors = m_state_errors;
        P_event(Event::ERRORS_UPDATE, m_eventData, args);
    }
}

void Ctrl::P_event_errors_clear(unsigned err, void * args)
{
    unsigned old = m_state_errors;
    m_state_errors &= ~err;
    if(m_state_errors != old)
    {
        m_eventData.ERRORS_UPDATE.errors = m_state_errors;
        P_event(Event::ERRORS_UPDATE, m_eventData, args);
    }
}

void Ctrl::P_event_warnings_set(unsigned warn, void * args)
{
    unsigned old = m_state_warnings;
    m_state_warnings |= warn;
    if(m_state_warnings != old)
    {
        m_eventData.WARNINGS_UPDATE.warnings = m_state_warnings;
        P_event(Event::WARNINGS_UPDATE, m_eventData, args);
    }
}

void Ctrl::P_event_warnings_clean(unsigned warn, void * args)
{
    unsigned old = m_state_warnings;
    m_state_warnings &= ~warn;
    if(m_state_warnings != old)
    {
        m_eventData.WARNINGS_UPDATE.warnings = m_state_warnings;
        P_event(Event::WARNINGS_UPDATE, m_eventData, args);
    }
}

void Ctrl::P_event_motor_on(void * args)
{
    P_event(Event::MOTOR_ON, m_eventData, args);
}

void Ctrl::P_event_motor_off(void * args)
{
    P_event(Event::MOTOR_OFF, m_eventData, args);
}

void Ctrl::P_event_motor_setpoint_update(Ctrl::speed_t setpoint, void * args)
{
    m_eventData.DRIVE_SETPOINT_UPDATE.setpoint = setpoint;
    P_event(Event::MOTOR_SETPOINT_UPDATE, m_eventData, args);
}

void Ctrl::P_event_lift_up(void * args)
{
    P_event(Event::LIFT_UP, m_eventData, args);
}

void Ctrl::P_event_lift_down(void * args)
{
    P_event(Event::LIFT_DOWN, m_eventData, args);
}

Ctrl::Error Ctrl::P_fsm(Command cmd, const CommandData & data, void * args)
{
    State next_state = m_state;
    switch(m_state)
    {
        case State::INIT:
        {
             m_state_errors = 0;
             m_state_warnings = 0;
             m_state_allowed_autostop = false;
             m_state_autostop_triggered = false;
             m_speed_manual_delta = 0;
             m_speed_free = 0;
             m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
             P_event_motor_off(args);
             P_event_lift_up(args);
             Ctrl::speed_t speed = P_speed_baseline_get();
             P_event_motor_setpoint_update(speed, args);
             next_state = State::NORMAL_STOPPED;
            break;
        }

        case State::NORMAL_STOPPED:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    break;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    next_state = State::SERVICE_MODE1_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    next_state = State::SERVICE_MODE2_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    next_state = State::SERVICE_MODE3_STOPPED;
                    break;
                }
                case Command::START:
                {
                    if(!(m_state_allowed_autostop && m_state_autostop_triggered))
                    {
                        Ctrl::speed_t speed = P_speed_baseline_get();
                        P_event_motor_setpoint_update(speed, args);
                        P_event_motor_on(args);
                        P_event_lift_down(args);
                        next_state = State::NORMAL_STARTED_AUTO;
                    }
                    break;
                }
                case Command::STOP:
                {
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    m_speed_free = data.SPEED_FREE_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    break;
                }
            }
            break;
        }

        case State::NORMAL_STARTED_AUTO:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    break;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    return Error::INVALID_MODE;
                }
                case Command::START:
                {
                    Ctrl::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                    P_event_motor_setpoint_update(speed, args);
                    next_state = State::NORMAL_STARTED_MANUAL;
                    break;
                }
                case Command::STOP:
                {
                    P_event_warnings_clean(CTRL_WARNING_SPEED_TOO_LOW | CTRL_WARNING_SPEED_TOO_HIGH, args);
                    P_event_motor_off(args);
                    P_event_lift_up(args);
                    next_state = State::NORMAL_STOPPED;
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    Ctrl::speed_t speed = P_speed_baseline_get();
                    P_event_motor_setpoint_update(speed, args);
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    Ctrl::speed_t speed = P_speed_baseline_get();
                    P_event_motor_setpoint_update(speed, args);
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    m_speed_free = data.SPEED_FREE_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    if(m_state_autostop_triggered)
                    {
                        P_event_motor_off(args);
                        P_event_lift_up(args);
                    }
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    if(m_state_allowed_autostop)
                    {
                        P_event_motor_off(args);
                        P_event_lift_up(args);
                        next_state = State::NORMAL_STOPPED;
                    }
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    if(m_state_allowed_autostop)
                    {
                        P_event_motor_off(args);
                        P_event_lift_up(args);
                        next_state = State::NORMAL_STOPPED;
                    }
                    break;
                }
            }
            break;
        }

        case State::NORMAL_STARTED_MANUAL:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    break;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    return Error::INVALID_MODE;
                }
                case Command::START:
                {
                    Ctrl::speed_t speed = P_speed_baseline_get();
                    P_event_motor_setpoint_update(speed, args);
                    next_state = State::NORMAL_STARTED_AUTO;
                    break;
                }
                case Command::STOP:
                {
                    P_event_warnings_clean(CTRL_WARNING_SPEED_TOO_LOW | CTRL_WARNING_SPEED_TOO_HIGH, args);
                    P_event_motor_off(args);
                    P_event_lift_up(args);
                    next_state = State::NORMAL_STOPPED;
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    Ctrl::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                    P_event_motor_setpoint_update(speed, args);
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    Ctrl::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                    P_event_motor_setpoint_update(speed, args);
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    if(m_speed_manual_delta != data.SPEED_MANUAL_UPDATE.speed)
                    {
                        m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                        Ctrl::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                        P_event_motor_setpoint_update(speed, args);
                    }
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    m_speed_free = data.SPEED_FREE_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    if(m_state_autostop_triggered)
                    {
                        P_event_motor_off(args);
                        P_event_lift_up(args);
                    }
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    if(m_state_allowed_autostop)
                    {
                        P_event_motor_off(args);
                        P_event_lift_up(args);
                        next_state = State::NORMAL_STOPPED;
                    }
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    if(m_state_allowed_autostop)
                    {
                        P_event_motor_off(args);
                        P_event_lift_up(args);
                        next_state = State::NORMAL_STOPPED;
                    }
                    break;
                }
            }
            break;
        }

        case State::SERVICE_MODE1_STOPPED:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    next_state = State::NORMAL_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    break;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    next_state = State::SERVICE_MODE2_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    next_state = State::SERVICE_MODE3_STOPPED;
                    break;
                }
                case Command::START:
                {
                    P_event_lift_down(args);
                    next_state = State::SERVICE_MODE1_STARTED;
                    break;
                }
                case Command::STOP:
                {
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    m_speed_free = data.SPEED_FREE_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    break;
                }
            }
            break;
        }

        case State::SERVICE_MODE1_STARTED:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    break;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    return Error::INVALID_MODE;
                }
                case Command::START:
                {
                    break;
                }
                case Command::STOP:
                {
                    P_event_lift_up(args);
                    next_state = State::SERVICE_MODE1_STOPPED;
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    m_speed_free = data.SPEED_FREE_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    break;
                }
            }
            break;
        }

        case State::SERVICE_MODE2_STOPPED:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    next_state = State::NORMAL_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    next_state = State::SERVICE_MODE1_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    break;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    next_state = State::SERVICE_MODE3_STOPPED;
                    break;
                }
                case Command::START:
                {
                    Ctrl::speed_t speed = P_speed_baseline_get();
                    P_event_motor_setpoint_update(speed, args);
                    P_event_motor_on(args);
                    next_state = State::SERVICE_MODE2_STARTED;
                    break;
                }
                case Command::STOP:
                {
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    m_speed_free = data.SPEED_FREE_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    break;
                }
            }
            break;
        }

        case State::SERVICE_MODE2_STARTED:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    break;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    return Error::INVALID_MODE;
                }
                case Command::START:
                {
                    break;
                }
                case Command::STOP:
                {
                    P_event_motor_off(args);
                    next_state = State::SERVICE_MODE2_STOPPED;
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    m_speed_free = data.SPEED_FREE_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    break;
                }
            }
            break;
        }

        case State::SERVICE_MODE3_STOPPED:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    next_state = State::NORMAL_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    next_state = State::SERVICE_MODE1_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    next_state = State::SERVICE_MODE2_STOPPED;
                    break;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    break;
                }
                case Command::START:
                {
                    Ctrl::speed_t speed = m_speed_free;
                    P_event_motor_setpoint_update(speed, args);
                    P_event_motor_on(args);
                    next_state = State::SERVICE_MODE3_STARTED;
                    break;
                }
                case Command::STOP:
                {
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    if(m_speed_free != data.SPEED_FREE_UPDATE.speed)
                    {
                        m_speed_free = data.SPEED_FREE_UPDATE.speed;
                        Ctrl::speed_t speed = m_speed_free;
                        P_event_motor_setpoint_update(speed, args);
                    }
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    break;
                }
            }
            break;
        }

        case State::SERVICE_MODE3_STARTED:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::ENTER_MODE_NORMAL:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE1:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE2:
                {
                    return Error::INVALID_MODE;
                }
                case Command::ENTER_SERVICE_MODE3:
                {
                    break;
                }
                case Command::START:
                {
                    break;
                }
                case Command::STOP:
                {
                    P_event_motor_off(args);
                    next_state = State::SERVICE_MODE3_STOPPED;
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::SPEED_FREE_UPDATE:
                {
                    m_speed_free = data.SPEED_FREE_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    break;
                }
            }
            break;
        }

    }

    m_state = next_state;
    return Error::OK;
}

Ctrl::Ctrl(
        speed_t baselineSpeedLow,
        speed_t baselineSpeedHigh,
        void (*eventFunc)(Event event, const EventData& data, void * args),
        void * args
)
{
    /* init vars */
    m_speed_baselines[ARRAY_INDEX(BaselineSpeedMode::MODE_LOW )] = baselineSpeedLow;
    m_speed_baselines[ARRAY_INDEX(BaselineSpeedMode::MODE_HIGH)] = baselineSpeedHigh;
    m_eventFunc = eventFunc;

    m_state = State::INIT;
    P_fsm(Command::INIT, m_cmdData, args);
}

Ctrl::~Ctrl()
{
    /* nothing to do*/
}

Ctrl::Error Ctrl::baselineSpeedModeSet(BaselineSpeedMode baselineSpeedMode, void * args)
{
    Command cmd;
    switch(baselineSpeedMode)
    {
        case BaselineSpeedMode::MODE_LOW : cmd = Command::SPEED_BASELINE_LOW; break;
        case BaselineSpeedMode::MODE_HIGH: cmd = Command::SPEED_BASELINE_HIGH; break;
        default: return Error::FATAL;
    }
    return P_fsm(cmd, m_cmdData, args);
}

Ctrl::Error Ctrl::speedManualSet(speed_t speed, void * args)
{
    m_cmdData.SPEED_MANUAL_UPDATE.speed = speed;
    return P_fsm(Command::SPEED_MANUAL_UPDATE, m_cmdData, args);
}

Ctrl::Error Ctrl::speedFreeSet(speed_t speed, void * args)
{
    m_cmdData.SPEED_FREE_UPDATE.speed = speed;
    return P_fsm(Command::SPEED_FREE_UPDATE, m_cmdData, args);
}

Ctrl::Error Ctrl::autostopAllowSet(bool allow_autostop, void * args)
{
    return P_fsm(allow_autostop ? Command::AUTOSTOP_ALLOW : Command::AUTOSTOP_DENY, m_cmdData, args);
}

Ctrl::Error Ctrl::start(void * args)
{
    return P_fsm(Command::START, m_cmdData, args);
}

Ctrl::Error Ctrl::stop(void * args)
{
    return P_fsm(Command::STOP, m_cmdData, args);
}

Ctrl::Error Ctrl::mode_normal(void * args)
{
    return P_fsm(Command::ENTER_MODE_NORMAL, m_cmdData, args);
}

Ctrl::Error Ctrl::mode_service_1(void * args)
{
    return P_fsm(Command::ENTER_SERVICE_MODE1, m_cmdData, args);
}

Ctrl::Error Ctrl::mode_service_2(void * args)
{
    return P_fsm(Command::ENTER_SERVICE_MODE2, m_cmdData, args);
}

Ctrl::Error Ctrl::mode_service_3(void * args)
{
    return P_fsm(Command::ENTER_SERVICE_MODE3, m_cmdData, args);
}

void Ctrl::actualSpeedUpdate(speed_t speed, void * args)
{
    speed_t setpoint = 0;
    bool check = false;
    switch(m_state)
    {
        case State::INIT: break;
        case State::NORMAL_STOPPED: break;
        case State::NORMAL_STARTED_AUTO:
        {
            setpoint = P_speed_baseline_get();
            check = true;
            break;
        }
        case State::NORMAL_STARTED_MANUAL:
        {
            setpoint = P_speed_baseline_get() + m_speed_manual_delta;
            check = true;
            break;
        }
        case State::SERVICE_MODE1_STOPPED: break;
        case State::SERVICE_MODE1_STARTED: break;
        case State::SERVICE_MODE2_STOPPED: break;
        case State::SERVICE_MODE2_STARTED:
        {
            setpoint = P_speed_baseline_get(); break;
            check = true;
            break;
        }
        case State::SERVICE_MODE3_STOPPED: break;
        case State::SERVICE_MODE3_STARTED:
        {
            setpoint = m_speed_free;
            check = true;
            break;
        }
    }

    if(check)
    {
        if(speed < setpoint)
        {
            P_event_warnings_set(CTRL_WARNING_SPEED_TOO_LOW, args);
        }
        else if(speed > setpoint)
        {
            P_event_warnings_set(CTRL_WARNING_SPEED_TOO_HIGH, args);
        }
        else
        {
            P_event_warnings_clean(CTRL_WARNING_SPEED_TOO_LOW | CTRL_WARNING_SPEED_TOO_HIGH, args);
        }
    }

}

Ctrl::Error Ctrl::stopTriggeredSet(bool triggered, void * args)
{
    return P_fsm(triggered ? Command::GAUGE_STOP_ON : Command::GAUGE_STOP_OFF, m_cmdData, args);
}

Ctrl::RunMode Ctrl::runModeGet()
{
    switch(m_state)
    {
        case State::INIT   : return RunMode::NORMAL_STOPPED;
        case State::NORMAL_STOPPED: return RunMode::NORMAL_STOPPED;
        case State::NORMAL_STARTED_AUTO: return RunMode::NORMAL_STARTED_AUTO;
        case State::NORMAL_STARTED_MANUAL: return RunMode::NORMAL_STARTED_MANUAL;
        case State::SERVICE_MODE1_STOPPED: return RunMode::SERVICE_MODE1_STOPPED;
        case State::SERVICE_MODE1_STARTED: return RunMode::SERVICE_MODE1_STARTED;
        case State::SERVICE_MODE2_STOPPED: return RunMode::SERVICE_MODE2_STOPPED;
        case State::SERVICE_MODE2_STARTED: return RunMode::SERVICE_MODE2_STARTED;
        case State::SERVICE_MODE3_STOPPED: return RunMode::SERVICE_MODE3_STOPPED;
        case State::SERVICE_MODE3_STARTED: return RunMode::SERVICE_MODE3_STARTED;
    }
    return RunMode::NORMAL_STOPPED;
}

int Ctrl::errorsGet() const
{
    return m_state_errors;
}

int Ctrl::warningsGet() const
{
    return m_state_warnings;
}

#ifdef CTRL_DEBUG

void Ctrl::debug_get(internal_state_t * state) const
{
    state->m_state = m_state;
    state->m_speed_manual_delta = m_speed_manual_delta;
}
#endif

}
