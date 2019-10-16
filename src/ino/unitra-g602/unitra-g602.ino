#include <util/atomic.h>
#include <EEPROM.h>

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

void P_config_store(const uint8_t * conf, size_t size)
{
    unsigned i;
    for(i = 0; i < size; ++i)
    {
        EEPROM.write(i, conf[i]);
    }
}

void P_config_load(uint8_t * conf, size_t size, bool * empty)
{
    bool tempty = true;
    unsigned i;
    for(i = 0; i < size; ++i)
    {
        for(i = 0; i < size; ++i)
        {
            conf[i] = EEPROM.read(i);
            if(conf[i] != 255)
            {
                tempty = false;
            }
        }
    }
    (*empty) = tempty;
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

void P_motor_update(bool state, int output)
{
    if(state)
    {
        analogWrite(PIN_DO_ENGINE, output);
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
    P_config_store,
    P_config_load,
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
    P_pulse_update(&P_motor_rotate, DI_MOTOR_DEBOUNCE_TIME);
}

void table_rotate_pulse_update(void)
{
    P_pulse_update(&P_table_rotate, DI_TABLE_DEBOUNCE_TIME);
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

typedef struct
{
    unsigned long long pulses_sum;
    unsigned amount;
    unsigned long ppm;
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

    struct pulse motor_pulses;
    struct pulse table_pulses;


    unsigned long time_delta = ctrl_handler_period;

    P_pulses_all_get(&motor_pulses, &table_pulses, true);

    // rotate_pulse_counter_t motor_pulses_diff = motor_pulses.pulses;
    rotate_pulse_counter_t table_pulses_diff = table_pulses.pulses;

    self->m_pulses.append(table_pulses_diff);

    static unsigned periods_amount[G602_ROTATE_MEASURES__NUM] =
    {
            1,
            5,
            15,
            30,
            60,
    };

    tmp_measure_t * meas;

    unsigned i;
    for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
    {
        meas = &measures[i];
        meas->pulses_sum = 0;
        meas->amount = 0;
    };


    unsigned vindex = 0;
    SWindow::reverse_const_iterator it;
    for(
            it = self->m_pulses.rcbegin(), vindex = 0;
            it != self->m_pulses.rcend();
            --it, ++vindex
    )
    {
        for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
        {
            if(vindex < periods_amount[i])
            {
                meas = &measures[i];
                meas->pulses_sum += (*it);
                ++(meas->amount);
            }
        };
    }

#define SPEED_PPM(dpulses, dtime) \
        ( \
                ((unsigned long)dpulses * 1000 * 60) / \
                ((unsigned long)dtime) \
        )

#define SPEED_RPM(dpulses, dtime) \
        ( \
                ((unsigned long)(dpulses) * 1000 * 60) / \
                ((unsigned long)(dtime) * G602_TABLE_PULSES_PER_REV) \
        )

    for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
    {
        meas = &measures[i];
        meas->ppm = SPEED_PPM(meas->pulses_sum, time_delta * meas->amount);
        meas->rpm = SPEED_RPM(meas->pulses_sum, time_delta * meas->amount);
        meas->amount = 0;

    };

    i = 2; /* 15 sec */
    meas = &measures[i];

    int speed_pv_ppm = (int)meas->ppm;

    self->m_ctrl.actualSpeedUpdate(speed_pv_ppm, self);

    bool motor_state;
    app::Ctrl::speed_t table_setpoint;
    self->m_ctrl.motorGet(&motor_state, &table_setpoint);
    app::Ctrl::speed_t speed_sp = (motor_state ? table_setpoint : 0);

    Fixed sp;
    Fixed pv;
    Fixed ctrl;
    sp.set(speed_sp);
    pv.set(speed_pv_ppm);

    ctrl = self->m_pid.calculate(sp, pv);

#if 1
    fixed32_t ctrl_raw = ctrl.toRawFixed();
    if(ctrl_raw < 0) ctrl_raw = 0;
    //DEBUG_PRINT("ctrl_raw = "); DEBUG_PRINTLN(ctrl_raw);

    int ctrl_int = (int)(ctrl_raw >> 16);
    //DEBUG_PRINT("ctrl_int = "); DEBUG_PRINTLN(ctrl_int);

    int motor_output = constrain(ctrl_int, 0, 255);
#else
    int motor_output = (int)map(
            speed_sp,
            G602_SPEED_MIN,
            G602_SPEED_MAX,
            0,
            255
    );

    if(!motor_state) motor_output = 0;
#endif

    self->m_event_motor_update(motor_state, motor_output);

    if(self->m_permanent_process_send)
    {
        self->P_rpc_eventSPPV(time, (uint16_t)speed_sp, (uint16_t)speed_pv_ppm, (fixed32_t)ctrl_raw);
    }

#if 0
    DEBUG_PRINT(now);

    for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
    {
        meas = &measures[i];
        DEBUG_PRINT("\t");

        unsigned long time_delta = ctrl_handler_period;

        DEBUG_PRINT("[ ");
        DEBUG_PRINT(time_delta * periods_amount[i]);
        DEBUG_PRINT(" ]");

        DEBUG_PRINT("\t");
        DEBUG_PRINT(meas->ppm);
        DEBUG_PRINT(" ");
        DEBUG_PRINT(meas->rpm);
    }

    DEBUG_PRINT("\tp ");
    DEBUG_PRINT(table_pulses.pulses);
    DEBUG_PRINT("\tboun ");
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

    m_sched.shedule(
            shed_task_id_ctrl,
            m_time_now + ctrl_handler_period,
            P_task_rotator_handler,
            this
    );

    m_pid.reset();
    m_time_next = P_rtcNextTimeGet();
}

void G602::P_measures_stop()
{
    struct pulse m_pulse;
    struct pulse t_pulse;
    P_pulses_all_get(&m_pulse, &t_pulse, true); /* reset */

    m_sched.unshedule(shed_task_id_ctrl);
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
    attachInterrupt(PIN_DI_2_INTERRUPT, drive_rotate_pulse_update, FALLING);
    pinMode(PIN_DI_GAUGE_TABLE_ROTATEPULSE, INPUT_PULLUP);
    attachInterrupt(PIN_DI_3_INTERRUPT, table_rotate_pulse_update, FALLING);

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
            xval, \
            G602_POTENTIOMETER_MIN, G602_POTENTIOMETER_MAX, \
            G602_SPEED_RANGE_MIN, G602_SPEED_RANGE_MAX \
        )

    int speed_manual = POTENTIOMETER_TO_MANUAL_SPEED(avg);

    g602.manualSpeedSet(speed_manual);

    g602.loop();
}
