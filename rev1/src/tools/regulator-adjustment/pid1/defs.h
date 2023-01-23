#ifndef DEFS_H
#define DEFS_H

#define SIMULATION_DT_MS 50 /* interval between simulation ticks. lower value -> faster simulation */
#define INTERVAL_MS 750 /* interval of measures */

#define MANUAL_SETPOINT_MIN (0)
#define MANUAL_SETPOINT_MAX (51)
#define MANUAL_SETPOINT_INITIAL_VALUE (0)
#define VIEW_PV_MIN (MANUAL_SETPOINT_MIN)
#define VIEW_PV_MAX (MANUAL_SETPOINT_MAX + 5)
#define AXIS_Y_MIN (VIEW_PV_MAX)
#define AXIS_Y_MAX (VIEW_PV_MIN)


#define ENGINE_POWER_MIN (0.0)
#define ENGINE_POWER_MAX (1000.0)

/* seconds */
#define AXIS_X_MIN (0)
#define AXIS_X_MAX (15)

#if defined(PID_DISCRETE)

#   define PIDK_SELECTION_MIN (-100.0)
#   define PIDK_SELECTION_MAX (100.0)
#   define PIDK_SELECTION_VALUE_DEFAULT_MIN (0.0)
#   define PIDK_SELECTION_VALUE_DEFAULT_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.00488281)
#   define PID_Kd (0.07934570)

#elif defined(PID_RECURRENT)

#   define PIDK_SELECTION_MIN (-100.0)
#   define PIDK_SELECTION_MAX (100.0)
#   define PIDK_SELECTION_VALUE_DEFAULT_MIN (0.0)
#   define PIDK_SELECTION_VALUE_DEFAULT_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.00488281)
#   define PID_Kd (0.07934570)

#elif defined(PID_RECURRENT_FIXED32)

#   define PIDK_SELECTION_MIN (-1000.0)
#   define PIDK_SELECTION_MAX (1000.0)
#   define PIDK_SELECTION_VALUE_DEFAULT_MIN (0.0)
#   define PIDK_SELECTION_VALUE_DEFAULT_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.01464844)
#   define PID_Kd (0.11718750)

#endif

#endif // DEFS_H

