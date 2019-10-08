#ifndef DEFS_H
#define DEFS_H

#define INTERVAL_MS 100

#if defined(PID_RECURRENT_FIXED32)
#   define MANUAL_SETPOINT_MAX (255)
#   define MANUAL_SETPOINT_MIN (0)
#   define MANUAL_SETPOINT_INITIAL_VALUE (0)
#   define AXIS_Y_MIN (MANUAL_SETPOINT_MAX)
#   define AXIS_Y_MAX (MANUAL_SETPOINT_MIN)
#else
#   define MANUAL_SETPOINT_MAX (255)
#   define MANUAL_SETPOINT_MIN (0)
#   define MANUAL_SETPOINT_INITIAL_VALUE (0)
#   define AXIS_Y_MIN (MANUAL_SETPOINT_MAX)
#   define AXIS_Y_MAX (MANUAL_SETPOINT_MIN)
#endif


#define ENGINE_POWER_MIN (0.0)
#define ENGINE_POWER_MAX (1000.0)

/* seconds */
#define AXIS_X_MIN (0)
#define AXIS_X_MAX (15)

#if defined(PID_DISCRETE)

#   define PIDK_SELECTION_MIN (0.0)
#   define PIDK_SELECTION_MAX (100.0)
#   define PIDK_SELECTION_RANGE_MIN (0.0)
#   define PIDK_SELECTION_RANGE_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.00488281)
#   define PID_Kd (0.07934570)

#elif defined(PID_RECURRENT)

#   define PIDK_SELECTION_MIN (0.0)
#   define PIDK_SELECTION_MAX (100.0)
#   define PIDK_SELECTION_RANGE_MIN (0.0)
#   define PIDK_SELECTION_RANGE_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.00488281)
#   define PID_Kd (0.07934570)

#elif defined(PID_RECURRENT_FIXED32)

#   define PIDK_SELECTION_MIN (0.0)
#   define PIDK_SELECTION_MAX (1000.0)
#   define PIDK_SELECTION_RANGE_MIN (0.0)
#   define PIDK_SELECTION_RANGE_MAX (10.0)
#   define PID_Kp (1.25000000)
#   define PID_Ki (0.01464844)
#   define PID_Kd (0.11718750)

#endif

#define RPC_FUNC_00_PULSES_R     0x00
#define RPC_FUNC_01_MODE_CURRENT_R 0x01
#define RPC_FUNC_02_KOEF_R         0x02
#define RPC_FUNC_03_KOEF_W         0x03
#define RPC_FUNC_04_SPEED_SP_R     0x04
#define RPC_FUNC_05_SPEED_SP_W     0x05
#define RPC_FUNC_06_SPEED_PV_R     0x06
#define RPC_FUNC_07_PROCESS_START  0x07
#define RPC_FUNC_08_PROCESS_STOP   0x08

#endif // DEFS_H

