/**
 * @file main.cpp
 *
 */

#include "GDInputDebounced.hpp"
#include "Blinker.hpp"

void onTriggeredOn(){

}
void onTriggeredOff(){

}

class A
{
public:
    A(){};
    ~A(){};
    void do_x(){};
};

class B
{
public:
    B(){};
    ~B(){};

    static A x;
    void x_do_x()
    {
        x.do_x();
    }
};

A B::x;

int main()
{

    B b;
    b.x_do_x();
    //b.x.do_x();
/*
    GDInputDebounced di(
            true,
            onTriggeredOn,
            onTriggeredOff,
            10
    );
*/

    static unsigned long pattern3[] =
    {
            0,
            250, /* OFF */
            250,
            250,
            250,
            250,
            250,
            250,
    };

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

    Blinker bl;
    bl.start(pattern3, ARRAY_SIZE(pattern3), false);

    bool end;
    bool light;
    unsigned long wait_time;

    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);
    bl.task(&end, &light, &wait_time);

}


