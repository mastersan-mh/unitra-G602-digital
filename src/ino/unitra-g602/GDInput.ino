
#include "GDInput.hpp"

GDInput::GDInput(
            bool initState,
            void (*onTriggeredOn)(void * args),
            void (*onTriggeredOff)(void * args),
            void * args
)
: GObject()
{
    m_state_prev = !initState;
    m_onTriggeredOn = onTriggeredOn;
    m_onTriggeredOff = onTriggeredOff;
    stateSet(initState, args);
}

GDInput::~GDInput()
{
    /* nothing */
}

void GDInput::stateSet(bool state, void * args)
{
    if(state)
    {
        if(!m_state_prev)
        {
            if(m_onTriggeredOn != nullptr)
            {
                m_onTriggeredOn(args);
            }
        }
    }
    else
    {
        if(m_state_prev)
        {
            if(m_onTriggeredOff != nullptr)
            {
                m_onTriggeredOff(args);
            }
        }
    }

    m_state_prev = state;
}
