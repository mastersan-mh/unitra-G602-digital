#ifndef PID_RECURRENT_H_
#define PID_RECURRENT_H_

#pragma once

/**
 * настройка ПИД-регулятора
 * http://www.kb-agava.ru/recomendacii_nastroyka_pid_regulyatora_kontrollera_agava_6432_20
 */


#if defined(PID_DISCRETE)

#   include <nostd/regulatorpid/regulatorpiddiscrete.hpp>
typedef nostd::PidDiscrete< double, 50 > PID;

#elif defined(PID_RECURRENT)

#   include <nostd/regulatorpid/regulatorpidrecurrent.hpp>
typedef nostd::PidRecurrent< double > PID_recurrent;

#elif defined(PID_RECURRENT_FIXED32)

#   include <nostd/fixed/fixed32>
#   include <nostd/regulatorpid/regulatorpidrecurrent.hpp>
typedef nostd::PidRecurrent< nostd::Fixed32 > PID_recurrent_Fixed32;

#endif

#endif /* PID_RECURRENT_H_ */
