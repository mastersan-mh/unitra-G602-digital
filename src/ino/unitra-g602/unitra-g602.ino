#include <nostd-main.h>
#include <nostd.h>

#include "config.hpp"

#include "defs.hpp"

#include "AverageTinyMemory.hpp"

#include "GDInputDebounced.hpp"
#include "GTime.hpp"
#include "G602.hpp"

#include <util/atomic.h>

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
typedef nostd::PidRecurrent<Fixed> PID;
typedef nostd::SchedulerSoft< 5, GTime_t > Scheduler;

Scheduler sched;
PID pid;

typedef struct
{
    bool resetted;
    unsigned motor_prev;
    unsigned table_prev;
} rotate_measures_t;

typedef struct
{
    bool motor_on;
    int motor_setpoint;
    rotate_measures_t rotate_measures[G602_ROTATE_MEASURES__NUM - 1];
} global_t;

static global_t global = {};

static void terminate_handler(const char * file, unsigned int line, int error)
{
#define BLINK_PERIOD_MS 250
    static long time_prev = 0;
    static long time;
    static bool on = true;
    while(1)
    {
        time = millis();
        if(time - time_prev >= BLINK_PERIOD_MS)
        {
            time_prev = time;
            if(file == NULL)
            {
                digitalWrite(PIN_DO_STROBE_ERROR, on ? HIGH : LOW);
            }
            digitalWrite(PIN_DO_STROBE_ERROR, on ? HIGH : LOW);
            on = !on;
        }
    }
}

enum class StrobeMode
{
    STROBEMODE_OFF,
    STROBEMODE_NORMAL,
    STROBEMODE_SPEEDMODE_SWITCHED_TO_FREEMODE,
    STROBEMODE_SPEEDMODE_SWITCHED_TO_AUTOMODE,
    STROBEMODE_ERROR_SPEED_TOO_LOW,
    STROBEMODE_ERROR_SPEED_TOO_HIGH,
};

static unsigned long time;
static unsigned long time_prev = 0;
static unsigned long cycletime = 0;
static unsigned long time_next;

static bool blink_speed_to_low = false;
static bool blink_speed_to_high = false;

static void P_event_stopSet();
static void P_event_stopUnset();

static void P_event_speedMode45();
static void P_event_speedMode33();
static void P_event_autostopEnable();
static void P_event_autostopDisable();
static void P_event_start();
static void P_event_stop();

static GDInputDebounced di_gauge_stop(false, P_event_stopUnset,  P_event_stopSet, DI_DEBOUNCE_TIME);
static GDInputDebounced di_btn_speed_mode(false, P_event_speedMode33,  P_event_speedMode45, DI_DEBOUNCE_TIME);
static GDInputDebounced di_btn_autostop(false, P_event_autostopEnable,  P_event_autostopDisable, DI_DEBOUNCE_TIME);
static GDInputDebounced di_btn_start(false, P_event_start,  nullptr, DI_DEBOUNCE_TIME);
static GDInputDebounced di_btn_stop(false, P_event_stop,  nullptr, DI_DEBOUNCE_TIME);

static void g602_event(app::G602::Event event, const app::G602::EventData& data);

app::G602 g602(
        G602_SPEED_BASELINE_LOW,
        G602_SPEED_BASELINE_HIGH,
        g602_event
);

static void P_motor_update()
{
//    DEBUG_PRINT("m = "); DEBUG_PRINTLN(global.motor_on ? "on" : "off");
//    DEBUG_PRINT("SP = "); DEBUG_PRINT(global.motor_setpoint);
    if(global.motor_on)
    {
        /* [0; 255] */
        int value = map(global.motor_setpoint, G602_SPEED_MIN, G602_SPEED_MAX, 0, 255);
        analogWrite(PIN_DO_ENGINE, value);
//        DEBUG_PRINT("; v = "); DEBUG_PRINTLN(value);
    }
    else
    {
        analogWrite(PIN_DO_ENGINE, 0);
//        DEBUG_PRINTLN("; v = [0]");
    }
}

