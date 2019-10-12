#include <util/atomic.h>

#include <nostd-main.h>
#include <nostd.h>

#include "config.hpp"

#include "defs.hpp"

#include "AverageTinyMemory.hpp"

#include "G602.hpp"

#include "utils.hpp"

DEBUG_INIT_GLOBAL();

/*

/home/mastersan/progs/arduino-nightly/hardware/arduino/avr/cores/arduino
/home/mastersan/progs/arduino-nightly/hardware/arduino/avr/variants/standard
/home/mastersan/progs/arduino-nightly/hardware/tools/avr/avr/include




/home/mastersan/progs/arduino-nightly/hardware/tools/avr/bin/avr-g++
-c
-g
-Os
-w
-std=gnu++11
-fpermissive
-fno-exceptions
-ffunction-sections
-fdata-sections
-fno-threadsafe-statics
-Wno-error=narrowing
-flto
-w
-x c++
-E
-CC
-mmcu=atmega328p
-DF_CPU=16000000L
-DARDUINO=10807
-DARDUINO_AVR_NANO
-DARDUINO_ARCH_AVR
-I/home/mastersan/progs/arduino-nightly/hardware/arduino/avr/cores/arduino
-I/home/mastersan/progs/arduino-nightly/hardware/arduino/avr/variants/eightanaloginputs
/tmp/arduino_build_724020/sketch/unitra-g602.ino.cpp
-o /dev/null

*/

#define BOOL_TO_STR(xval) (xval ? "true" : "false")

typedef nostd::Fixed32 Fixed;

typedef struct
{
    bool resetted;
    unsigned motor_prev;
    unsigned table_prev;
} rotate_measures_t;

typedef struct
{
    rotate_measures_t rotate_measures[G602_ROTATE_MEASURES__NUM - 1];
} global_t;

static global_t global = {};

//#define rotate_pulse_counter_t unsigned int
typedef unsigned int rotate_pulse_counter_t;

void dump_u8(const uint8_t * data, unsigned size)
{
    unsigned i;
    DEBUG_PRINT("  size8 = ");
    DEBUG_PRINTLN(size);
    for(i = 0; i < size; ++i)
    {
        DEBUG_PRINT("  data8[");
        DEBUG_PRINT(i);
        DEBUG_PRINT("]=");
        DEBUG_PRINTLN(data[i]);
    }
}

void dump_u16(const uint16_t * data, unsigned size)
{
    unsigned i;
    DEBUG_PRINT("  size16 = ");
    DEBUG_PRINTLN(size);
    for(i = 0; i < size; ++i)
    {
        DEBUG_PRINT("  data16[");
        DEBUG_PRINT(i);
        DEBUG_PRINT("]=");
        DEBUG_PRINTLN(data[i]);
    }
}

static void terminate_handler(const char * file, unsigned int line, int error)
{
#define BLINK_PERIOD_MS 250
    static long time_prev = 0;
    static long time;
    static bool on = true;

    unsigned long x = (unsigned long)file + line + error; /* to suppress warnings */

    while(1)
    {
        time = millis();
        if(time - time_prev >= BLINK_PERIOD_MS)
        {
            time_prev = time;
            if(x == 0)
            {
                digitalWrite(PIN_DO_STROBE_ERROR, on ? HIGH : LOW);
            }
            digitalWrite(PIN_DO_STROBE_ERROR, on ? HIGH : LOW);
            on = !on;
        }
    }
}

static void P_event_strober(bool on)
{
    digitalWrite(PIN_DO_STROBE_ERROR, (on ? HIGH : LOW));
}

static void P_event_lift_up()
{
    digitalWrite(PIN_DO_LIFT, LOW);
    digitalWrite(PIN_DO_AUDIO_DENY, LOW);
}

static void P_event_lift_down()
{
    digitalWrite(PIN_DO_LIFT, HIGH);
    digitalWrite(PIN_DO_AUDIO_DENY, HIGH);
}

void P_motor_update(bool state, int setpoint)
{
    if(state)
    {
        /* [0; 255] */
        int value = (int)map(setpoint, G602_SPEED_MIN, G602_SPEED_MAX, 0, 200);
        analogWrite(PIN_DO_ENGINE, value);
    }
    else
    {
        analogWrite(PIN_DO_ENGINE, 0);
    }
}

static unsigned long time;
static unsigned long time_prev = 0;
static unsigned long cycletime = 0;

static G602 g602(
    G602_SPEED_BASELINE_LOW,
    G602_SPEED_BASELINE_HIGH,
    P_event_strober,
    P_event_lift_up,
    P_event_lift_down,
    P_motor_update
);

struct pulse
{
    unsigned long change_time; /**< Time of change */
    rotate_pulse_counter_t pulses; /**< Amount of clean pulses */
    rotate_pulse_counter_t bounces; /**< Amount of bounces (dirty pulses) */
};

static volatile struct pulse P_motor_rotate;
static volatile struct pulse P_table_rotate;

#define G602_MOTOR_BOUNCE_TIME 2U
#define G602_TABLE_BOUNCE_TIME 10U

