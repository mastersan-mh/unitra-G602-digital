/**
 * @file G602.cpp
 */

#include "G602.hpp"

#define G602_DEFINE_SELF() \
    G602 * self = static_cast<G602*>(args)

#define TO_BOOL(x) ((x) != 0)

#define SPLIT32_HI(x)  (static_cast<uint16_t>(static_cast<uint32_t>(x) >> 16))
#define SPLIT32_LO(x) (static_cast<uint16_t>(static_cast<uint32_t>(x) & 0x0000ffff))
#define BUILD_32(hi, lo)  ((static_cast<uint32_t>(hi) << 16) | static_cast<uint32_t>(lo))

#define IN_RANGE(x, l, r) ((l) <= (x) && (x) <= (r))

static const uint16_t P_run_modes[] =
{
        [ARRAY_INDEX(Ctrl::RunMode::NORMAL_STOPPED       )] = 0x1000, /* 1.1: stopped */
        [ARRAY_INDEX(Ctrl::RunMode::NORMAL_STARTED_AUTO  )] = 0x1001, /* 1.2: auto */
        [ARRAY_INDEX(Ctrl::RunMode::NORMAL_STARTED_MANUAL)] = 0x1002, /* 1.3: manual */
        [ARRAY_INDEX(Ctrl::RunMode::SERVICE_MODE1_STOPPED)] = 0x2100, /* 2.1: stopped */
        [ARRAY_INDEX(Ctrl::RunMode::SERVICE_MODE1_STARTED)] = 0x2101, /* 2.1: started */
        [ARRAY_INDEX(Ctrl::RunMode::SERVICE_MODE2_STOPPED)] = 0x2200, /* 2.2: stopped */
        [ARRAY_INDEX(Ctrl::RunMode::SERVICE_MODE2_STARTED)] = 0x2201, /* 2.2: started */
        [ARRAY_INDEX(Ctrl::RunMode::SERVICE_MODE3_STOPPED)] = 0x2300, /* 2.3: stopped */
        [ARRAY_INDEX(Ctrl::RunMode::SERVICE_MODE3_STARTED)] = 0x2301, /* 2.3: started */
};

G602::G602(
    void (*event_config_store)(const uint8_t * conf, size_t size),
    void (*event_config_load)(uint8_t * conf, size_t size, bool * empty),
    void (*event_strober)(bool on),
    void (*event_lift_up)(),
    void (*event_lift_down)(),
    void (*event_motor_update)(bool state, int setpoint),
    void (*event_pulses_get)(unsigned * dmotor_pulses, unsigned * dtable_pulses)
)
: m_event_config_store(event_config_store)
, m_event_config_load(event_config_load)
, m_event_strober(event_strober)
, m_event_lift_up(event_lift_up)
, m_event_lift_down(event_lift_down)
, m_event_motor_update(event_motor_update)
, m_event_pulses_get(event_pulses_get)
, m_ctrl(m_conf_baselineSpeedLow, m_conf_baselineSpeedHigh, P_ctrl_event, this)
, m_di_gauge_stop(false, P_event_stopUnset, P_event_stopSet, this, DI_DEBOUNCE_TIME)
, m_di_btn_speed_mode(false, P_event_speedMode33,  P_event_speedMode45, this, DI_DEBOUNCE_TIME)
, m_di_btn_autostop(false, P_event_autostopEnable,  P_event_autostopDisable, this, DI_DEBOUNCE_TIME)
, m_di_btn_start(false, P_event_start,  nullptr, this, DI_DEBOUNCE_TIME)
, m_di_btn_stop(false, P_event_stop,  P_event_stop_release, this, DI_DEBOUNCE_TIME)
, m_rpc(P_rpc_send, this)
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
    Ctrl::internal_state_t state;
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

void G602::manualSpeedSet(Speed speed)
{
    m_ctrl.speedManualSet(speed, this);
}

void G602::P_rpc_eventModeChanged(Ctrl::RunMode runMode)
{
    uint8_t resc = 1;
    uint16_t resv[1] = { P_run_modes[ARRAY_INDEX(runMode)] };
    m_rpc.event(
        G602_EVENT_MODE_CHANGED,
        resc,
        resv
    );
}

