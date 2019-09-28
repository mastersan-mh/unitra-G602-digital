
#include "GDInput.hpp"

GDInput::GDInput(
            bool initState,
            void (*onTriggeredOn)(),
            void (*onTriggeredOff)()
)
: GObject()
{
    this->state_prev = !initState;
    this->onTriggeredOn = onTriggeredOn;
    this->onTriggeredOff = onTriggeredOff;
    stateSet(initState);
}

GDInput::~GDInput()
{
    /* nothing */
}

void GDInput::stateSet(bool state)
{
    if(state)
    {
        if(!state_prev)
        {
            if(onTriggeredOn != nullptr)
            {
                onTriggeredOn();
            }
        }
    }
    else
    {
        if(state_prev)
        {
            if(onTriggeredOff != nullptr)
            {
                onTriggeredOff();
            }
        }
    }

    state_prev = state;
}
