/**
 * @file G602.cpp
 */

#include "G602.hpp"

#define TO_BOOL(x) ((x) != 0)

static const uint16_t P_run_modes[] =
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

G602::G602(
    int baselineSpeedLow,
    int baselineSpeedHigh,
    void (*event_config_store)(const uint8_t * conf, size_t size),
    void (*event_config_load)(uint8_t * conf, size_t size, bool * empty),
    void (*event_strober)(bool on),
    void (*event_lift_up)(),
    void (*event_lift_down)(),
    void (*event_motor_update)(bool state, int setpoint)
)
: m_event_config_store(event_config_store)
, m_event_config_load(event_config_load)
, m_event_strober(event_strober)
, m_event_lift_up(event_lift_up)
, m_event_lift_down(event_lift_down)
, m_event_motor_update(event_motor_update)
, m_time_now(0)
, m_time_next(0)
, m_sched()
, m_blinker()
, m_ctrl(baselineSpeedLow, baselineSpeedHigh, P_ctrl_event, this)
, m_di_gauge_stop(false, P_event_stopUnset, P_event_stopSet, this, DI_DEBOUNCE_TIME)
, m_di_btn_speed_mode(false, P_event_speedMode33,  P_event_speedMode45, this, DI_DEBOUNCE_TIME)
, m_di_btn_autostop(false, P_event_autostopEnable,  P_event_autostopDisable, this, DI_DEBOUNCE_TIME)
, m_di_btn_start(false, P_event_start,  nullptr, this, DI_DEBOUNCE_TIME)
, m_di_btn_stop(false, P_event_stop,  P_event_stop_release, this, DI_DEBOUNCE_TIME)
, m_comm()
, m_rpc(P_rpc_send, this)
, m_buf_frame()
, m_permanent_process_send(false)
, m_Kp()
, m_Ki()
, m_Kd()
, m_pid()
, m_pulses()
{
    P_config_load();

    static const GRPCServer::func_t *funcs[] =
    {
            P_rpc_func_00_pulses_r,
            P_rpc_func_01_mode_current_r,
            P_rpc_func_02_koef_r,
            P_rpc_func_03_koef_w,
            P_rpc_func_04_speed_SP_r,
            P_rpc_func_05_speed_SP_w,
            P_rpc_func_06_speed_PV_r,
            P_rpc_func_07_process_start,
            P_rpc_func_08_process_stop,
            P_rpc_func_09_conf_store,
            nullptr
    };
    m_rpc.funcs_register(funcs);

    m_pid.KpSet(m_Kp);
    m_pid.KiSet(m_Ki);
    m_pid.KdSet(m_Kd);
    m_pid.reset();

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
        m_sched.handle(m_time_now);
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

void G602::P_rpc_eventModeChanged(app::Ctrl::RunMode runMode)
{
    uint8_t resc = 1;
    uint16_t resv[1] = { P_run_modes[ARRAY_INDEX(runMode)] };
    m_rpc.event(
        G602_EVENT_MODE_CHANGED,
        resc,
        resv
    );
}

void G602::P_rpc_eventSPPV(GTime_t time, uint16_t sp, uint16_t pv, fixed32_t out)
{
    uint8_t resc = 6;
    uint16_t resv[6];
    resv[0] = (uint16_t)((uint32_t)time >> 16);
    resv[1] = (uint16_t)((uint32_t)time & 0x0000ffff);
    resv[2] = sp;
    resv[3] = pv;
    resv[4] = (uint16_t)(out >> 16);
    resv[5] = (uint16_t)(out & 0x0000ffff);

    m_rpc.event(
        G602_EVENT_SPPV,
        resc,
        resv
    );
}

#define CONF_K(conf, i) \
        (*((fixed32_t*)&conf[(sizeof(fixed32_t) * i)]))

void G602::P_config_store()
{
    uint8_t conf[3 * sizeof(fixed32_t)];

    CONF_K(conf, 0) = m_Kp.toRawFixed();
    CONF_K(conf, 1) = m_Ki.toRawFixed();
    CONF_K(conf, 2) = m_Kd.toRawFixed();
    m_event_config_store(conf, sizeof(conf));
}

void G602::P_config_load()
{
    bool empty;
    uint8_t conf[3 * sizeof(fixed32_t)];
    m_event_config_load(conf, sizeof(conf), &empty);
    if(empty)
    {
        /* 1 minute to stable speed */
        m_Kp.set(1.17187500);
        m_Ki.set(0.03906250);
        m_Kd.set(2.50000000);
    }
    else
    {
        m_Kp.setRawFixed(CONF_K(conf, 0));
        m_Ki.setRawFixed(CONF_K(conf, 1));
        m_Kd.setRawFixed(CONF_K(conf, 2));
    }
}

unsigned long G602::P_rtcNextTimeGet() const
{
    unsigned long time_next = m_sched.nearestTime();
    return (time_next > 0 ? time_next : m_time_now + 10 );
}

void G602::P_blinker_start(GBlinker::BlinkType type)
{
    unsigned actions = m_blinker.typeSet(type, true);

    if(actions & GBLINKER_ACTIONFLAG_UNSCHEDULE)
    {
        m_sched.unshedule(shed_task_id_blinker);
    }
    if(actions & GBLINKER_ACTIONFLAG_SCHEDULE)
    {
        m_sched.shedule(shed_task_id_blinker, m_time_now, P_task_blinker, this);
        m_time_next = P_rtcNextTimeGet();
    }
}

void G602::P_blinker_stop(GBlinker::BlinkType type)
{
    unsigned actions = m_blinker.typeSet(type, false);

    if(actions & GBLINKER_ACTIONFLAG_UNSCHEDULE)
    {
        m_sched.unshedule(shed_task_id_blinker);
    }
    if(actions & GBLINKER_ACTIONFLAG_SCHEDULE)
    {
        m_sched.shedule(shed_task_id_blinker, m_time_now, P_task_blinker, this);
        m_time_next = P_rtcNextTimeGet();
    }
}

void G602::P_task_blinker(nostd::size_t id, GTime_t time, UNUSED GTime_t now, G602Scheduler & sched, void * args)
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

void G602::P_task_awaiting_service_mode(
        UNUSED nostd::size_t id,
        UNUSED GTime_t time,
        UNUSED GTime_t now,
        UNUSED G602Scheduler & sched,
        void * args
)
{
    G602_DEFINE_SELF();
    self->m_ctrl.mode_service_1(self);
    self->P_blinker_start(GBlinker::BlinkType::FAST6);

    app::Ctrl::RunMode mode_new = self->m_ctrl.runModeGet();
    self->P_rpc_eventModeChanged(mode_new);
}

void G602::P_motor_update()
{
    /* reset the measure and control tasks */
    P_measures_stop();
    P_measures_start();
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
            self->P_motor_update();
            break;
        }

        case app::Ctrl::Event::MOTOR_OFF:
        {
            self->P_motor_update();
            break;
        }

        case app::Ctrl::Event::MOTOR_SETPOINT_UPDATE:
        {
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

    app::Ctrl::RunMode mode = self->m_ctrl.runModeGet();
    switch(mode)
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

    app::Ctrl::RunMode mode_new = self->m_ctrl.runModeGet();
    if(mode != mode_new)
    {
        self->P_rpc_eventModeChanged(mode_new);
    }
}

void G602::P_event_stop(void * args)
{
    G602_DEFINE_SELF();

    app::Ctrl::RunMode mode = self->m_ctrl.runModeGet();
    switch(mode)
    {
        case app::Ctrl::RunMode::NORMAL_STOPPED:
        {
            self->m_sched.shedule(
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

    app::Ctrl::RunMode mode_new = self->m_ctrl.runModeGet();
    if(mode != mode_new)
    {
        self->P_rpc_eventModeChanged(mode_new);
    }
}

void G602::P_event_stop_release(void * args)
{
    G602_DEFINE_SELF();
    self->m_sched.unshedule(shed_task_id_service_mode_awaiting);
    self->m_time_next = self->P_rtcNextTimeGet();
}

void G602::P_comm_reader()
{
    unsigned size = 0;

    GCommBase::Error error = m_comm.readFrame(m_buf_frame, CAPACITY, &size);
    if(error == GCommBase::Error::OK)
    {
        UNUSED GRPCServer::Error err = m_rpc.handle(m_buf_frame, size);
        //DEBUG_PRINT("P_comm_reader(): m_rpc.handle: err = ");
        //DEBUG_PRINTLN((int)err);
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
uint8_t G602::P_rpc_func_00_pulses_r(
        UNUSED unsigned argc,
        UNUSED uint16_t * argv,
        unsigned * resc,
        uint16_t * resv,
        UNUSED void * args
)
{
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    (*resc) = 1;
    resv[0] = G602_TABLE_PULSES_PER_REV;
    return GRPC_REPLY_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_func_01_mode_current_r(
        unsigned argc,
        UNUSED uint16_t * argv,
        unsigned * resc,
        uint16_t * resv,
        void * args
)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    app::Ctrl::RunMode runMode = self->m_ctrl.runModeGet();

    (*resc) = 1;
    resv[0] = P_run_modes[ARRAY_INDEX(runMode)];
    return GRPC_REPLY_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_func_02_koef_r(
        unsigned argc,
        UNUSED uint16_t * argv,
        unsigned * resc,
        uint16_t * resv,
        void * args
)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;
    (*resc) = 6;

    uint32_t Kp_raw = (uint32_t)self->m_Kp.toRawFixed();
    uint32_t Ki_raw = (uint32_t)self->m_Ki.toRawFixed();
    uint32_t Kd_raw = (uint32_t)self->m_Kd.toRawFixed();

    resv[0] = (uint16_t)(Kp_raw >> 16);
    resv[1] = (uint16_t)(Kp_raw & 0x0000ffff);
    resv[2] = (uint16_t)(Ki_raw >> 16);
    resv[3] = (uint16_t)(Ki_raw & 0x0000ffff);
    resv[4] = (uint16_t)(Kd_raw >> 16);
    resv[5] = (uint16_t)(Kd_raw & 0x0000ffff);
    return GRPC_REPLY_ERR_OK;
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
uint8_t G602::P_rpc_func_03_koef_w(
        unsigned argc,
        uint16_t * argv,
        UNUSED unsigned * resc,
        UNUSED uint16_t * resv,
        void * args
)
{
    G602_DEFINE_SELF();
    app::Ctrl::RunMode runMode = self->m_ctrl.runModeGet();
    if(!(
            runMode == app::Ctrl::RunMode::SERVICE_MODE3_STOPPED ||
            runMode == app::Ctrl::RunMode::SERVICE_MODE3_STARTED
    ))
    {
        return GRPC_REPLY_ERR_INVALID_MODE;
    }

    if(argc != 6) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    fixed32_t Kp_raw;
    fixed32_t Ki_raw;
    fixed32_t Kd_raw;
#define BUILD_32(hi, lo) (((uint32_t)(hi) << 16) | (uint32_t)(lo))

    Kp_raw = (fixed32_t)BUILD_32(argv[0], argv[1]);
    Ki_raw = (fixed32_t)BUILD_32(argv[2], argv[3]);
    Kd_raw = (fixed32_t)BUILD_32(argv[4], argv[5]);

    self->m_Kp.setRawFixed(Kp_raw);
    self->m_Ki.setRawFixed(Ki_raw);
    self->m_Kd.setRawFixed(Kd_raw);

    self->m_pid.KpSet(self->m_Kp);
    self->m_pid.KiSet(self->m_Ki);
    self->m_pid.KdSet(self->m_Kd);
    self->m_pid.reset();

    return GRPC_REPLY_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_func_04_speed_SP_r(
        unsigned argc,
        UNUSED uint16_t * argv,
        unsigned * resc,
        uint16_t * resv,
        void * args
)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    app::Ctrl::speed_t speed = self->m_ctrl.speedFreeGet();
    (*resc) = 1;
    resv[0] = (uint16_t)speed;

    return GRPC_REPLY_ERR_OK;
}

/**
 * @param argc = 1
 * @param argv[0]   imp/rev
 */
uint8_t G602::P_rpc_func_05_speed_SP_w(
        unsigned argc,
        uint16_t * argv,
        UNUSED unsigned * resc,
        UNUSED uint16_t * resv,
        void * args
)
{
    G602_DEFINE_SELF();
    app::Ctrl::RunMode runMode = self->m_ctrl.runModeGet();
    if(!(
            runMode == app::Ctrl::RunMode::SERVICE_MODE3_STOPPED ||
            runMode == app::Ctrl::RunMode::SERVICE_MODE3_STARTED
    ))
    {
        return GRPC_REPLY_ERR_INVALID_MODE;
    }
    if(argc != 1) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    unsigned speed = argv[0];
    if(speed > G602_SPEED_MAX)
    {
        return GRPC_REPLY_ERR_OUT_OF_RANGE;
    }

    self->m_ctrl.speedFreeSet(speed, self);
    return GRPC_REPLY_ERR_OK;
}

uint8_t G602::P_rpc_func_06_speed_PV_r(
        unsigned argc,
        UNUSED uint16_t * argv,
        unsigned * resc,
        uint16_t * resv,
        UNUSED void * args
)
{
    //G602_DEFINE_SELF();
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    (*resc) = 1;
    resv[0] = 666;

    return GRPC_REPLY_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_func_07_process_start(
        unsigned argc,
        UNUSED uint16_t * argv,
        UNUSED unsigned * resc,
        UNUSED uint16_t * resv,
        void * args
)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;
    self->m_permanent_process_send = true;
    return GRPC_REPLY_ERR_OK;
}

/**
 * @param argc = 0
 */
uint8_t G602::P_rpc_func_08_process_stop(
        unsigned argc,
        UNUSED uint16_t * argv,
        UNUSED unsigned * resc,
        UNUSED uint16_t * resv,
        void * args
)
{
    G602_DEFINE_SELF();
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;
    self->m_permanent_process_send = false;
    return GRPC_REPLY_ERR_OK;
}

uint8_t G602::P_rpc_func_09_conf_store(
        unsigned argc,
        UNUSED uint16_t * argv,
        UNUSED unsigned * resc,
        UNUSED uint16_t * resv,
        void * args
)
{
    G602_DEFINE_SELF();
    app::Ctrl::RunMode runMode = self->m_ctrl.runModeGet();
    if(!(
            runMode == app::Ctrl::RunMode::SERVICE_MODE3_STOPPED ||
            runMode == app::Ctrl::RunMode::SERVICE_MODE3_STARTED
    ))
    {
        return GRPC_REPLY_ERR_INVALID_MODE;
    }

    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    self->P_config_store();
    return GRPC_REPLY_ERR_OK;
}
