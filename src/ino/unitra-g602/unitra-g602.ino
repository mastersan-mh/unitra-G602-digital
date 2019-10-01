#include <util/atomic.h>

#include <nostd-main.h>
#include <nostd.h>

#include "config.hpp"

#include "defs.hpp"

#include "AverageTinyMemory.hpp"

#include "GDInputDebounced.hpp"
#include "GTime.hpp"
#include "Ctrl.hpp"
#include "GBlinker.hpp"


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

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define TO_BOOL(x) ((x) != 0)

typedef nostd::Fixed32 Fixed;
typedef nostd::PidRecurrent<Fixed> PID;
typedef nostd::SchedulerSoft< G602_SHEDULER_TASKS__NUM, GTime_t > G602Scheduler;

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

//#define rotate_pulse_counter_t unsigned int
typedef unsigned int rotate_pulse_counter_t;

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

static unsigned long time;
static unsigned long time_prev = 0;
static unsigned long cycletime = 0;
static unsigned long time_next;

class G602
{
#define G602_DEFINE_SELF() \
    G602 * self = static_cast<G602*>(args)

#define ARRAY_INDEX(x) static_cast<unsigned int>(x)

public:
    static const nostd::size_t shed_task_id_blinker = 0;
    G602(
        void (*event_strober)(bool on)
    );
    ~G602();
    G602(const G602 &) = delete;
    G602& operator= (const G602 &) = delete;
    /** @brief Call it on each loop */
    void loop();

    G602Scheduler sched;
private:
    GBlinker m_blinker;
public:
    app::Ctrl ctrl;
    GDInputDebounced di_gauge_stop;
    GDInputDebounced di_btn_speed_mode;
    GDInputDebounced di_btn_autostop;
    GDInputDebounced di_btn_start;
    GDInputDebounced di_btn_stop;

private:
    void (*m_event_strober)(bool on);

    void P_blinkerStart(GBlinker::BlinkType type);
    void P_blinkerStop(GBlinker::BlinkType type);
    unsigned long P_rtcNextTimeGet() const;
    void P_measures_start();
    void P_measures_stop();

    static void P_blinker_task(nostd::size_t id, unsigned long time, unsigned long now, G602Scheduler & sched, void * args);
    static void P_ctrl_event(app::Ctrl::Event event, const app::Ctrl::EventData& data, void * args);
    static void P_event_stopSet(void * args);
    static void P_event_stopUnset(void * args);
    static void P_event_speedMode45(void * args);
    static void P_event_speedMode33(void * args);
    static void P_event_autostopEnable(void * args);
    static void P_event_autostopDisable(void * args);
    static void P_event_start(void * args);
    static void P_event_stop(void * args);

};

static G602 g602(P_event_strober);

G602::G602(
    void (*event_strober)(bool on)
)
: sched()
, m_blinker()
, ctrl(G602_SPEED_BASELINE_LOW, G602_SPEED_BASELINE_HIGH, P_ctrl_event, this)
, di_gauge_stop(false, P_event_stopUnset, P_event_stopSet, this, DI_DEBOUNCE_TIME)
, di_btn_speed_mode(false, P_event_speedMode33,  P_event_speedMode45, this, DI_DEBOUNCE_TIME)
, di_btn_autostop(false, P_event_autostopEnable,  P_event_autostopDisable, this, DI_DEBOUNCE_TIME)
, di_btn_start(false, P_event_start,  nullptr, this, DI_DEBOUNCE_TIME)
, di_btn_stop(false, P_event_stop,  nullptr, this, DI_DEBOUNCE_TIME)
, m_event_strober(event_strober)
{
}

G602::~G602()
{
}

unsigned long G602::P_rtcNextTimeGet() const
{
    unsigned long time_next;
    time_next = sched.nearestTime();
    if(time_next == 0) time_next = time + 10;
    return time_next;
}

void G602::loop()
{
    if(time < time_next)
    {
        return;
    }

/*    long late = (long)time - (long)time_next; */
    sched.handle(time);
    time_next = P_rtcNextTimeGet();
}

void G602::P_blinkerStart(GBlinker::BlinkType type)
{
    unsigned actions = m_blinker.typeSet(type, true);

    if(actions & GBLINKER_ACTIONFLAG_UNSCHEDULE)
    {
        sched.unshedule(shed_task_id_blinker);
    }
    if(actions & GBLINKER_ACTIONFLAG_SCHEDULE)
    {
        sched.shedule(shed_task_id_blinker, time, P_blinker_task, this);
    }
}

