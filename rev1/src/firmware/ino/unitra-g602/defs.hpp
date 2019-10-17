#ifndef DEFS_H_
#define DEFS_H_

/* button debounce time, ms */
#define DI_DEBOUNCE_TIME (100UL)

#define DI_MOTOR_DEBOUNCE_TIME (2U)
#define DI_TABLE_DEBOUNCE_TIME (100U)

#define DI_POTENTIOMETER_FACTOR 10

/* irq 0 (digital pin 2) */
#define PIN_DI_2_INTERRUPT (0U)
/* irq 1 (digital pin 3) */
#define PIN_DI_3_INTERRUPT (1U)

#define PIN_DI_GAUGE_DRIVE_ROTATEPULSE (2U)
#define PIN_DI_GAUGE_TABLE_ROTATEPULSE (3U)
#define PIN_DI_GAUGE_STOP        (4U)
#define PIN_DI_BTN_45_33    (5U)
#define PIN_DI_BTN_AUTOSTOP (6U)
#define PIN_DI_BTN_START    (7U)
#define PIN_DI_BTN_STOP     (8U)
#define PIN_DO_ENGINE       (9U)
#define PIN_DO_LIFT         (10U)
#define PIN_DO_STROBE_ERROR (11U)
#define PIN_DO_AUDIO_DENY   (12U)

#define PIN_AI_POTENTIOMETER (PIN_A0)

#define G602_POTENTIOMETER_HALF 512 /* full scale of potentiometer = 1024 -> 1024/2 */
#define G602_POTENTIOMETER_MIN 0 /* 0 */
#define G602_POTENTIOMETER_MAX (G602_POTENTIOMETER_HALF * 2 - 1) /* full scale of potentiometer = 1024 -> 1024/2 */

#define G602_EVENT_MODE_CHANGED 0x00
#define G602_EVENT_SPPV 0x01

#define G602_SHEDULER_TASKS__NUM 3

/* Amount of impulses per revolution */
#define G602_ENGINE_PULSES_PER_REV 6
#define G602_TABLE_PULSES_PER_REV 8

/** @brief Speeds of table, rpm */
#define G602_TABLE_SPEED_LOW_RPM  (33)
#define G602_TABLE_SPEED_HIGH_RPM (45)

#define G602_SPEED_HALF          ((G602_SPEED_BASELINE_HIGH - G602_SPEED_BASELINE_LOW) / 2)
#define G602_SPEED_RANGE_MIN     (-G602_SPEED_HALF)
#define G602_SPEED_RANGE_MAX     (G602_SPEED_HALF)
#define G602_SPEED_MIN           (G602_SPEED_BASELINE_LOW - G602_SPEED_HALF)
#define G602_SPEED_BASELINE_LOW  (G602_TABLE_SPEED_LOW_RPM * G602_TABLE_PULSES_PER_REV)
#define G602_SPEED_BASELINE_HIGH (G602_TABLE_SPEED_HIGH_RPM * G602_TABLE_PULSES_PER_REV)
#define G602_SPEED_MAX           (G602_SPEED_BASELINE_HIGH + G602_SPEED_HALF)

/* Time(ms) for which <xppr> pulses are counted at the <xspeed> speed. */
#define G602_MINIMAL_TIME(xppr, xspeed) ((60UL * 1000UL * (unsigned long)(xppr)) / (unsigned long)(xspeed))

#define G602_MINIMAL_TIME_OF_MINIMAL_SPEED      G602_MINIMAL_TIME(G602_TABLE_PULSES_PER_REV, G602_SPEED_MIN)

#endif