void P_pulses_init(volatile struct pulse * pulse)
{
    pulse->change_time = time;
    pulse->pulses = 0;
    pulse->bounces = 0;
}

static void P_pulse_update(volatile struct pulse * pulse, unsigned long bounce_time)
{
    if(time - pulse->change_time >= bounce_time)
    {
        ++(pulse->pulses);
    }
    else
    {
        ++(pulse->bounces);
    }
    pulse->change_time = time;
}

static void P_pulse_get(
        volatile struct pulse * pulse_in,
        bool reset,
        struct pulse * pulse_out
)
{
    pulse_out->change_time = pulse_in->change_time;
    pulse_out->pulses = pulse_in->pulses;
    pulse_out->bounces = pulse_in->bounces;
    if(reset)
    {
        pulse_in->pulses = 0;
        pulse_in->bounces = 0;
    }
}


void drive_rotate_pulse_update(void)
{
    P_pulse_update(&P_motor_rotate, G602_MOTOR_BOUNCE_TIME);
}

void table_rotate_pulse_update(void)
{
    P_pulse_update(&P_table_rotate, G602_TABLE_BOUNCE_TIME);
}

void P_pulses_all_get(
        struct pulse * pulses_motor,
        struct pulse * pulses_table,
        bool reset
)
{
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        P_pulse_get(&P_motor_rotate, reset, pulses_motor);
        P_pulse_get(&P_table_rotate, reset, pulses_table);
    }
}

static AverageTinyMemory potentiometer_avg(FACTOR);

static const unsigned rotate_measurer_sheduler_id[G602_ROTATE_MEASURES__NUM] =
{
        2,
        3,
        4,
        5,
        6,
};

static const unsigned long rotate_measurer_handler_times[G602_ROTATE_MEASURES__NUM] =
{
        3000,
        7000,
        15000,
        30000,
        60000,
};

static int P_rotator_id_get(size_t sheduler_id)
{
    unsigned i;
    for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
    {
        if(sheduler_id == rotate_measurer_sheduler_id[i]) return i;
    }
    return -1;
}

typedef struct
{
    unsigned long m_pulses;
    unsigned long t_pulses;
    unsigned long rpm;
} tmp_measure_t;

static tmp_measure_t measures[G602_ROTATE_MEASURES__NUM] = {};

void G602::P_task_rotator_handler(
        size_t id,
        GTime_t time,
        UNUSED GTime_t now,
        G602Scheduler & sched,
        void * args
)
{
    G602_DEFINE_SELF();

    int rid = P_rotator_id_get(id);
    if(rid < 0) return;

    bool last = false;
    if(rid == G602_ROTATE_MEASURES__NUM - 1)
    {
        last = true;
    }

    struct pulse motor_pulses;
    struct pulse table_pulses;

    rotate_pulse_counter_t motor_pulses_diff;
    rotate_pulse_counter_t table_pulses_diff;

    unsigned long time_delta = rotate_measurer_handler_times[rid];

    bool speed_valid;

    P_pulses_all_get(&motor_pulses, &table_pulses, last);
    if(last)
    {
        unsigned i;
        for(i = 0; i < G602_ROTATE_MEASURES__NUM - 1; ++i)
        {
            global.rotate_measures[i].resetted = true;
        }
        motor_pulses_diff = motor_pulses.pulses;
        table_pulses_diff = table_pulses.pulses;
        speed_valid = true;
    }
    else
    {
        rotate_measures_t * rotate_measure = &global.rotate_measures[rid];
        if(rotate_measure->resetted)
        {
            rotate_measure->resetted = false;
            rotate_measure->motor_prev = 0;
            rotate_measure->table_prev = 0;
            motor_pulses_diff = 0;
            table_pulses_diff = 0;
            speed_valid = false;
        }
        else
        {
            motor_pulses_diff = motor_pulses.pulses - rotate_measure->motor_prev;
            table_pulses_diff = table_pulses.pulses - rotate_measure->table_prev;
            rotate_measure->motor_prev = motor_pulses.pulses;
            rotate_measure->table_prev = table_pulses.pulses;
            speed_valid = true;
        }
    }

    tmp_measure_t *meas = &measures[rid];

    if(speed_valid)
    {
        /* speed, rev/m */
        int speed = (int)(
                ((unsigned long)table_pulses_diff * 1000 * 60) /
                ((unsigned long)time_delta * G602_TABLE_PULSES_PER_REV));

        //DEBUG_PRINT("pv(rpm) = ");
        //DEBUG_PRINTLN(speed);

        int speed_ppm = (int)(
                ((unsigned long)table_pulses_diff * 1000 * 60) /
                ((unsigned long)time_delta));

        g602.actualSpeedUpdate(speed_ppm);
        if(self->m_permanent_process_send)
        {
            self->eventSPPV(time, (uint16_t)speed_ppm);
        }

        /*
            Fixed ctrl;
            Fixed sp;
            Fixed pv;
            pv.set(speed_actual);

            ctrl = pid.calculate(sp, pv);
        */


        meas->m_pulses = motor_pulses_diff;
        meas->t_pulses = table_pulses_diff;
        meas->rpm = speed;

//        DEBUG_PRINT("; m_pulses_d = " ); DEBUG_PRINT((int)motor_pulses_diff);
//        DEBUG_PRINT("; t_pulses_d = " ); DEBUG_PRINT((int)table_pulses_diff);
//        DEBUG_PRINT("; t_speed(rpm) = "); DEBUG_PRINT(speed);
    }
#if 0
    DEBUG_PRINT((unsigned long)now);

    unsigned i;
    for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
    {
        meas = &measures[i];
        DEBUG_PRINT("\t");

        unsigned long time_delta = rotate_measurer_handler_times[i];

        if(i == (unsigned)rid)
        {
            DEBUG_PRINT("[");
            DEBUG_PRINT(time_delta);
            DEBUG_PRINT("]");
        }
        else
        {
            DEBUG_PRINT(time_delta);
        }
        DEBUG_PRINT("\t");

        DEBUG_PRINT(meas->m_pulses);
        DEBUG_PRINT("\t");
        DEBUG_PRINT(meas->t_pulses);
        DEBUG_PRINT("\t");
        DEBUG_PRINT(meas->rpm);
    }

    DEBUG_PRINT("\tBC =\t");
    DEBUG_PRINT(motor_pulses.bounces);
    DEBUG_PRINT("\t");
    DEBUG_PRINT(table_pulses.bounces);

    DEBUG_PRINTLN();
#endif
    sched.shedule(
            id,
            time + time_delta,
            P_task_rotator_handler,
            &g602
    );
}

