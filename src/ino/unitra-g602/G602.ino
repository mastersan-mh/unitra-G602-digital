/**
 * @file G602.cpp
 */

#include "G602.hpp"

#define TO_BOOL(x) ((x) != 0)

G602::G602(
    int baselineSpeedLow,
    int baselineSpeedHigh,
    void (*event_strober)(bool on),
    void (*event_lift_up)(),
    void (*event_lift_down)(),
    void (*event_motor_update)(bool state, int setpoint)
)
: m_event_strober(event_strober)
, m_event_lift_up(event_lift_up)
, m_event_lift_down(event_lift_down)
, m_event_motor_update(event_motor_update)
, m_time_now(0)
, m_time_next(0)
, m_motor_on(false)
, m_motor_setpoint(0)
, sched()
, m_blinker()
, m_ctrl(baselineSpeedLow, baselineSpeedHigh, P_ctrl_event, this)
, m_di_gauge_stop(false, P_event_stopUnset, P_event_stopSet, this, DI_DEBOUNCE_TIME)
, m_di_btn_speed_mode(false, P_event_speedMode33,  P_event_speedMode45, this, DI_DEBOUNCE_TIME)
, m_di_btn_autostop(false, P_event_autostopEnable,  P_event_autostopDisable, this, DI_DEBOUNCE_TIME)
, m_di_btn_start(false, P_event_start,  nullptr, this, DI_DEBOUNCE_TIME)
, m_di_btn_stop(false, P_event_stop,  P_event_stop_release, this, DI_DEBOUNCE_TIME)
, m_comm()
, m_rpc(P_rpc_send, this)
, m_permanent_process_send(false)
, m_permanent_process_send_ruid()
{
    const GRPC::procedure_t procs[] =
    {
            P_rpc_proc_00_impulses_r,
            nullptr
    };
    m_rpc.procedures_register(procs);

    P_measures_start();

}

G602::~G602()
{
}

void G602::timeSet(unsigned long time_now)
{
    m_time_now = time_now;
}

void G602::loop()
{
    P_comm_reader();

    if(m_time_now >= m_time_next)
    {
        /*    long late = (long)time - (long)time_next; */
        sched.handle(m_time_now);
        m_time_next = P_rtcNextTimeGet();

    }

#ifdef CTRL_DEBUG
    app::Ctrl::internal_state_t state;
    g602.m_ctrl.debug_get(&state);
    DEBUG_PRINT("m_state = "); DEBUG_PRINT((int)state.m_state);
    DEBUG_PRINT("; m_speed_manual_delta = "); DEBUG_PRINT((int)state.m_speed_manual_delta);
    DEBUG_PRINTLN();
#endif

}

void G602::notifyGaugeStopSet(bool state)
{
    m_di_gauge_stop.stateSet(state, this, m_time_now);
}

void G602::notifyButtonSpeedModeSet(bool state)
{
    m_di_btn_speed_mode.stateSet(state, this, m_time_now);
}

void G602::notifyButtonAutostopSet(bool state)
{
    m_di_btn_autostop.stateSet(state, this, m_time_now);
}

void G602::notifyButtonStartSet(bool state)
{
    m_di_btn_start.stateSet(state, this, m_time_now);
}

void G602::notifyButtonStopSet(bool state)
{
    m_di_btn_stop.stateSet(state, this, m_time_now);
}

void G602::manualSpeedSet(int speed)
{
    m_ctrl.speedManualSet(speed, this);
}

void G602::actualSpeedUpdate(int speed)
{
    m_ctrl.actualSpeedUpdate(speed, this);
}

unsigned long G602::P_rtcNextTimeGet() const
{
    unsigned long time_next = sched.nearestTime();
    return (time_next > 0 ? time_next : m_time_now + 10 );
}

void G602::P_blinker_start(GBlinker::BlinkType type)
{
    unsigned actions = m_blinker.typeSet(type, true);

    if(actions & GBLINKER_ACTIONFLAG_UNSCHEDULE)
    {
        sched.unshedule(shed_task_id_blinker);
    }
    if(actions & GBLINKER_ACTIONFLAG_SCHEDULE)
    {
        sched.shedule(shed_task_id_blinker, m_time_now, P_task_blinker, this);
        m_time_next = P_rtcNextTimeGet();
    }
}

void G602::P_blinker_stop(GBlinker::BlinkType type)
{
    unsigned actions = m_blinker.typeSet(type, false);

    if(actions & GBLINKER_ACTIONFLAG_UNSCHEDULE)
    {
        sched.unshedule(shed_task_id_blinker);
    }
    if(actions & GBLINKER_ACTIONFLAG_SCHEDULE)
    {
        sched.shedule(shed_task_id_blinker, m_time_now, P_task_blinker, this);
        m_time_next = P_rtcNextTimeGet();
    }
}