static void g602_event(app::G602::Event event, const app::G602::EventData& data)
{
    switch(event)
    {
        case app::G602::Event::ERRORS_UPDATE:
        {
            break;
        }

        case app::G602::Event::WARNINGS_UPDATE:
        {
#define TO_BOOL(x) ((x) != 0)
            blink_speed_to_low = TO_BOOL(data.WARNINGS_UPDATE.warnings | WARNING_SPEED_TOO_LOW);
            blink_speed_to_high = TO_BOOL(data.WARNINGS_UPDATE.warnings | WARNING_SPEED_TOO_HIGH);
            break;
        }

        case app::G602::Event::MOTOR_ON:
        {
            global.motor_on = true;
            P_motor_update();
            break;
        }

        case app::G602::Event::MOTOR_OFF:
        {
            global.motor_on = false;
            P_motor_update();
            break;
        }

        case app::G602::Event::MOTOR_SETPOINT_UPDATE:
        {
            global.motor_setpoint = data.DRIVE_SETPOINT_UPDATE.setpoint;
            P_motor_update();
            break;
        }

        case app::G602::Event::LIFT_UP:
        {
            digitalWrite(PIN_DO_LIFT, LOW);
            break;
        }

        case app::G602::Event::LIFT_DOWN:
        {
            digitalWrite(PIN_DO_LIFT, HIGH);
            break;
        }

    }

}

#define rotate_pulse_counter_t unsigned int
/* typedef unsigned int rotate_pulse_counter_t; */

static volatile rotate_pulse_counter_t motor_rotate_pulse_counter = 0;
static volatile rotate_pulse_counter_t table_rotate_pulse_counter = 0;

void drive_rotate_pulse_update(void)
{
    ++motor_rotate_pulse_counter;
}

void table_rotate_pulse_update(void)
{
    ++table_rotate_pulse_counter;
}

void P_pulses_get(
        rotate_pulse_counter_t * pulses_motor,
        rotate_pulse_counter_t * pulses_table,
        bool reset
)
{
    if(reset)
    {
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            (*pulses_motor) = motor_rotate_pulse_counter;
            (*pulses_table) = table_rotate_pulse_counter;
            motor_rotate_pulse_counter = 0;
            table_rotate_pulse_counter = 0;
        }
    }
    else
    {
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            (*pulses_motor) = motor_rotate_pulse_counter;
            (*pulses_table) = table_rotate_pulse_counter;
        }
    }
}

static void P_event_stopSet()
{
    DEBUG_PRINTLN("event_stopSet()");
    g602.stopTriggeredSet(true);
}

static void P_event_stopUnset()
{
    DEBUG_PRINTLN("event_stopUnset()");
    g602.stopTriggeredSet(false);
}

static void P_event_speedMode45()
{
    DEBUG_PRINTLN("event_speedMode45()");
    g602.baselineSpeedModeSet(app::G602::BaselineSpeedMode::MODE_HIGH);
}

static void P_event_speedMode33()
{
    DEBUG_PRINTLN("event_speedMode33()");
    g602.baselineSpeedModeSet(app::G602::BaselineSpeedMode::MODE_LOW);
}

static void P_event_autostopEnable()
{
    DEBUG_PRINTLN("event_autostopEnable()");
    g602.autostopAllowSet(true);
}

static void P_event_autostopDisable()
{
    DEBUG_PRINTLN("event_autostopDisable()");
    g602.autostopAllowSet(false);
}

void P_event_start()
{
    g602.start();
    digitalWrite(PIN_DO_AUDIO_DENY, HIGH);
    DEBUG_PRINTLN("event_start()");
}

void P_event_stop()
{
    DEBUG_PRINTLN("event_stop()");
    g602.stop();
}

/*
void event_tmp_pd()
{
    DEBUG_PRINTLN("event_tmp_pd()");
}

void event_tmp_pt()
{
    DEBUG_PRINTLN("event_tmp_pt()");
}
*/

AverageTinyMemory potentiometer_avg(FACTOR);

static void read_inputs()
{
    /* sensors */
    di_gauge_stop.stateSet(digitalRead(PIN_DI_GAUGE_STOP) == LOW, time);

    /* buttons */
    di_btn_speed_mode.stateSet(digitalRead(PIN_DI_BTN_45_33) == LOW, time);
    di_btn_autostop.stateSet(digitalRead(PIN_DI_BTN_AUTOSTOP) == LOW, time);
    di_btn_start.stateSet(digitalRead(PIN_DI_BTN_START) == LOW, time);
    di_btn_stop.stateSet(digitalRead(PIN_DI_BTN_STOP) == LOW, time);

    /* 10 bits */
    /* 0000.0011.1111.1111 */
    unsigned val = analogRead(PIN_AI_POTENTIOMETER);

#ifdef DEBUG
    static long values = 0;
    ++values;
#endif

    potentiometer_avg.appendValue(val);

    int avg = potentiometer_avg.averageGet();

#define POTENTIOMETER_TO_MANUAL_SPEED(xval) (xval)
    int speed_manual = POTENTIOMETER_TO_MANUAL_SPEED(avg - G602_SPEED_HALF);
    g602.manualSpeedDeltaSet(speed_manual);

/*
    DEBUG_PRINT("speed_manual = "); DEBUG_PRINT(speed_manual);
    DEBUG_PRINTLN();
*/

#ifdef G602_DEBUG
    app::G602::internal_state_t state;
    g602.debug_get(&state);
    DEBUG_PRINT("m_state = "); DEBUG_PRINT((int)state.m_state);
    DEBUG_PRINT("; m_speed_manual_delta = "); DEBUG_PRINT((int)state.m_speed_manual_delta);
    DEBUG_PRINTLN();
#endif
}

