/**
 * @file    VehicleMode.c
 * @brief   Vehicle operating-mode state machine.
 *
 * State hierarchy (all transitions documented in docs/software/state_machines.md):
 *
 *   Root                       ESTOP request / EMERGENCY fault  -> EMERGENCY_STOP
 *   +-- SAFE                   self-test passed                 -> STANDBY
 *   +-- STANDBY                ARM + all guards                 -> ARMED (MANUAL)
 *   +-- ARMED                  DISARM -> STANDBY, FAILSAFE fault -> FAILSAFE,
 *   |   |                      DEGRADED fault -> ARMED.DEGRADED
 *   |   +-- MANUAL             ASSIST_ON + estimator valid       -> ASSISTED
 *   |   +-- ASSISTED           ASSIST_OFF -> MANUAL, estimator lost -> DEGRADED
 *   |   +-- DEGRADED           ASSIST_OFF (resume) + no fault    -> MANUAL
 *   +-- FAILSAFE               RESET + fault gone                -> STANDBY
 *   +-- EMERGENCY_STOP         RESET + no emergency + e-stop released + neutral -> SAFE
 *
 * Propulsion is enabled only while inside ARMED (entry/exit actions), so every path
 * out of ARMED disables it by construction.
 */
#include "VehicleMode.h"

#include "Fsm.h"
#include "Rte_VehicleMode.h"

#define VEHMODE_MAX_REQUESTS_PER_CYCLE 4u

enum
{
    VEHMODE_EV_TICK = 1,
    VEHMODE_EV_ARM,
    VEHMODE_EV_DISARM,
    VEHMODE_EV_ASSIST_ON,
    VEHMODE_EV_ASSIST_OFF,
    VEHMODE_EV_ESTOP,
    VEHMODE_EV_RESET
};

typedef struct
{
    const VehicleMode_ConfigType *cfg;
    Rte_ArmingInputsType          inputs;
    Rte_FaultClassType            faultClass;
    Rte_VehicleModeType           mode;
    bool                          propulsion;
    float                         authority;
    Rte_ArmGuardMaskType          rejectMask;
} VehicleMode_CtxType;

static VehicleMode_CtxType VehicleMode_Ctx;
static Fsm_MachineType     VehicleMode_Fsm;

/* ---- state declarations ---------------------------------------------------- */

static Fsm_ResultType Root_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           Safe_Entry(Fsm_MachineType *m);
static Fsm_ResultType Safe_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           Standby_Entry(Fsm_MachineType *m);
static Fsm_ResultType Standby_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           Armed_Entry(Fsm_MachineType *m);
static void           Armed_Exit(Fsm_MachineType *m);
static Fsm_ResultType Armed_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           Manual_Entry(Fsm_MachineType *m);
static Fsm_ResultType Manual_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           Assisted_Entry(Fsm_MachineType *m);
static Fsm_ResultType Assisted_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           Degraded_Entry(Fsm_MachineType *m);
static Fsm_ResultType Degraded_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           Failsafe_Entry(Fsm_MachineType *m);
static Fsm_ResultType Failsafe_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           Estop_Entry(Fsm_MachineType *m);
static Fsm_ResultType Estop_Handler(Fsm_MachineType *m, const Fsm_EventType *e);

static const Fsm_StateType St_Root;
static const Fsm_StateType St_Safe;
static const Fsm_StateType St_Standby;
static const Fsm_StateType St_Armed;
static const Fsm_StateType St_Manual;
static const Fsm_StateType St_Assisted;
static const Fsm_StateType St_Degraded;
static const Fsm_StateType St_Failsafe;
static const Fsm_StateType St_Estop;