void G602::P_measures_start()
{
    struct pulse m_pulse;
    struct pulse t_pulse;
    P_pulses_all_get(&m_pulse, &t_pulse, true); /* reset */

    unsigned i;
    for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
    {
        sched.shedule(
                rotate_measurer_sheduler_id[i],
                m_time_now + rotate_measurer_handler_times[i],
                P_task_rotator_handler,
                this
        );
    }
    m_time_next = P_rtcNextTimeGet();
}

void setup()
{
    DEBUG_PRINT_INIT();

    time = millis();

    nostd::set_fatal(terminate_handler);
    /* INFO: разомкнуто = HIGH */

    /* gauges */
    pinMode(PIN_DI_GAUGE_DRIVE_ROTATEPULSE, INPUT_PULLUP);
    attachInterrupt(PIN_DI_2_INTERRUPT, drive_rotate_pulse_update, RISING);
    pinMode(PIN_DI_GAUGE_TABLE_ROTATEPULSE, INPUT_PULLUP);
    attachInterrupt(PIN_DI_3_INTERRUPT, table_rotate_pulse_update, RISING);

    pinMode(PIN_DI_GAUGE_STOP         , INPUT_PULLUP);
    /* buttons */
    pinMode(PIN_DI_BTN_45_33   , INPUT_PULLUP);
    pinMode(PIN_DI_BTN_AUTOSTOP, INPUT_PULLUP);
    pinMode(PIN_DI_BTN_START   , INPUT_PULLUP);
    pinMode(PIN_DI_BTN_STOP    , INPUT_PULLUP);

    pinMode(PIN_DO_ENGINE      , OUTPUT);
    pinMode(PIN_DO_LIFT        , OUTPUT);
    pinMode(PIN_DO_STROBE_ERROR, OUTPUT);
    pinMode(PIN_DO_AUDIO_DENY  , OUTPUT);

    analogWrite(PIN_DO_ENGINE, 0);
    digitalWrite(PIN_DO_STROBE_ERROR, HIGH);
    digitalWrite(PIN_DO_AUDIO_DENY  , LOW);

    analogReference(DEFAULT);

    P_pulses_init(&P_motor_rotate);
    P_pulses_init(&P_table_rotate);

#ifdef DEBUG
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
#endif

}

void loop()
{
    time_prev = time;
    time = millis();
    cycletime = time - time_prev;

    g602.timeSet(time);

    /* sensors */
    g602.notifyGaugeStopSet(digitalRead(PIN_DI_GAUGE_STOP) == LOW);
    /* buttons */
    g602.notifyButtonSpeedModeSet(digitalRead(PIN_DI_BTN_45_33) == LOW);
    g602.notifyButtonAutostopSet(digitalRead(PIN_DI_BTN_AUTOSTOP) == LOW);
    g602.notifyButtonStartSet(digitalRead(PIN_DI_BTN_START) == LOW);
    g602.notifyButtonStopSet(digitalRead(PIN_DI_BTN_STOP) == LOW);

    /* potentiometer */
    int val = analogRead(PIN_AI_POTENTIOMETER); /* 10 bits */
    potentiometer_avg.appendValue(val);
    int avg = potentiometer_avg.averageGet();
#define POTENTIOMETER_TO_MANUAL_SPEED(xval) \
        (int)map( \
            xval - G602_POTENTIOMETER_HALF, \
            G602_POTENTIOMETER_MIN, G602_POTENTIOMETER_MAX, \
            G602_SPEED_RANGE_MIN, G602_SPEED_RANGE_MAX \
        )

    int speed_manual = POTENTIOMETER_TO_MANUAL_SPEED(avg);

    g602.manualSpeedSet(speed_manual);

    g602.loop();
}