void G602::P_blinkerStop(GBlinker::BlinkType type)
{
    unsigned actions = m_blinker.typeSet(type, false);

    if(actions & GBLINKER_ACTIONFLAG_UNSCHEDULE)
    {
        sched.unshedule(shed_task_id_blinker);
    }
    if(actions & GBLINKER_ACTIONFLAG_SCHEDULE)
    {
        sched.shedule(shed_task_id_blinker, time, P_blinker_task, this);
    }
}

void G602::P_blinker_task(nostd::size_t id, unsigned long time, unsigned long now, G602Scheduler & sched, void * args)
{
    G602_DEFINE_SELF();
    bool end;
    bool light;
    unsigned long wait_time;
    self->m_blinker.scheduledPartGet(&end, &light, &wait_time);
    if(end)
    {
        self->m_event_strober(true);
    }
    else
    {
        self->m_event_strober(!light);
        sched.shedule(id, time + wait_time, G602::P_blinker_task, args);
    }
}

void G602::P_event_stopSet(void * args)
{
    G602_DEFINE_SELF();
    self->ctrl.stopTriggeredSet(true, self);
}

void G602::P_event_stopUnset(void * args)
{
    G602_DEFINE_SELF();
    self->ctrl.stopTriggeredSet(false, self);
}

void G602::P_event_speedMode45(void * args)
{
    G602_DEFINE_SELF();
    self->ctrl.baselineSpeedModeSet(app::Ctrl::BaselineSpeedMode::MODE_HIGH, self);
}

void G602::P_event_speedMode33(void * args)
{
    G602_DEFINE_SELF();
    self->ctrl.baselineSpeedModeSet(app::Ctrl::BaselineSpeedMode::MODE_LOW, self);
}

void G602::P_event_autostopEnable(void * args)
{
    G602_DEFINE_SELF();
    self->ctrl.autostopAllowSet(true, self);
}

void G602::P_event_autostopDisable(void * args)
{
    G602_DEFINE_SELF();
    self->ctrl.autostopAllowSet(false, self);
}

void G602::P_event_start(void * args)
{
    G602_DEFINE_SELF();
    app::Ctrl::RunMode mode_prev = self->ctrl.runModeGet();
    self->ctrl.start(self);
    app::Ctrl::RunMode mode = self->ctrl.runModeGet();
    if(mode != mode_prev)
    {
        if(
                mode_prev == app::Ctrl::RunMode::STARTED_AUTO ||
                mode_prev == app::Ctrl::RunMode::STARTED_MANUAL
        )
        {
            switch(mode)
            {
                case app::Ctrl::RunMode::STARTED_AUTO  : self->P_blinkerStart(GBlinker::BlinkType::ON_AUTO); break;
                case app::Ctrl::RunMode::STARTED_MANUAL: self->P_blinkerStart(GBlinker::BlinkType::ON_MANUAL); break;
                default: break;
            }
        }
        else
        {
            self->P_blinkerStart(GBlinker::BlinkType::ON_START);
        }
    }
}

void G602::P_event_stop(void * args)
{
    G602_DEFINE_SELF();
    app::Ctrl::RunMode mode_prev = self->ctrl.runModeGet();
    self->ctrl.stop(self);
    app::Ctrl::RunMode mode = self->ctrl.runModeGet();
    if(mode != mode_prev)
    {
        self->P_blinkerStart(GBlinker::BlinkType::ON_STOP);
    }
}

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