void G602::P_task_blinker(nostd::size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args)
{
    G602_DEFINE_SELF();
    bool end = false;
    bool light = false;
    unsigned long wait_time = 0;
    self->m_blinker.scheduledPartGet(&end, &light, &wait_time);
    if(end)
    {
        self->m_event_strober(true);
    }
    else
    {
        self->m_event_strober(!light);
        sched.shedule(id, time + wait_time, G602::P_task_blinker, args);
    }
}

void G602::P_task_awaiting_service_mode(nostd::size_t id, GTime_t time, GTime_t now, G602Scheduler & sched, void * args)
{
    G602_DEFINE_SELF();
    self->m_ctrl.mode_service_1(self);
    self->P_blinker_start(GBlinker::BlinkType::FAST6);
}

void G602::P_motor_update()
{
    m_event_motor_update(m_motor_on, m_motor_setpoint);
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
            bool blink_speed_to_low  = TO_BOOL(data.WARNINGS_UPDATE.warnings & CTRL_WARNING_SPEED_TOO_LOW);
            bool blink_speed_to_high = TO_BOOL(data.WARNINGS_UPDATE.warnings & CTRL_WARNING_SPEED_TOO_HIGH);

            if(blink_speed_to_low)
            {
                self->P_blinker_start(GBlinker::BlinkType::SLOW);
            }
            else
            {
                self->P_blinker_stop(GBlinker::BlinkType::SLOW);
            }

            if(blink_speed_to_high)
            {
                self->P_blinker_start(GBlinker::BlinkType::FAST);
            }
            else
            {
                self->P_blinker_stop(GBlinker::BlinkType::FAST);
            }

            break;
        }

        case app::Ctrl::Event::MOTOR_ON:
        {
            self->m_motor_on = true;
            self->P_motor_update();
            break;
        }

        case app::Ctrl::Event::MOTOR_OFF:
        {
            self->m_motor_on = false;
            self->P_motor_update();
            break;
        }

        case app::Ctrl::Event::MOTOR_SETPOINT_UPDATE:
        {
            self->m_motor_setpoint = data.DRIVE_SETPOINT_UPDATE.setpoint;
            self->P_motor_update();
            break;
        }

        case app::Ctrl::Event::LIFT_UP:
        {
            self->m_event_lift_up();
            break;
        }

        case app::Ctrl::Event::LIFT_DOWN:
        {
            self->m_event_lift_down();
            break;
        }

    }

}

void G602::P_event_stopSet(void * args)
{
    G602_DEFINE_SELF();
    self->m_ctrl.stopTriggeredSet(true, self);
}

void G602::P_event_stopUnset(void * args)
{
    G602_DEFINE_SELF();
    self->m_ctrl.stopTriggeredSet(false, self);
}

void G602::P_event_speedMode45(void * args)
{
    G602_DEFINE_SELF();
    self->m_ctrl.baselineSpeedModeSet(app::Ctrl::BaselineSpeedMode::MODE_HIGH, self);
}

void G602::P_event_speedMode33(void * args)
{
    G602_DEFINE_SELF();
    self->m_ctrl.baselineSpeedModeSet(app::Ctrl::BaselineSpeedMode::MODE_LOW, self);
}

void G602::P_event_autostopEnable(void * args)
{
    G602_DEFINE_SELF();
    self->m_ctrl.autostopAllowSet(true, self);
}

void G602::P_event_autostopDisable(void * args)
{
    G602_DEFINE_SELF();
    self->m_ctrl.autostopAllowSet(false, self);
}

