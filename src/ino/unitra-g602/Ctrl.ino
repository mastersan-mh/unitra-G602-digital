
#include "Ctrl.hpp"

#define ARRAY_INDEX(x) static_cast<unsigned int>(x)

namespace app
{

Ctrl::speed_t Ctrl::P_speed_baseline_get() const
{
    return m_speed_baselines[ARRAY_INDEX(m_speed_baseline_mode)];
}

void Ctrl::P_event(Event event, const EventData& data) const
{
    if(m_eventFunc)
    {
        m_eventFunc(event, data);
    }
}

void Ctrl::P_event_errors_set(unsigned err)
{
    unsigned old = m_state_errors;
    m_state_errors |= err;
    if(m_state_errors != old)
    {
        m_eventData.ERRORS_UPDATE.errors = m_state_errors;
        P_event(Event::ERRORS_UPDATE, m_eventData);
    }
}

void Ctrl::P_event_errors_clear(unsigned err)
{
    unsigned old = m_state_errors;
    m_state_errors &= ~err;
    if(m_state_errors != old)
    {
        m_eventData.ERRORS_UPDATE.errors = m_state_errors;
        P_event(Event::ERRORS_UPDATE, m_eventData);
    }
}

void Ctrl::P_event_warnings_set(unsigned warn)
{
    unsigned old = m_state_warnings;
    m_state_warnings |= warn;
    if(m_state_warnings != old)
    {
        m_eventData.WARNINGS_UPDATE.warnings = m_state_warnings;
        P_event(Event::WARNINGS_UPDATE, m_eventData);
    }
}

void Ctrl::P_event_warnings_clean(unsigned warn)
{
    unsigned old = m_state_warnings;
    m_state_warnings &= ~warn;
    if(m_state_warnings != old)
    {
        m_eventData.WARNINGS_UPDATE.warnings = m_state_warnings;
        P_event(Event::WARNINGS_UPDATE, m_eventData);
    }
}

void Ctrl::P_event_motor_on()
{
    P_event(Event::MOTOR_ON, m_eventData);
}

void Ctrl::P_event_motor_off()
{
    P_event(Event::MOTOR_OFF, m_eventData);
}

void Ctrl::P_event_motor_setpoint_update(Ctrl::speed_t setpoint)
{
    m_eventData.DRIVE_SETPOINT_UPDATE.setpoint = setpoint;
    P_event(Event::MOTOR_SETPOINT_UPDATE, m_eventData);
}

void Ctrl::P_event_lift_up()
{
    P_event(Event::LIFT_UP, m_eventData);
}

void Ctrl::P_event_lift_down()
{
    P_event(Event::LIFT_DOWN, m_eventData);
}

void Ctrl::P_fsm(Command cmd, const CommandData & data)
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
             m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
             P_event_motor_off();
             P_event_lift_up();
             Ctrl::speed_t speed = P_speed_baseline_get();
             P_event_motor_setpoint_update(speed);
             next_state = State::STOPPED;
            break;
        }

        case State::STOPPED:
        {
            switch(cmd)
            {
                case Command::INIT:
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
                case Command::START:
                {
                    if(!(m_state_allowed_autostop && m_state_autostop_triggered))
                    {
                        Ctrl::speed_t speed = P_speed_baseline_get();
                        P_event_motor_setpoint_update(speed);
                        P_event_motor_on();
                        P_event_lift_down();
                        next_state = State::STARTED;
                    }
                    break;
                }
                case Command::STOP:
                {
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

        case State::STARTED:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    Ctrl::speed_t speed = P_speed_baseline_get();
                    P_event_motor_setpoint_update(speed);
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    Ctrl::speed_t speed = P_speed_baseline_get();
                    P_event_motor_setpoint_update(speed);
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    if(m_state_autostop_triggered)
                    {
                        P_event_motor_off();
                        P_event_lift_up();
                    }
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::START:
                {
                    Ctrl::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                    P_event_motor_setpoint_update(speed);
                    next_state = State::STARTED_MANUAL;
                    break;
                }
                case Command::STOP:
                {
                    P_event_motor_off();
                    P_event_lift_up();
                    next_state = State::STOPPED;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    if(m_state_allowed_autostop)
                    {
                        P_event_motor_off();
                        P_event_lift_up();
                        next_state = State::STOPPED;
                    }
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    if(m_state_allowed_autostop)
                    {
                        P_event_motor_off();
                        P_event_lift_up();
                        next_state = State::STOPPED;
                    }
                    break;
                }
            }
            break;
        }

        case State::STARTED_MANUAL:
        {
            switch(cmd)
            {
                case Command::INIT:
                {
                    break;
                }
                case Command::SPEED_BASELINE_LOW:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_LOW;
                    Ctrl::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                    P_event_motor_setpoint_update(speed);
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    Ctrl::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                    P_event_motor_setpoint_update(speed);
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    if(m_speed_manual_delta != data.SPEED_MANUAL_UPDATE.speed)
                    {
                        m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                        Ctrl::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                        P_event_motor_setpoint_update(speed);
                    }
                    break;
                }
                case Command::AUTOSTOP_ALLOW:
                {
                    m_state_allowed_autostop = true;
                    if(m_state_autostop_triggered)
                    {
                        P_event_motor_off();
                        P_event_lift_up();
                    }
                    break;
                }
                case Command::AUTOSTOP_DENY:
                {
                    m_state_allowed_autostop = false;
                    break;
                }
                case Command::START:
                {
                    Ctrl::speed_t speed = P_speed_baseline_get();
                    P_event_motor_setpoint_update(speed);
                    next_state = State::STARTED;
                    break;
                }
                case Command::STOP:
                {
                    P_event_motor_off();
                    P_event_lift_up();
                    next_state = State::STOPPED;
                    break;
                }
                case Command::GAUGE_STOP_ON:
                {
                    m_state_autostop_triggered = true;
                    if(m_state_allowed_autostop)
                    {
                        P_event_motor_off();
                        P_event_lift_up();
                        next_state = State::STOPPED;
                    }
                    break;
                }
                case Command::GAUGE_STOP_OFF:
                {
                    m_state_autostop_triggered = false;
                    if(m_state_allowed_autostop)
                    {
                        P_event_motor_off();
                        P_event_lift_up();
                        next_state = State::STOPPED;
                    }
                    break;
                }
            }
            break;
        }
    }

    m_state = next_state;
}

