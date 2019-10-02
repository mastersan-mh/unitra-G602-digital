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
    explicit CTestMotor(double unpower_);
    ~CTestMotor();
    double process(double power_);

private:
    double speed;
    double unpower;
};

#endif // CTESTMOTOR_H
