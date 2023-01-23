#ifndef GDINPUT_H_
#define GDINPUT_H_

#include "GObject.hpp"

class GDInput : public GObject
{
public:
    GDInput() = delete;
    GDInput(const GDInput&) = delete;
    GDInput& operator=(const GDInput&) = delete;

    GDInput(
            bool initState,
            void (*onTriggeredOn)(void * args),
            void (*onTriggeredOff)(void * args),
            void * args
    );
    virtual ~GDInput();
    virtual void stateSet(bool state, void * args);
private:
    bool m_state_prev;

    void (*m_onTriggeredOn)(void * args);
    void (*m_onTriggeredOff)(void * args);
};

#endif /* GDINPUT_H_ */
