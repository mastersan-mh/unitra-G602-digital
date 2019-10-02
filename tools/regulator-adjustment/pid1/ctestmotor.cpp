#include "ctestmotor.h"

#include <math.h>

CTestMotor::CTestMotor(double unpower_)
{
    unpower = unpower_;
    speed = 0.0;
}

CTestMotor::~CTestMotor()
{

}

/**
 * @param power = [0; 1]
 * @return speed
 */
double CTestMotor::process(double power_)
{

    bool rev = (power_ < 0.0);
    double dspeed;
    if(!rev)
    {
        dspeed = log(power_ + 1);
    }
    else
    {
        power_ = -power_;
        dspeed = -log(power_ + 1);
    }

    speed -= unpower;
    speed += dspeed;

    if(speed < 0.0)
    {
        speed = 0.0;
    }
    return speed;
}
