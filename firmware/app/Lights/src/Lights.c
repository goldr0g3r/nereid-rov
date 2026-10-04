/**
 * @file    Lights.c
 * @brief   Lights state machine: OFF <-> ON driven by TOGGLE / ON / OFF requests.
 *          The output follows the state through entry actions, so the state is the
 *          single source of truth (no separate "previous state" bookkeeping).
 */
#include "Lights.h"

#include "Fsm.h"
#include "Rte_Lights.h"

#define LIGHTS_MAX_REQUESTS_PER_CYCLE 4u

static void           Off_Entry(Fsm_MachineType *m);
static Fsm_ResultType Off_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static void           On_Entry(Fsm_MachineType *m);
static Fsm_ResultType On_Handler(Fsm_MachineType *m, const Fsm_EventType *e);

static const Fsm_StateType St_Off = {"LIGHTS_OFF", NULL, NULL, Off_Entry, NULL, Off_Handler};
static const Fsm_StateType St_On  = {"LIGHTS_ON", NULL, NULL, On_Entry, NULL, On_Handler};

static Fsm_MachineType Lights_Fsm;

static void Off_Entry(Fsm_MachineType *m)
{
    STD_UNUSED(m);
    (void)Rte_Write_Lights_Output(false);
}

static Fsm_ResultType Off_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    if ((e->id == (Fsm_EventIdType)RTE_LIGHTS_REQ_TOGGLE) || (e->id == (Fsm_EventIdType)RTE_LIGHTS_REQ_ON))
    {
        return Fsm_Transition(m, &St_On);
    }
    return FSM_HANDLED;
}

static void On_Entry(Fsm_MachineType *m)
{
    STD_UNUSED(m);
    (void)Rte_Write_Lights_Output(true);
}

static Fsm_ResultType On_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    if ((e->id == (Fsm_EventIdType)RTE_LIGHTS_REQ_TOGGLE) || (e->id == (Fsm_EventIdType)RTE_LIGHTS_REQ_OFF))
    {
        return Fsm_Transition(m, &St_Off);
    }
    return FSM_HANDLED;
}

void Lights_Init(void)
{
    (void)Fsm_Init(&Lights_Fsm, &St_Off, NULL, NULL);
}

void Lights_MainFunction(void)
{
    Rte_LightsRequestType req;
    uint32_t              n = 0u;

    while ((n < LIGHTS_MAX_REQUESTS_PER_CYCLE) && (Rte_Read_Lights_Request(&req) == RTE_E_OK))
    {
        const Fsm_EventType ev = {(Fsm_EventIdType)req, 0u};
        (void)Fsm_Dispatch(&Lights_Fsm, &ev);
        n++;
    }
}

bool Lights_IsOn(void)
{
    return Fsm_GetState(&Lights_Fsm) == &St_On;
}
