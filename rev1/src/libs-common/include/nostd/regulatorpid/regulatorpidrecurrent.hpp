#ifndef NOSTD_REGULATOR_PID_RECURRENT_HPP_
#define NOSTD_REGULATOR_PID_RECURRENT_HPP_

namespace nostd
{
/**
 * @brief Proportional-Integral-Difference recurrent regulator.
 * @param value_type    The values type (double, for example).
 */
template < class value_type >
class PidRecurrent
{
public:
    PidRecurrent()
    {
        reset();
    }

    PidRecurrent(value_type Kp, value_type Ki_discr, value_type Kd_discr) :
        m_Kp(Kp),
        m_Ki_discr(Ki_discr),
        m_Kd_discr(Kd_discr)
    {
        reset();
    }

    virtual ~PidRecurrent()
    {
    }

    /**
     * @brief вычисление нового управляющего воздествия
     * @note Данную функцию необходимо вызывать через РАВНЫЕ промежутки времени, поскольку мы не знаем ничего о dt (время)
     */
    value_type calculate(value_type setpoint, value_type processVariable)
    {
        /*

        реккурентная формула

        u[n] = u[n-1]
        + Kp*(E[n] - E[n-1])
        + Ki_discr*E[n]
        + Kd_discr*(E[n] - 2*E[n-1] + E[n-2])

        Ki_discr = Kp*Kip*Time
        Kd_discr = (Kp*Kdp) / Time

         */

        value_type error_N = setpoint - processVariable;

        value_type P = m_Kp * (error_N - m_error_N_minus_1);
        value_type I = m_u_prev + m_Ki_discr * error_N;
        value_type D = m_Kd_discr * (error_N - (m_error_N_minus_1 + m_error_N_minus_1) + m_error_N_minus_2);

        value_type u = P + I + D;

        m_u_prev = u;

        m_error_N_minus_2 = m_error_N_minus_1;
        m_error_N_minus_1 = error_N;

        return u;
    }

    void reset()
    {
        m_u_prev = value_type();
        m_error_N_minus_1 = value_type();
        m_error_N_minus_2 = value_type();
    }

    /**
     * @brief Установка коэффициентов
     * @note после вызова этих функций, необходимо вызвать reset(), для сброса внутреннего состояния и корректной работы регулятора
     */
    inline void KpSet(value_type value)
    {
        m_Kp = value;
    }

    inline void KiSet(value_type value)
    {
        m_Ki_discr = value;
    }

    inline void KdSet(value_type value)
    {
        m_Kd_discr = value;
    }

    inline value_type KpGet()
    {
        return m_Kp;
    }

    inline value_type KiGet()
    {
        return m_Ki_discr;
    }

    inline value_type KdGet()
    {
        return m_Kd_discr;
    }

private:

    value_type m_Kp{};
    value_type m_Ki_discr{};
    value_type m_Kd_discr{};

    value_type m_u_prev{};
    value_type m_error_N_minus_1{};
    value_type m_error_N_minus_2{};

};

}

#endif /* NOSTD_REGULATOR_PID_RECURRENT_HPP_ */
