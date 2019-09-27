
#include "G602.hpp"

#define ARRAY_INDEX(x) static_cast<unsigned int>(x)

namespace app
{

G602::speed_t G602::P_speed_baseline_get() const
{
    return m_speed_baselines[ARRAY_INDEX(m_speed_baseline_mode)];
}

void G602::P_event(Event event, const EventData& data) const
{
    if(m_eventFunc)
    {
        m_eventFunc(event, data);
    }
}

void G602::P_event_errors_set(unsigned err)
{
    unsigned old = m_state_errors;
    m_state_errors |= err;
    if(m_state_errors != old)
    {
        m_eventData.ERRORS_UPDATE.errors = m_state_errors;
        P_event(Event::ERRORS_UPDATE, m_eventData);
    }
}

void G602::P_event_errors_clear(unsigned err)
{
    unsigned old = m_state_errors;
    m_state_errors &= ~err;
    if(m_state_errors != old)
    {
        m_eventData.ERRORS_UPDATE.errors = m_state_errors;
        P_event(Event::ERRORS_UPDATE, m_eventData);
    }
}

void G602::P_event_warnings_set(unsigned warn)
{
    unsigned old = m_state_warnings;
    m_state_warnings |= warn;
    if(m_state_warnings != old)
    {
        m_eventData.WARNINGS_UPDATE.warnings = m_state_warnings;
        P_event(Event::WARNINGS_UPDATE, m_eventData);
    }
}

void G602::P_event_warnings_clean(unsigned warn)
{
    unsigned old = m_state_warnings;
    m_state_warnings &= ~warn;
    if(m_state_warnings != old)
    {
        m_eventData.WARNINGS_UPDATE.warnings = m_state_warnings;
        P_event(Event::WARNINGS_UPDATE, m_eventData);
    }
}

void G602::P_event_motor_on()
{
    P_event(Event::MOTOR_ON, m_eventData);
}

void G602::P_event_motor_off()
{
    P_event(Event::MOTOR_OFF, m_eventData);
}

void G602::P_event_motor_setpoint_update(G602::speed_t setpoint)
{
    m_eventData.DRIVE_SETPOINT_UPDATE.setpoint = setpoint;
    P_event(Event::MOTOR_SETPOINT_UPDATE, m_eventData);
}

void G602::P_event_lift_up()
{
    P_event(Event::LIFT_UP, m_eventData);
}

void G602::P_event_lift_down()
{
    P_event(Event::LIFT_DOWN, m_eventData);
}

void G602::P_fsm(Command cmd, const CommandData & data)
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
             G602::speed_t speed = P_speed_baseline_get();
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
                        G602::speed_t speed = P_speed_baseline_get();
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
                    G602::speed_t speed = P_speed_baseline_get();
                    P_event_motor_setpoint_update(speed);
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    G602::speed_t speed = P_speed_baseline_get();
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
                    G602::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
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
                    G602::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                    P_event_motor_setpoint_update(speed);
                    break;
                }
                case Command::SPEED_BASELINE_HIGH:
                {
                    m_speed_baseline_mode = BaselineSpeedMode::MODE_HIGH;
                    G602::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
                    P_event_motor_setpoint_update(speed);
                    break;
                }
                case Command::SPEED_MANUAL_UPDATE:
                {
                    if(m_speed_manual_delta != data.SPEED_MANUAL_UPDATE.speed)
                    {
                        m_speed_manual_delta = data.SPEED_MANUAL_UPDATE.speed;
                        G602::speed_t speed = P_speed_baseline_get() + m_speed_manual_delta;
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
                    G602::speed_t speed = P_speed_baseline_get();
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

G602::G602(
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

G602::~G602()
{
    /* nothing to do*/
}

void G602::baselineSpeedModeSet(BaselineSpeedMode baselineSpeedMode)
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

void G602::manualSpeedDeltaSet(speed_t speed)
{
    m_cmdData.SPEED_MANUAL_UPDATE.speed = speed;
    P_fsm(Command::SPEED_MANUAL_UPDATE, m_cmdData);
}

void G602::autostopAllowSet(bool allow_autostop)
{
    P_fsm(allow_autostop ? Command::AUTOSTOP_ALLOW : Command::AUTOSTOP_DENY, m_cmdData);
}

void G602::start()
{
    P_fsm(Command::START, m_cmdData);
}

void G602::stop()
{
    P_fsm(Command::STOP, m_cmdData);
}

void G602::actualSpeedUpdate(speed_t speed)
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
            P_event_warnings_set(WARNING_SPEED_TOO_LOW);
        }
        else if(speed > setpoint)
        {
            P_event_warnings_set(WARNING_SPEED_TOO_HIGH);
        }
        else
        {
            clear = true;
        }
    }

    if(clear)
    {
        P_event_warnings_clean(WARNING_SPEED_TOO_LOW | WARNING_SPEED_TOO_HIGH);
    }

}

void G602::stopTriggeredSet(bool triggered)
{
    P_fsm(triggered ? Command::GAUGE_STOP_ON : Command::GAUGE_STOP_OFF, m_cmdData);
}

int G602::errorsGet() const
{
    return m_state_errors;
}

int G602::warningsGet() const
{
    return m_state_warnings;
}

#ifdef G602_DEBUG

void G602::debug_get(internal_state_t * state) const
{
    state->m_state = m_state;
    state->m_speed_manual_delta = m_speed_manual_delta;
}
#endif

}