void G602::P_rpc_eventSPPV(GTime_t time, Speed sp, Speed pv, Speed out)
{
    uint8_t resc = 8;
    uint16_t resv[8];
    resv[0] = SPLIT32_HI(time);
    resv[1] = SPLIT32_LO(time);

    const uint32_t sp_raw = sp.toRawFixed();
    resv[2] = SPLIT32_HI(sp_raw);
    resv[3] = SPLIT32_LO(sp_raw);

    const uint32_t pv_raw = pv.toRawFixed();
    resv[4] = SPLIT32_HI(pv_raw);
    resv[5] = SPLIT32_LO(pv_raw);

    const uint32_t out_raw = out.toRawFixed();
    resv[6] = SPLIT32_LO(out_raw);
    resv[7] = SPLIT32_LO(out_raw);

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
        m_Kp.setDouble(G602_PID_DEFAULT_KP);
        m_Ki.setDouble(G602_PID_DEFAULT_KI);
        m_Kd.setDouble(G602_PID_DEFAULT_KD);
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
}

void G602::P_task_ctrl(
        size_t id,
        GTime_t time,
        UNUSED GTime_t now,
        G602Scheduler & sched,
        void * args
)
{
    G602_DEFINE_SELF();

    unsigned motor_dpulses;
    unsigned table_dpulses;

    self->m_event_pulses_get(&motor_dpulses, &table_dpulses);

    self->m_pulses_sum.add(table_dpulses);
    const unsigned pulses_per_window_raw = self->m_pulses_sum.sum();
    const Speed pulses_per_window(pulses_per_window_raw, Speed::tag_int);
    const Speed window_time_ms(ctrl_handler_period * self->m_pulses_sum.size(), Speed::tag_int);
    const Speed window_time_s = window_time_ms / 1000;
    const Speed rpm = (pulses_per_window * 60) / (window_time_s * m_conf_table_pulses_per_revolution);

    const Speed speed_PV_raw = rpm;

    bool motor_state;
    Speed table_setpoint;
    self->m_ctrl.motorGet(&motor_state, &table_setpoint);
    const Speed speed_SP = (motor_state ? table_setpoint : Speed());

    /* filter */
#ifdef G602_USE_FILTER

    Speed speed_PV;
    if(IN_RANGE(
            speed_PV_raw,
            speed_SP - m_const_value1div2,
            speed_SP + m_const_value1div2
    ))
    {
        speed_PV = speed_SP;
    }
    else
    {
        speed_PV = speed_PV_raw;
    }
#else
    const Speed speed_PV = speed_PV_raw;
#endif

    self->m_ctrl.actualSpeedUpdate(speed_PV, self);

    const Speed ctrl = self->m_pid.calculate(speed_SP, speed_PV_raw);

    fixed32_t ctrl_raw = ctrl.toRawFixed();
    if(ctrl_raw < 0)
    {
        ctrl_raw = 0;
    }
    // DEBUG_PRINT("ctrl_raw = "); DEBUG_PRINTLN(ctrl_raw);

#if 1
    int ctrl_int = (int)(ctrl_raw >> 16); /* cut-off frac */
    // DEBUG_PRINT("ctrl_int = "); DEBUG_PRINTLN(ctrl_int);
    int motor_output = constrain(ctrl_int, 0, 255);
#else
    int motor_output = (int)map(
            speed_SP,
            G602_SPEED_MIN,
            G602_SPEED_MAX,
            0,
            255
    );

    if(!motor_state) motor_output = 0;
#endif

    self->m_event_motor_update(motor_state, motor_output);

    if(self->m_send_counter < ctrl_send_factor)
    {
        self->m_send_counter++;
    }
    else
    {
        DEBUG_PRINT("table_dpulses = "); DEBUG_PRINTLN(table_dpulses);
        DEBUG_PRINT("pulses_per_window_raw = "); DEBUG_PRINTLN(pulses_per_window_raw);
        DEBUG_PRINT("pulses_per_window = "); DEBUG_PRINTLN(pulses_per_window.toDouble());
        DEBUG_PRINT("rpm = "); DEBUG_PRINTLN(rpm.toDouble());

        if(self->m_permanent_process_send)
        {
            self->P_rpc_eventSPPV(time, speed_SP, speed_PV_raw, ctrl);
        }
        self->m_send_counter = 0;
    }

    sched.shedule(
            id,
            time + ctrl_handler_period,
            P_task_ctrl,
            args
    );
    self->m_time_next = self->P_rtcNextTimeGet();
}

