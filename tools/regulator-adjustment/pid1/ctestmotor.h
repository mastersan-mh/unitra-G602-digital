#ifndef CTESTMOTOR_H
#define CTESTMOTOR_H

/*
Kp = 3.750000000
Ki =
Kd =
*/

class CTestMotor
{
public:
    explicit CTestMotor(double braking_strength);
    ~CTestMotor();
    double process(double power);

private:
    double m_speed;
    double m_braking_strength;
};

#endif // CTESTMOTOR_H