void G602::P_ctrl_event(app::Ctrl::Event event, const app::Ctrl::EventData& data, void * args)
{
    G602_DEFINE_SELF();
    switch(event)
    {
        case app::Ctrl::Event::ERRORS_UPDATE:
        {
            break;
        }

        case app::Ctrl::Event::WARNINGS_UPDATE:
        {
            bool blink_speed_to_low  = TO_BOOL(data.WARNINGS_UPDATE.warnings | CTRL_WARNING_SPEED_TOO_LOW);
            bool blink_speed_to_high = TO_BOOL(data.WARNINGS_UPDATE.warnings | CTRL_WARNING_SPEED_TOO_HIGH);

            if(blink_speed_to_low)
            {
                self->P_blinkerStart(GBlinker::BlinkType::ON_TOO_LOW_SPEED);
            }
            else
            {
                self->P_blinkerStop(GBlinker::BlinkType::ON_TOO_LOW_SPEED);
            }

            if(blink_speed_to_high)
            {
                self->P_blinkerStart(GBlinker::BlinkType::ON_TOO_HIGH_SPEED);
            }
            else
            {
                self->P_blinkerStop(GBlinker::BlinkType::ON_TOO_HIGH_SPEED);
            }

            break;
        }

        case app::Ctrl::Event::MOTOR_ON:
        {
            global.motor_on = true;
            P_motor_update();
            self->P_measures_start();
            break;
        }

        case app::Ctrl::Event::MOTOR_OFF:
        {
            global.motor_on = false;
            P_motor_update();
            self->P_measures_stop();
            break;
        }

        case app::Ctrl::Event::MOTOR_SETPOINT_UPDATE:
        {
            global.motor_setpoint = data.DRIVE_SETPOINT_UPDATE.setpoint;
            P_motor_update();
            break;
        }

        case app::Ctrl::Event::LIFT_UP:
        {
            digitalWrite(PIN_DO_LIFT, LOW);
            digitalWrite(PIN_DO_AUDIO_DENY, LOW);
            break;
        }

        case app::Ctrl::Event::LIFT_DOWN:
        {
            digitalWrite(PIN_DO_LIFT, HIGH);
            digitalWrite(PIN_DO_AUDIO_DENY, HIGH);
            break;
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

AverageTinyMemory potentiometer_avg(FACTOR);

static void read_inputs()
{
    /* sensors */
    g602.di_gauge_stop.stateSet(digitalRead(PIN_DI_GAUGE_STOP) == LOW, &g602, time);

    /* buttons */
    g602.di_btn_speed_mode.stateSet(digitalRead(PIN_DI_BTN_45_33) == LOW, &g602, time);
    g602.di_btn_autostop.stateSet(digitalRead(PIN_DI_BTN_AUTOSTOP) == LOW, &g602, time);
    g602.di_btn_start.stateSet(digitalRead(PIN_DI_BTN_START) == LOW, &g602, time);
    g602.di_btn_stop.stateSet(digitalRead(PIN_DI_BTN_STOP) == LOW, &g602, time);

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
    g602.ctrl.manualSpeedDeltaSet(speed_manual, &g602);

    //DEBUG_PRINT("speed_manual = "); DEBUG_PRINT(speed_manual);
    //DEBUG_PRINTLN();

#ifdef CTRL_DEBUG
    app::Ctrl::internal_state_t state;
    g602.ctrl.debug_get(&state);
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

typedef struct
{
    unsigned long m_pulses;
    unsigned long t_pulses;
    unsigned long rpm;
} tmp_measure_t;

static tmp_measure_t measures[G602_ROTATE_MEASURES__NUM] = {};

static void P_rotator_handler(size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args)
{

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

//    DEBUG_PRINT("SPEED MEASURE: ");
//    DEBUG_PRINT("T = ");
//    DEBUG_PRINT((unsigned long)time_delta);

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
        /* speed, rpm */
        unsigned long speed =
                ((unsigned long)table_pulses_diff * 1000 * 60) /
                ((unsigned long)time_delta * G602_TABLE_PULSES_PER_ROTATE);

        meas->m_pulses = motor_pulses_diff;
        meas->t_pulses = table_pulses_diff;
        meas->rpm = speed;

//        DEBUG_PRINT("; m_pulses_d = " ); DEBUG_PRINT((int)motor_pulses_diff);
//        DEBUG_PRINT("; t_pulses_d = " ); DEBUG_PRINT((int)table_pulses_diff);
//        DEBUG_PRINT("; t_speed(rpm) = "); DEBUG_PRINT(speed);
    }

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

    /* TODO: */
#define PULSES_TO_SPEED(xpulses) (xpulses)
    int speed_actual = PULSES_TO_SPEED(motor_pulses_diff);

    g602.ctrl.actualSpeedUpdate(speed_actual, &g602);


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
            P_rotator_handler,
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
                time + rotate_measurer_handler_times[i],
                P_rotator_handler,
                this
        );
    }
    time_next = P_rtcNextTimeGet();
}

void G602::P_measures_stop()
{
    unsigned i;
    for(i = 0; i < G602_ROTATE_MEASURES__NUM; ++i)
    {
        sched.unshedule(
                rotate_measurer_sheduler_id[i]
        );
    }
    time_next = P_rtcNextTimeGet();
}

static void P_real_time_calls_init()
{
    time_next = 0;
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

    P_real_time_calls_init();

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

    read_inputs();

    g602.loop();
}
