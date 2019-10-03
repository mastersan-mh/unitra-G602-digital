#include "ctestmotor.h"

#include <math.h>

CTestMotor::CTestMotor(double braking_strength)
{
    m_braking_strength = braking_strength;
    m_speed = 0.0;
}

CTestMotor::~CTestMotor()
{

}

/**
 * @param power = [0; 1]
 * @return speed
 */
double CTestMotor::process(double power)
{

    bool rev = (power < 0.0);
    double dspeed;
    if(!rev)
    {
        dspeed = log(power + 1) * 0.1;
    }
    else
    {
        power = -power;
        dspeed = -log(power + 1) * 0.05;
    }

    m_speed -= m_braking_strength;
    m_speed += dspeed;

    if(m_speed < 0.0)
    {
        m_speed = 0.0;
    }
    return m_speed;
}