static const Fsm_StateType St_Root     = {"ROOT", NULL, &St_Safe, NULL, NULL, Root_Handler};
static const Fsm_StateType St_Safe     = {"SAFE", &St_Root, NULL, Safe_Entry, NULL, Safe_Handler};
static const Fsm_StateType St_Standby  = {"STANDBY", &St_Root, NULL, Standby_Entry, NULL, Standby_Handler};
static const Fsm_StateType St_Armed    = {"ARMED", &St_Root, &St_Manual, Armed_Entry, Armed_Exit, Armed_Handler};
static const Fsm_StateType St_Manual   = {"MANUAL", &St_Armed, NULL, Manual_Entry, NULL, Manual_Handler};
static const Fsm_StateType St_Assisted = {"ASSISTED", &St_Armed, NULL, Assisted_Entry, NULL, Assisted_Handler};
static const Fsm_StateType St_Degraded = {"DEGRADED", &St_Armed, NULL, Degraded_Entry, NULL, Degraded_Handler};
static const Fsm_StateType St_Failsafe = {"FAILSAFE", &St_Root, NULL, Failsafe_Entry, NULL, Failsafe_Handler};
static const Fsm_StateType St_Estop    = {"EMERGENCY_STOP", &St_Root, NULL, Estop_Entry, NULL, Estop_Handler};

/* ---- helpers --------------------------------------------------------------- */

static VehicleMode_CtxType *Ctx(Fsm_MachineType *m)
{
    return (VehicleMode_CtxType *)m->context;
}

/* Published from entry actions so that every state passed through is observable,
 * even when a cycle chains two transitions (e.g. RESET: E-STOP -> SAFE -> STANDBY). */
static void SetMode(Fsm_MachineType *m, Rte_VehicleModeType mode, float authority)
{
    Ctx(m)->mode      = mode;
    Ctx(m)->authority = authority;
    (void)Rte_Write_VehicleMode_Mode(mode);
}

static Rte_ArmGuardMaskType EvaluateArmGuards(const VehicleMode_CtxType *c)
{
    Rte_ArmGuardMaskType mask = 0u;

    if (!c->inputs.selfTestPassed)
    {
        mask |= RTE_ARM_GUARD_SELF_TEST;
    }
    if (!c->inputs.commandFresh)
    {
        mask |= RTE_ARM_GUARD_COMMAND_STALE;
    }
    if (!c->inputs.estimatorValid)
    {
        mask |= RTE_ARM_GUARD_ESTIMATOR;
    }
    if (!c->inputs.batteryOk)
    {
        mask |= RTE_ARM_GUARD_BATTERY;
    }
    if (c->inputs.leakDetected)
    {
        mask |= RTE_ARM_GUARD_LEAK;
    }
    if (!c->inputs.pilotNeutral)
    {
        mask |= RTE_ARM_GUARD_PILOT_INPUT;
    }
    if (c->faultClass >= RTE_FAULT_DEGRADED)
    {
        mask |= RTE_ARM_GUARD_FAULT_ACTIVE;
    }
    return mask;
}

static void RefreshInputs(VehicleMode_CtxType *c)
{
    Rte_FaultClassType cls = RTE_FAULT_EMERGENCY; /* fail safe if the read fails */

    if (Rte_Read_VehicleMode_FaultClass(&cls) != RTE_E_OK)
    {
        cls = RTE_FAULT_EMERGENCY;
    }
    c->faultClass = cls;

    if (Rte_Read_VehicleMode_ArmingInputs(&c->inputs) != RTE_E_OK)
    {
        c->inputs.selfTestPassed = false;
        c->inputs.commandFresh   = false;
        c->inputs.estimatorValid = false;
        c->inputs.batteryOk      = false;
        c->inputs.leakDetected   = true;
        c->inputs.pilotNeutral   = false;
        c->inputs.estopReleased  = false;
    }
}

/* Operator fault reset: clear inactive latched faults, then re-evaluate inputs. */
static void ClearFaultsAndRefresh(VehicleMode_CtxType *c)
{
    (void)Rte_Call_VehicleMode_ClearFaults();
    RefreshInputs(c);
}

/* ---- state behaviour ------------------------------------------------------- */

static Fsm_ResultType Root_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    if (e->id == VEHMODE_EV_ESTOP)
    {
        return Fsm_Transition(m, &St_Estop);
    }
    return FSM_HANDLED; /* every other unhandled request is ignored */
}

static void Safe_Entry(Fsm_MachineType *m)
{
    SetMode(m, RTE_VEHMODE_SAFE, 0.0f);
}

static Fsm_ResultType Safe_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    if ((e->id == VEHMODE_EV_TICK) && Ctx(m)->inputs.selfTestPassed && (Ctx(m)->faultClass < RTE_FAULT_FAILSAFE))
    {
        return Fsm_Transition(m, &St_Standby);
    }
    return FSM_IGNORED;
}

