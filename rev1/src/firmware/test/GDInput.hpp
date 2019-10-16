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
            void (*onTriggeredOn)(),
            void (*onTriggeredOff)()
    );
    virtual ~GDInput();
    virtual void stateSet(bool state);
private:
    void (*onTriggeredOn)();
    void (*onTriggeredOff)();
    bool state_prev;
};

#endif /* GDINPUT_H_ */
