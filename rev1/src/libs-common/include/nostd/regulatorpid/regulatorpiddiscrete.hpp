#ifndef NOSTD_REGULATOR_PID_DISCRETE_HPP_
#define NOSTD_REGULATOR_PID_DISCRETE_HPP_

#include <nostd/fastsum/fastsum>

namespace nostd
{

/**
 * настройка ПИД-регулятора
 * http://www.kb-agava.ru/recomendacii_nastroyka_pid_regulyatora_kontrollera_agava_6432_20
 */

template < class value_type, nostd::size_t intergal_num >
class PidDiscrete
{
    public:
        PidDiscrete(value_type Kp, value_type Ki_discr, value_type Kd_discr):
            m_Kp(Kp),
            m_Ki_discr(Ki_discr),
            m_Kd_discr(Kd_discr)
        {
            reset();
        }

        virtual ~PidDiscrete()
        {
        }

        /**
         * @brief вычисление нового управляющего воздествия
         * @note Данную функцию необходимо вызывать через РАВНЫЕ промежутки времени, поскольку мы не знаем ничего о dt (время)
         */
        value_type calculate(value_type setpoint, value_type processVariable)
        {

            value_type error = setpoint - processVariable;

            value_type P = m_Kp * error;

            integral_window.add(error);
            value_type I = m_Ki_discr * integral_window.value();

            value_type D = m_Kd_discr * (error - m_error_prev);

            value_type u = P + I + D;

            m_error_prev = error;

            return u;
        }

        void reset()
        {
            m_error_prev = 0.0;
            integral_window.reset();
        }

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

    private:

        value_type m_Kp;
        value_type m_Ki_discr;
        value_type m_Kd_discr;
        value_type m_error_prev;

        /* значения, из которых складывается интегральная часть */
        FastSum< value_type, intergal_num > integral_window;
};

}

#endif /* NOSTD_REGULATOR_PID_DISCRETE_HPP_ */