void G602::P_ctrl_start()
{
    unsigned motor_dpulses;
    unsigned table_dpulses;
    m_event_pulses_get(&motor_dpulses, &table_dpulses);

    m_pid.reset();
    m_pulses_sum.reset();

    m_sched.shedule(
            shed_task_id_ctrl,
            m_time_now + ctrl_handler_period,
            P_task_ctrl,
            this
    );
    m_time_next = P_rtcNextTimeGet();
}

void G602::P_ctrl_stop()
{
    m_sched.unshedule(shed_task_id_ctrl);
    m_time_next = P_rtcNextTimeGet();
}

void G602::P_motor_update()
{
    /* reset the measure and control tasks */
    P_ctrl_stop();
    P_ctrl_start();
}

void G602::P_ctrl_event(Ctrl::Event event, const Ctrl::EventData &data, void * args)
{
    G602_DEFINE_SELF();
    switch(event)
    {
        case Ctrl::Event::WARNINGS_UPDATE:
        {
            bool blink_speed_to_low  = TO_BOOL(data.WARNINGS_UPDATE.warnings & CTRL_WARNING_SPEED_TOO_LOW);
            bool blink_speed_to_high = TO_BOOL(data.WARNINGS_UPDATE.warnings & CTRL_WARNING_SPEED_TOO_HIGH);

            if(blink_speed_to_low)
            {
                //self->P_blinker_start(GBlinker::BlinkType::SLOW);
            }
            else
            {
                //self->P_blinker_stop(GBlinker::BlinkType::SLOW);
            }

            if(blink_speed_to_high)
            {
                //self->P_blinker_start(GBlinker::BlinkType::FAST);
            }
            else
            {
                //self->P_blinker_stop(GBlinker::BlinkType::FAST);
            }

            break;
        }

        case Ctrl::Event::MOTOR_ON:
        {
            self->P_motor_update();
            break;
        }

        case Ctrl::Event::MOTOR_OFF:
        {
            self->P_motor_update();
            break;
        }

        case Ctrl::Event::MOTOR_SETPOINT_UPDATE:
        {
            break;
        }

        case Ctrl::Event::LIFT_UP:
        {
            self->m_event_lift_up();
            break;
        }

        case Ctrl::Event::LIFT_DOWN:
        {
            self->m_event_lift_down();
            break;
        }

        case Ctrl::Event::RUNMODE_CHANGED:
        {
            self->P_rpc_eventModeChanged(data.RUNMODE_CHANGED.runMode);
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
    self->m_ctrl.baselineSpeedModeSet(Ctrl::BaselineSpeedMode::MODE_HIGH, self);
}

void G602::P_event_speedMode33(void * args)
{
    G602_DEFINE_SELF();
    self->m_ctrl.baselineSpeedModeSet(Ctrl::BaselineSpeedMode::MODE_LOW, self);
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

    Ctrl::RunMode mode = self->m_ctrl.runModeGet();
    switch(mode)
    {
        case Ctrl::RunMode::NORMAL_STOPPED:
        {
            self->m_ctrl.start(self);
            self->P_blinker_start(GBlinker::BlinkType::ACCELERATING);
            break;
        }
        case Ctrl::RunMode::NORMAL_STARTED_AUTO:
        {
            self->m_ctrl.start(self);
            self->P_blinker_start(GBlinker::BlinkType::B3TIME);
            break;
        }
        case Ctrl::RunMode::NORMAL_STARTED_MANUAL:
        {
            self->m_ctrl.start(self);
            self->P_blinker_start(GBlinker::BlinkType::B1TIME);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE1_STOPPED:
        {
            self->m_ctrl.start(self);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE1_STARTED:
        {
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE2_STOPPED:
        {
            self->m_ctrl.start(self);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE2_STARTED:
        {
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE3_STOPPED:
        {
            self->m_ctrl.start(self);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE3_STARTED:
        {
            break;
        }
    }
}

void G602::P_event_stop(void * args)
{
    G602_DEFINE_SELF();

    Ctrl::RunMode mode = self->m_ctrl.runModeGet();
    switch(mode)
    {
        case Ctrl::RunMode::NORMAL_STOPPED:
        {
            self->m_sched.shedule(
                shed_task_id_service_mode_awaiting,
                self->m_time_now + service_mode_enter_awaiting_time,
                P_task_awaiting_service_mode,
                self
            );
            break;
        }
        case Ctrl::RunMode::NORMAL_STARTED_AUTO:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
        case Ctrl::RunMode::NORMAL_STARTED_MANUAL:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE1_STOPPED:
        {
            self->m_ctrl.mode_service_2(self);
            self->P_blinker_start(GBlinker::BlinkType::B2TIME);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE1_STARTED:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE2_STOPPED:
        {
            self->m_ctrl.mode_service_3(self);
            self->P_blinker_start(GBlinker::BlinkType::B3TIME);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE2_STARTED:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE3_STOPPED:
        {
            self->m_ctrl.mode_normal(self);
            self->P_blinker_start(GBlinker::BlinkType::FAST6);
            break;
        }
        case Ctrl::RunMode::SERVICE_MODE3_STARTED:
        {
            self->m_ctrl.stop(self);
            self->P_blinker_start(GBlinker::BlinkType::BRAKING);
            break;
        }
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

    Ctrl::RunMode runMode = self->m_ctrl.runModeGet();

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

    const fixed32_t Kp_raw = self->m_Kp.toRawFixed();
    const fixed32_t Ki_raw = self->m_Ki.toRawFixed();
    const fixed32_t Kd_raw = self->m_Kd.toRawFixed();

    resv[0] = SPLIT32_HI(Kp_raw);
    resv[1] = SPLIT32_LO(Kp_raw);
    resv[2] = SPLIT32_HI(Ki_raw);
    resv[3] = SPLIT32_LO(Ki_raw);
    resv[4] = SPLIT32_HI(Kd_raw);
    resv[5] = SPLIT32_LO(Kd_raw);
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
    Ctrl::RunMode runMode = self->m_ctrl.runModeGet();
    if(!(
            runMode == Ctrl::RunMode::SERVICE_MODE3_STOPPED ||
            runMode == Ctrl::RunMode::SERVICE_MODE3_STARTED
    ))
    {
        return GRPC_REPLY_ERR_INVALID_MODE;
    }

    if(argc != 6) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    self->m_Kp.setRawFixed(BUILD_32(argv[0], argv[1]));
    self->m_Ki.setRawFixed(BUILD_32(argv[2], argv[3]));
    self->m_Kd.setRawFixed(BUILD_32(argv[4], argv[5]));

    self->m_pid.KpSet(self->m_Kp);
    self->m_pid.KiSet(self->m_Ki);
    self->m_pid.KdSet(self->m_Kd);
    self->m_pid.reset();
    self->m_pulses_sum.reset();

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
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;
    G602_DEFINE_SELF();

    const fixed32_t speed = self->m_ctrl.speedFreeGet().toRawFixed();

    (*resc) = 2;
    resv[0] = SPLIT32_HI(speed);
    resv[1] = SPLIT32_LO(speed);

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
    if(argc != 2) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    G602_DEFINE_SELF();
    const Ctrl::RunMode runMode = self->m_ctrl.runModeGet();
    if(!(
            runMode == Ctrl::RunMode::SERVICE_MODE3_STOPPED ||
            runMode == Ctrl::RunMode::SERVICE_MODE3_STARTED
    ))
    {
        return GRPC_REPLY_ERR_INVALID_MODE;
    }

    const fixed32_t speed_raw = BUILD_32(argv[0], argv[1]);

    Speed speed(speed_raw, Speed::tag_raw);

    if(speed > Speed(G602_SPEED_MAX * 1.0, Speed::tag_double))
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
    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    G602_DEFINE_SELF();
    const fixed32_t pv_raw = self->m_ctrl.actualSpeed().toRawFixed();

    (*resc) = 2;
    resv[0] = SPLIT32_HI(pv_raw);
    resv[1] = SPLIT32_LO(pv_raw);

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
    Ctrl::RunMode runMode = self->m_ctrl.runModeGet();
    if(!(
            runMode == Ctrl::RunMode::SERVICE_MODE3_STOPPED ||
            runMode == Ctrl::RunMode::SERVICE_MODE3_STARTED
    ))
    {
        return GRPC_REPLY_ERR_INVALID_MODE;
    }

    if(argc != 0) return GRPC_REPLY_ERR_INVALID_ARGUMENTS_AMOUNT;

    self->P_config_store();
    return GRPC_REPLY_ERR_OK;
}
