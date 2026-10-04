/**
 * @file    EcuM.c
 * @brief   ECU state manager.
 */
#include "EcuM.h"

#include "BswM.h"
#include "Dem.h"
#include "Fsm.h"
#include "IoHwAb.h"
#include "Log.h"
#include "Mcu.h"
#include "Os.h"

enum
{
    ECUM_EV_INIT_DONE = 1,
    ECUM_EV_RESET_REQUEST
};

static Fsm_ResultType EcuM_StartupHandler(Fsm_MachineType *m, const Fsm_EventType *e);
static Fsm_ResultType EcuM_RunHandler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           EcuM_RunEntry(Fsm_MachineType *m);
static void           EcuM_ShutdownEntry(Fsm_MachineType *m);

static const Fsm_StateType EcuM_Startup  = {"STARTUP", NULL, NULL, NULL, NULL, EcuM_StartupHandler};
static const Fsm_StateType EcuM_Run      = {"RUN", NULL, NULL, EcuM_RunEntry, NULL, EcuM_RunHandler};
static const Fsm_StateType EcuM_Shutdown = {"SHUTDOWN", NULL, NULL, EcuM_ShutdownEntry, NULL, NULL};

static Fsm_MachineType EcuM_Machine;
static volatile bool   EcuM_ResetRequested;

static Fsm_ResultType EcuM_StartupHandler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    return (e->id == ECUM_EV_INIT_DONE) ? Fsm_Transition(m, &EcuM_Run) : FSM_HANDLED;
}

static Fsm_ResultType EcuM_RunHandler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    return (e->id == ECUM_EV_RESET_REQUEST) ? Fsm_Transition(m, &EcuM_Shutdown) : FSM_HANDLED;
}

static void EcuM_RunEntry(Fsm_MachineType *m)
{
    STD_UNUSED(m);
    LOG_INFO("EcuM", "RUN");
}

static void EcuM_ShutdownEntry(Fsm_MachineType *m)
{
    STD_UNUSED(m);
    IoHwAb_SetAllOutputsSafe();
    LOG_INFO("EcuM", "SHUTDOWN -> reset");
    Log_Flush();
    Mcu_PerformReset();
}

void EcuM_Init(void)
{
    Mcu_ResetType reset;

    EcuM_ResetRequested = false;
    (void)Fsm_Init(&EcuM_Machine, &EcuM_Startup, NULL, NULL);

    EcuM_Cfg_InitListZero();

    reset = Mcu_GetResetReason();
    LOG_INFO("EcuM", "Nereid ECU firmware, variant %s, reset reason %s", EcuM_Cfg_GetVariantName(),
             Mcu_GetResetReasonName(reset));
    if (reset == MCU_RESET_WATCHDOG)
    {
        (void)Dem_SetEventStatus(DEM_EVENT_WATCHDOG_RESET, DEM_EVENT_STATUS_FAILED);
    }

    if (EcuM_Cfg_InitListOne() != E_OK)
    {
        EcuM_ErrorHook(ECUM_ERROR_INIT_FAILED, "InitListOne");
    }

    {
        const Fsm_EventType ev = {ECUM_EV_INIT_DONE, 0u};
        (void)Fsm_Dispatch(&EcuM_Machine, &ev);
    }
    Os_Start();
}

void EcuM_MainFunction(void)
{
    if (EcuM_ResetRequested && BswM_IsShutdownAllowed())
    {
        const Fsm_EventType ev = {ECUM_EV_RESET_REQUEST, 0u};
        EcuM_ResetRequested    = false;
        (void)Fsm_Dispatch(&EcuM_Machine, &ev);
    }
}

void EcuM_RequestReset(void)
{
    EcuM_ResetRequested = true;
}

EcuM_StateType EcuM_GetState(void)
{
    const Fsm_StateType *s = Fsm_GetState(&EcuM_Machine);

    if (s == &EcuM_Run)
    {
        return ECUM_STATE_RUN;
    }
    if (s == &EcuM_Shutdown)
    {
        return ECUM_STATE_SHUTDOWN;
    }
    return ECUM_STATE_STARTUP;
}

const char *EcuM_GetStateName(EcuM_StateType state)
{
    static const char *const names[] = {"STARTUP", "RUN", "SHUTDOWN"};
    return ((uint32_t)state < STD_ARRAY_SIZE(names)) ? names[state] : "?";
}

void EcuM_ErrorHook(EcuM_ErrorType error, const char *detail)
{
    static const char *const names[] = {"OS assert", "stack overflow", "init failed", "hard fault"};

    IoHwAb_SetAllOutputsSafe();
    LOG_ERROR("EcuM", "FATAL: %s (%s)", ((uint32_t)error < STD_ARRAY_SIZE(names)) ? names[error] : "?",
              (detail != NULL) ? detail : "-");
    Log_Flush();
    Mcu_PerformReset();
}