void G602::P_event_start(void * args)
{
    G602_DEFINE_SELF();

    switch(self->m_ctrl.runModeGet())
    {
        case app::Ctrl::RunMode::NORMAL_STOPPED:
        {
            self->m_ctrl.start(self);
            self->P_blinker_start(GBlinker::BlinkType::ACCELERATING);
            break;
        }
        case app::Ctrl::RunMode::NORMAL_STARTED_AUTO:
        {
            self->m_ctrl.start(self);
            self->P_blinker_start(GBlinker::BlinkType::B3TIME);
            break;
        }
        case app::Ctrl::RunMode::NORMAL_STARTED_MANUAL:
        {
            self->m_ctrl.start(self);
            self->P_blinker_start(GBlinker::BlinkType::B1TIME);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE1_STOPPED:
        {
            self->m_ctrl.start(self);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE1_STARTED:
        {
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE2_STOPPED:
        {
            self->m_ctrl.start(self);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE2_STARTED:
        {
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE3_STOPPED:
        {
            self->m_ctrl.start(self);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE3_STARTED:
        {
            break;
        }
    }

    DEBUG_PRINT("P_event_start(): self->m_ctrl.runModeGet()) = ");
    DEBUG_PRINTLN((int)self->m_ctrl.runModeGet());

}

void G602::P_event_stop(void * args)
{
    G602_DEFINE_SELF();

    DEBUG_PRINT("P_event_stop(): self->m_ctrl.runModeGet()) = ");
    DEBUG_PRINTLN((int)self->m_ctrl.runModeGet());

    switch(self->m_ctrl.runModeGet())
    {
        case app::Ctrl::RunMode::NORMAL_STOPPED:
        {
            self->sched.shedule(
                shed_task_id_service_mode_awaiting,
                self->m_time_now + service_mode_enter_awaiting_time,
                P_task_awaiting_service_mode,
                self
            );
            break;
        }
        case app::Ctrl::RunMode::NORMAL_STARTED_AUTO:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
        case app::Ctrl::RunMode::NORMAL_STARTED_MANUAL:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE1_STOPPED:
        {
            self->m_ctrl.mode_service_2(self);
            self->P_blinker_start(GBlinker::BlinkType::B2TIME);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE1_STARTED:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE2_STOPPED:
        {
            self->m_ctrl.mode_service_3(self);
            self->P_blinker_start(GBlinker::BlinkType::B3TIME);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE2_STARTED:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE3_STOPPED:
        {
            self->m_ctrl.mode_normal(self);
            self->P_blinker_start(GBlinker::BlinkType::FAST6);
            break;
        }
        case app::Ctrl::RunMode::SERVICE_MODE3_STARTED:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
    }

    DEBUG_PRINT("P_event_stop(): self->m_ctrl.runModeGet()) = ");
    DEBUG_PRINTLN((int)self->m_ctrl.runModeGet());

}

void G602::P_event_stop_release(void * args)
{
    G602_DEFINE_SELF();
    self->sched.unshedule(shed_task_id_service_mode_awaiting);
}

void G602::P_comm_reader()
{
#define CAPACITY 32
    uint8_t data[CAPACITY];
    unsigned size;
    unsigned rest;

    size = m_comm.readFrame(data, CAPACITY, &rest);
    if(size > 0)
    {
#ifdef DEBUG
        unsigned i;
        for(i = 0; i < size; ++i)
        {
            DEBUG_PRINT("P_comm_reader(): data[");
            DEBUG_PRINT(i);
            DEBUG_PRINT("]=");
            DEBUG_PRINTLN(data[i]);
        }
#endif
        GRPC::Error err = m_rpc.handle(data, size);
        DEBUG_PRINT("P_comm_reader(): m_rpc.handle: err = ");
        DEBUG_PRINTLN((int)err);
    }
}

void G602::P_rpc_send(const uint8_t * data, unsigned data_size, void * args)
{
    G602_DEFINE_SELF();
    self->m_comm.writeFrame(data, data_size);

}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_proc_00_impulses_r(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    DEBUG_PRINTLN("P_rpc_proc_00_impulses_r():");

    if(argc != 0) return GRPC_ERR_INVALID_ARGUMENTS;

    (*resc) = 1;
    resv[0] = G602_TABLE_PULSES_PER_REV;
    return GRPC_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_proc_01_mode_current_r(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_ERR_INVALID_ARGUMENTS;

    DEBUG_PRINTLN("P_rpc_proc_01_mode_current_r():");

    app::Ctrl::RunMode runMode = self->m_ctrl.runModeGet();
    static const uint16_t modes[] =
    {
            [ARRAY_INDEX(app::Ctrl::RunMode::NORMAL_STOPPED       )] = 0x1000, /* 1.1: stopped */
            [ARRAY_INDEX(app::Ctrl::RunMode::NORMAL_STARTED_AUTO  )] = 0x1001, /* 1.2: auto */
            [ARRAY_INDEX(app::Ctrl::RunMode::NORMAL_STARTED_MANUAL)] = 0x1002, /* 1.3: manual */
            [ARRAY_INDEX(app::Ctrl::RunMode::SERVICE_MODE1_STOPPED)] = 0x2100, /* 2.1: stopped */
            [ARRAY_INDEX(app::Ctrl::RunMode::SERVICE_MODE1_STARTED)] = 0x2101, /* 2.1: started */
            [ARRAY_INDEX(app::Ctrl::RunMode::SERVICE_MODE2_STOPPED)] = 0x2200, /* 2.2: stopped */
            [ARRAY_INDEX(app::Ctrl::RunMode::SERVICE_MODE2_STARTED)] = 0x2201, /* 2.2: started */
            [ARRAY_INDEX(app::Ctrl::RunMode::SERVICE_MODE3_STOPPED)] = 0x2300, /* 2.3: stopped */
            [ARRAY_INDEX(app::Ctrl::RunMode::SERVICE_MODE3_STARTED)] = 0x2301, /* 2.3: started */
    };

    (*resc) = 1;
    resv[0] = modes[ARRAY_INDEX(runMode)];

    return GRPC_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_proc_02_koef_r(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    //G602_DEFINE_SELF();
    if(argc != 0) return GRPC_ERR_INVALID_ARGUMENTS;
    (*resc) = 6;

    nostd::Fixed32 Kp(1.1);
    nostd::Fixed32 Ki(2.2);
    nostd::Fixed32 Kd(3.3);

    uint32_t Kp_raw = Kp.toRawFixed();
    uint32_t Ki_raw = Ki.toRawFixed();
    uint32_t Kd_raw = Kd.toRawFixed();

    resv[0] = (Kp_raw >> 16);
    resv[1] = (Kp_raw & 0x0000ffff);
    resv[2] = (Ki_raw >> 16);
    resv[3] = (Ki_raw & 0x0000ffff);
    resv[4] = (Kd_raw >> 16);
    resv[5] = (Kd_raw & 0x0000ffff);
    return GRPC_ERR_OK;
}

/**
 * @param argc = 6
 * @param argv[0]   Kp hi
 * @param argv[1]   Kp lo
 * @param argv[2]   Ki hi
 * @param argv[3]   Ki lo
 * @param argv[4]   Kd hi
 * @param argv[5]   Kd lo
 */
uint8_t G602::P_rpc_proc_03_koef_w(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    //G602_DEFINE_SELF();
    if(argc != 6) return GRPC_ERR_INVALID_ARGUMENTS;

    uint32_t Kp_raw;
    uint32_t Ki_raw;
    uint32_t Kd_raw;
#define BUILD_32(hi, lo) (((uint32_t)(hi) << 16) | (uint32_t)(lo))

    Kp_raw = BUILD_32(argv[0], argv[1]);
    Ki_raw = BUILD_32(argv[2], argv[3]);
    Kd_raw = BUILD_32(argv[4], argv[5]);

    nostd::Fixed32 Kp;
    nostd::Fixed32 Ki;
    nostd::Fixed32 Kd;

    Kp.setRawFixed(Kp_raw);
    Ki.setRawFixed(Ki_raw);
    Kd.setRawFixed(Kd_raw);

    return GRPC_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_proc_04_speed_SP_r(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_ERR_INVALID_ARGUMENTS;

    app::Ctrl::speed_t speed = self->m_ctrl.speedFreeGet();
    (*resc) = 1;
    resv[0] = speed;

    return GRPC_ERR_OK;
}

/**
 * @param argc = 1
 * @param argv[0]   imp/rev
 */
uint8_t G602::P_rpc_proc_05_speed_SP_w(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    G602_DEFINE_SELF();

    app::Ctrl::RunMode runMode = self->m_ctrl.runModeGet();
    if(!(
            runMode == app::Ctrl::RunMode::SERVICE_MODE3_STOPPED ||
            runMode == app::Ctrl::RunMode::SERVICE_MODE3_STARTED
    ))
    {
        return GRPC_ERR_INVALID_MODE;
    }
    if(argc != 1) return GRPC_ERR_INVALID_ARGUMENTS;

    unsigned speed = argv[0];
    if(speed > G602_SPEED_MAX)
    {
        return GRPC_ERR_OUT_OF_RANGE;
    }

    self->m_ctrl.speedFreeSet(speed, self);
    return GRPC_ERR_OK;
}

uint8_t G602::P_rpc_proc_06_speed_PV_r(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    //G602_DEFINE_SELF();
    if(argc != 0) return GRPC_ERR_INVALID_ARGUMENTS;

    (*resc) = 1;
    resv[0] = 666;

    return GRPC_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_proc_07_process_start(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_ERR_INVALID_ARGUMENTS;
    self->m_permanent_process_send = true;
    self->m_permanent_process_send_ruid = ruid;
    return GRPC_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_proc_08_process_stop(uint16_t ruid, unsigned argc, uint16_t * argv, unsigned * resc, uint16_t * resv, void * args)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_ERR_INVALID_ARGUMENTS;
    self->m_permanent_process_send = false;
    return GRPC_ERR_OK;
}