Ctrl::Ctrl(
        speed_t baseSpeedLow,
        speed_t baseSpeedHigh,
        void (*eventFunc)(Event event, const EventData& data)
)
{
    /* init vars */
    m_speed_baselines[ARRAY_INDEX(BaselineSpeedMode::MODE_LOW )] = baseSpeedLow;
    m_speed_baselines[ARRAY_INDEX(BaselineSpeedMode::MODE_HIGH)] = baseSpeedHigh;
    m_eventFunc = eventFunc;

    m_state = State::INIT;
    P_fsm(Command::INIT, m_cmdData);
}

Ctrl::~Ctrl()
{
    /* nothing to do*/
}

void Ctrl::baselineSpeedModeSet(BaselineSpeedMode baselineSpeedMode)
{
    Command cmd;
    switch(baselineSpeedMode)
    {
        case BaselineSpeedMode::MODE_LOW : cmd = Command::SPEED_BASELINE_LOW; break;
        case BaselineSpeedMode::MODE_HIGH: cmd = Command::SPEED_BASELINE_HIGH; break;
        default: return;
    }
    P_fsm(cmd, m_cmdData);
}

void Ctrl::manualSpeedDeltaSet(speed_t speed)
{
    m_cmdData.SPEED_MANUAL_UPDATE.speed = speed;
    P_fsm(Command::SPEED_MANUAL_UPDATE, m_cmdData);
}

void Ctrl::autostopAllowSet(bool allow_autostop)
{
    P_fsm(allow_autostop ? Command::AUTOSTOP_ALLOW : Command::AUTOSTOP_DENY, m_cmdData);
}

void Ctrl::start()
{
    P_fsm(Command::START, m_cmdData);
}

void Ctrl::stop()
{
    P_fsm(Command::STOP, m_cmdData);
}

void Ctrl::actualSpeedUpdate(speed_t speed)
{
    bool check = false;
    speed_t setpoint = 0;
    switch(m_state)
    {
        case State::INIT: break;
        case State::STOPPED: break;
        case State::STARTED:
        {
            setpoint = P_speed_baseline_get();
            check = true;
            break;
        }
        case State::STARTED_MANUAL:
        {
            setpoint = P_speed_baseline_get() + m_speed_manual_delta;
            check = true;
            break;
        }
    }

    bool clear = false;
    if(!check)
    {
        clear = true;
    }
    else
    {
        if(speed < setpoint)
        {
            P_event_warnings_set(CTRL_WARNING_SPEED_TOO_LOW);
        }
        else if(speed > setpoint)
        {
            P_event_warnings_set(CTRL_WARNING_SPEED_TOO_HIGH);
        }
        else
        {
            clear = true;
        }
    }

    if(clear)
    {
        P_event_warnings_clean(CTRL_WARNING_SPEED_TOO_LOW | CTRL_WARNING_SPEED_TOO_HIGH);
    }

}

void Ctrl::stopTriggeredSet(bool triggered)
{
    P_fsm(triggered ? Command::GAUGE_STOP_ON : Command::GAUGE_STOP_OFF, m_cmdData);
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
