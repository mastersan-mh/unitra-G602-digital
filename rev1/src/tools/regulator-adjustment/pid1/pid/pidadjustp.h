#ifndef PIDADJUSTP_H
#define PIDADJUSTP_H

#include <QVector>

class PIDAdjustP
{
private:
    static constexpr double m_measure_time = 1000.0; /* time, ms */
public:
    PIDAdjustP(unsigned len_max);
    ~PIDAdjustP() = default;
    void handle(double time, double value);
private:
    enum class State
    {
        INIT,
        MEASURE,
        SELECTION,
    };
    enum class Command
    {
        INIT,
        APPEND, /**< Append a value */
    };
    union Data
    {
        struct
        {
            double value;
        } APPEND;
    };

    unsigned m_len_max;
    State m_state = State::INIT;
    double m_measure_start_time = 0.0;
    double m_time = 0.0;
    unsigned m_accumulated_values = 0;
    QVector<double> m_values;

    void P_fsm(Command cmd, union Data * data);
};

#endif // PIDADJUSTP_H