static void Standby_Entry(Fsm_MachineType *m)
{
    SetMode(m, RTE_VEHMODE_STANDBY, 0.0f);
}

static Fsm_ResultType Standby_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    VehicleMode_CtxType *c = Ctx(m);

    switch (e->id)
    {
    case VEHMODE_EV_ARM:
        c->rejectMask = EvaluateArmGuards(c);
        (void)Rte_Write_VehicleMode_ArmRejectReason(c->rejectMask);
        return (c->rejectMask == 0u) ? Fsm_Transition(m, &St_Armed) : FSM_HANDLED;
    case VEHMODE_EV_TICK:
        return c->inputs.selfTestPassed ? FSM_IGNORED : Fsm_Transition(m, &St_Safe);
    default:
        return FSM_IGNORED;
    }
}

static void Armed_Entry(Fsm_MachineType *m)
{
    Ctx(m)->propulsion = true;
}

static void Armed_Exit(Fsm_MachineType *m)
{
    Ctx(m)->propulsion = false;
    Ctx(m)->authority  = 0.0f;
}

static Fsm_ResultType Armed_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    const VehicleMode_CtxType *c = Ctx(m);

    switch (e->id)
    {
    case VEHMODE_EV_DISARM:
        return Fsm_Transition(m, &St_Standby);
    case VEHMODE_EV_TICK:
        if (c->faultClass == RTE_FAULT_FAILSAFE)
        {
            return Fsm_Transition(m, &St_Failsafe);
        }
        if ((c->faultClass == RTE_FAULT_DEGRADED) && !Fsm_IsIn(m, &St_Degraded))
        {
            return Fsm_Transition(m, &St_Degraded);
        }
        return FSM_IGNORED;
    default:
        return FSM_IGNORED;
    }
}

static void Manual_Entry(Fsm_MachineType *m)
{
    SetMode(m, RTE_VEHMODE_ARMED_MANUAL, Ctx(m)->cfg->manualAuthority);
}

static Fsm_ResultType Manual_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    if (e->id == VEHMODE_EV_ASSIST_ON)
    {
        return Ctx(m)->inputs.estimatorValid ? Fsm_Transition(m, &St_Assisted) : FSM_HANDLED;
    }
    return FSM_IGNORED;
}

static void Assisted_Entry(Fsm_MachineType *m)
{
    SetMode(m, RTE_VEHMODE_ARMED_ASSISTED, Ctx(m)->cfg->assistedAuthority);
}

static Fsm_ResultType Assisted_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    switch (e->id)
    {
    case VEHMODE_EV_ASSIST_OFF:
        return Fsm_Transition(m, &St_Manual);
    case VEHMODE_EV_TICK:
        /* Faults first (handled by ARMED), then loss of estimate degrades the mode. */
        if ((Ctx(m)->faultClass < RTE_FAULT_DEGRADED) && !Ctx(m)->inputs.estimatorValid)
        {
            return Fsm_Transition(m, &St_Degraded);
        }
        return FSM_IGNORED;
    default:
        return FSM_IGNORED;
    }
}

static void Degraded_Entry(Fsm_MachineType *m)
{
    SetMode(m, RTE_VEHMODE_ARMED_DEGRADED, Ctx(m)->cfg->degradedAuthority);
}

static Fsm_ResultType Degraded_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    switch (e->id)
    {
    case VEHMODE_EV_ASSIST_ON:
        return FSM_HANDLED; /* assisted modes are not available while degraded */
    case VEHMODE_EV_ASSIST_OFF:
        return (Ctx(m)->faultClass <= RTE_FAULT_ADVISORY) ? Fsm_Transition(m, &St_Manual) : FSM_HANDLED;
    default:
        return FSM_IGNORED;
    }
}

static void Failsafe_Entry(Fsm_MachineType *m)
{
    SetMode(m, RTE_VEHMODE_FAILSAFE, 0.0f);
}

