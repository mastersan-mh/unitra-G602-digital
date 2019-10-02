#include "pid/pidadjustp.h"

PIDAdjustP::PIDAdjustP(unsigned len_max)
    : m_len_max(len_max)
    , m_state(State::INIT)
    , m_measure_start_time(0.0)
    , m_time(0.0)
    , m_accumulated_values(0)
{
    P_fsm(Command::INIT, nullptr);
}

PIDAdjustP::~PIDAdjustP()
{

}

void PIDAdjustP::handle(double time, double value)
{
    m_time = time;
    union Data data;
    data.APPEND.value = value;
    P_fsm(Command::APPEND, &data);
}

void PIDAdjustP::P_fsm(Command cmd, union Data * data)
{
    State next_state;
    switch(m_state)
    {
        case State::INIT:
        {
            m_measure_start_time = m_time;
            next_state = State::MEASURE;
            break;
        }
        case State::MEASURE:
        {

            ++m_accumulated_values;
            m_values.push_back(data->APPEND.value);

            if(m_accumulated_values > m_len_max)
            {
                --m_accumulated_values;
                /* erase old values */
                m_values.pop_front();
            }


            break;
        }
        case State::SELECTION:
        {
            break;
        }
    }
    m_state = next_state;
}
