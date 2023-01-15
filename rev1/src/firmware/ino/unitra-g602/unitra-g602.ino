#include "config.hpp"

#include "defs.hpp"

#include "AverageTinyMemory.hpp"

#include "G602.hpp"

#include "utils.hpp"

#include <nostd-main.h>
#include <nostd.h>

#include <util/atomic.h>
#include <EEPROM.h>

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
using rotate_pulse_counter_t = unsigned int;

static unsigned long g_time;
static unsigned long g_time_prev = 0;
static unsigned long c_cycletime = 0;

template<typename T>
T utils_map(T x, T in_min, T in_max, T out_min, T out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void dump_u8(
        const uint8_t * data ARG_UNUSED,
        unsigned size
)
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

void dump_u16(
        const uint16_t * data ARG_UNUSED,
        unsigned size
)
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

struct pulse
{
    unsigned long change_time; /**< Time of change */
    rotate_pulse_counter_t pulses; /**< Amount of clean pulses */
    rotate_pulse_counter_t bounces; /**< Amount of bounces (dirty pulses) */
};

static volatile struct pulse P_motor_rotate;
static volatile struct pulse P_table_rotate;

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

static void P_pulses_get(unsigned * motor_dpulses, unsigned * table_dpulses)
{
    struct pulse m_pulse;
    struct pulse t_pulse;
    P_pulses_all_get(&m_pulse, &t_pulse, true); /* reset */
    (*motor_dpulses) = m_pulse.pulses;
    (*table_dpulses) = t_pulse.pulses;

#if 0
    DEBUG_PRINT(g_time);

    DEBUG_PRINT("\tpulses ");
    DEBUG_PRINT(t_pulse.pulses);
    DEBUG_PRINT("\tbounces ");
    DEBUG_PRINT(t_pulse.bounces);

    DEBUG_PRINTLN();
#endif

}

static G602 g602(
    P_config_store,
    P_config_load,
    P_event_strober,
    P_event_lift_up,
    P_event_lift_down,
    P_motor_update,
    P_pulses_get
);

void P_pulses_init(volatile struct pulse * pulse)
{
    pulse->change_time = g_time;
    pulse->pulses = 0;
    pulse->bounces = 0;
}

static void P_pulse_update(volatile struct pulse * pulse, unsigned long bounce_time)
{
    if(g_time - pulse->change_time >= bounce_time)
    {
        ++(pulse->pulses);
    }
    else
    {
        ++(pulse->bounces);
    }
    pulse->change_time = g_time;
}

void drive_rotate_pulse_update(void)
{
    P_pulse_update(&P_motor_rotate, DI_MOTOR_DEBOUNCE_TIME);
}

void table_rotate_pulse_update(void)
{
    P_pulse_update(&P_table_rotate, DI_TABLE_DEBOUNCE_TIME);
}

static AverageTinyMemory potentiometer_avg(DI_POTENTIOMETER_FACTOR);

void setup()
{
    Serial.begin(9600);

    DEBUG_PRINT_INIT();

    g_time = millis();

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

}

void loop()
{
    g_time_prev = g_time;
    g_time = millis();
    c_cycletime = g_time - g_time_prev;

    g602.timeSet(g_time);

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
        utils_map( \
                Speed(xval, Speed::tag_int), \
                Speed(G602_POTENTIOMETER_MIN, Speed::tag_int), \
                Speed(G602_POTENTIOMETER_MAX, Speed::tag_int), \
                Speed(G602_SPEED_RANGE_MIN, Speed::tag_double), \
                Speed(G602_SPEED_RANGE_MAX, Speed::tag_double) \
        )
    const Speed speed_manual = POTENTIOMETER_TO_MANUAL_SPEED(avg);

    g602.manualSpeedSet(speed_manual);

    g602.loop();
}