static const unsigned rotate_measurer_sheduler_id[G602_ROTATE_MEASURES__NUM] =
{
        1,
        2,
        3,
        4,
        5,
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

static void P_rotator_handler(size_t id, GTime_t time, GTime_t now, Scheduler & rtc)
{

    int rid = P_rotator_id_get(id);
    if(rid < 0) return;

    bool last = false;
    if(rid == G602_ROTATE_MEASURES__NUM - 1)
    {
        last = true;
    }


    rotate_pulse_counter_t motor_pulses;
    rotate_pulse_counter_t table_pulses;

    rotate_pulse_counter_t motor_pulses_diff;
    rotate_pulse_counter_t table_pulses_diff;

    unsigned long time_delta = rotate_measurer_handler_times[rid];

    DEBUG_PRINT("SPEED MEASURE: ");
    DEBUG_PRINT("T = ");
    DEBUG_PRINT((unsigned long)time_delta);

    bool speed_valid;

    P_pulses_get(&motor_pulses, &table_pulses, last);
    if(last)
    {
        unsigned i;
        for(i = 0; i < G602_ROTATE_MEASURES__NUM - 1; ++i)
        {
            global.rotate_measures[i].resetted = true;
        }
        motor_pulses_diff = motor_pulses;
        table_pulses_diff = table_pulses;
        speed_valid = true;
    }
    else
    {
        rotate_measures_t * rotate_measure = &global.rotate_measures[rid];
        if(rotate_measure->resetted)
        {
            DEBUG_PRINT("[RESET]");
            rotate_measure->resetted = false;
            rotate_measure->motor_prev = 0;
            rotate_measure->table_prev = 0;
            motor_pulses_diff = 0;
            table_pulses_diff = 0;
            speed_valid = false;
        }
        else
        {
            motor_pulses_diff = motor_pulses - rotate_measure->motor_prev;
            table_pulses_diff = table_pulses - rotate_measure->table_prev;
            rotate_measure->motor_prev = motor_pulses;
            rotate_measure->table_prev = table_pulses;
            speed_valid = true;
        }
    }

    if(speed_valid)
    {
        unsigned long speed =
                ((unsigned long)table_pulses_diff * 1000 * 60) /
                ((unsigned long)time_delta * G602_TABLE_PULSES_PER_ROTATE);

        DEBUG_PRINT("; m_pulses_d = " ); DEBUG_PRINT((int)motor_pulses_diff);
        DEBUG_PRINT("; t_pulses_d = " ); DEBUG_PRINT((int)table_pulses_diff);
        DEBUG_PRINT("; t_speed(rpm) = "); DEBUG_PRINT(speed);
    }

    DEBUG_PRINTLN();

    /* TODO: */
#define PULSES_TO_SPEED(xpulses) (xpulses)
    int speed_actual = PULSES_TO_SPEED(motor_pulses_diff);

    g602.actualSpeedUpdate(speed_actual);


/*
    Fixed ctrl;
    Fixed sp;
    Fixed pv;
    pv.set(speed_actual);

    ctrl = pid.calculate(sp, pv);
*/

    sched.shedule(
            id,
            time + time_delta,
            P_rotator_handler
    );
}

void real_time_init()
{
    unsigned i;
    for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
    {
        sched.shedule(
                rotate_measurer_sheduler_id[i],
                rotate_measurer_handler_times[i],
                P_rotator_handler
        );
    }
    time_next = sched.nearestTime();
}

void real_time_calls()
{
    if(time < time_next)
    {
        return;
    }

    long late = (long)time - (long)time_next;
    sched.handle(time);
    time_next = sched.nearestTime();
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

    real_time_init();

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

    read_inputs();

    real_time_calls();
}