static Fsm_ResultType Failsafe_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    VehicleMode_CtxType *c = Ctx(m);

    switch (e->id)
    {
    case VEHMODE_EV_RESET:
        ClearFaultsAndRefresh(c);
        return (c->faultClass < RTE_FAULT_FAILSAFE) ? Fsm_Transition(m, &St_Standby) : FSM_HANDLED;
    case VEHMODE_EV_ESTOP:
        return FSM_IGNORED; /* root handles it */
    default:
        return FSM_HANDLED; /* stay put, ignore ARM etc. */
    }
}

static void Estop_Entry(Fsm_MachineType *m)
{
    SetMode(m, RTE_VEHMODE_EMERGENCY_STOP, 0.0f);
}

static Fsm_ResultType Estop_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    VehicleMode_CtxType *c = Ctx(m);

    if (e->id == VEHMODE_EV_RESET)
    {
        ClearFaultsAndRefresh(c);
        if ((c->faultClass < RTE_FAULT_EMERGENCY) && c->inputs.estopReleased && c->inputs.pilotNeutral)
        {
            return Fsm_Transition(m, &St_Safe);
        }
    }
    return FSM_HANDLED; /* latched: everything else, including repeated ESTOP, is absorbed */
}

/* ---- runnable interface ---------------------------------------------------- */

static Fsm_EventIdType MapRequest(Rte_OperatorRequestType req)
{
    switch (req)
    {
    case RTE_OPREQ_ARM:
        return VEHMODE_EV_ARM;
    case RTE_OPREQ_DISARM:
        return VEHMODE_EV_DISARM;
    case RTE_OPREQ_ASSIST_ON:
        return VEHMODE_EV_ASSIST_ON;
    case RTE_OPREQ_ASSIST_OFF:
        return VEHMODE_EV_ASSIST_OFF;
    case RTE_OPREQ_ESTOP:
        return VEHMODE_EV_ESTOP;
    case RTE_OPREQ_RESET:
        return VEHMODE_EV_RESET;
    default:
        return 0u;
    }
}

static void Dispatch(Fsm_EventIdType id)
{
    const Fsm_EventType ev = {id, 0u};
    (void)Fsm_Dispatch(&VehicleMode_Fsm, &ev);
}

static void PublishOutputs(void)
{
    (void)Rte_Write_VehicleMode_Mode(VehicleMode_Ctx.mode);
    (void)Rte_Write_VehicleMode_PropulsionEnable(VehicleMode_Ctx.propulsion);
    (void)Rte_Write_VehicleMode_AuthorityLimit(VehicleMode_Ctx.authority);
}

void VehicleMode_Init(const VehicleMode_ConfigType *config)
{
    VehicleMode_Ctx.cfg        = config;
    VehicleMode_Ctx.faultClass = RTE_FAULT_NONE;
    VehicleMode_Ctx.mode       = RTE_VEHMODE_SAFE;
    VehicleMode_Ctx.propulsion = false;
    VehicleMode_Ctx.authority  = 0.0f;
    VehicleMode_Ctx.rejectMask = 0u;
    (void)Fsm_Init(&VehicleMode_Fsm, &St_Safe, &VehicleMode_Ctx, NULL);
    PublishOutputs();
}

void VehicleMode_MainFunction(void)
{
    Rte_OperatorRequestType req;
    uint32_t                n = 0u;

    RefreshInputs(&VehicleMode_Ctx);

    /* 1. Emergency faults pre-empt everything else in this cycle. */
    if (VehicleMode_Ctx.faultClass == RTE_FAULT_EMERGENCY)
    {
        Dispatch(VEHMODE_EV_ESTOP);
    }

    /* 2. Operator requests, bounded per cycle. */
    while ((n < VEHMODE_MAX_REQUESTS_PER_CYCLE) && (Rte_Read_VehicleMode_OperatorRequest(&req) == RTE_E_OK))
    {
        Fsm_EventIdType id = MapRequest(req);
        if (id != 0u)
        {
            Dispatch(id);
        }
        n++;
    }

    /* 3. Level-triggered evaluation of faults and guards. */
    Dispatch(VEHMODE_EV_TICK);

    PublishOutputs();
}

Rte_VehicleModeType VehicleMode_GetMode(void)
{
    return VehicleMode_Ctx.mode;
}

Rte_ArmGuardMaskType VehicleMode_GetLastArmRejectMask(void)
{
    return VehicleMode_Ctx.rejectMask;
}
