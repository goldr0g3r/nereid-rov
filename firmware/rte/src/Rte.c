/**
 * @file    Rte.c
 * @brief   Hand-written Run-Time Environment for the Nereid ECU.
 *
 * Responsibilities (docs/software/firmware_architecture.md#rte):
 *  - own the inter-SWC signal buffers (last-is-best) and request queues (FIFO);
 *  - translate SWC ports into BSW calls (IoHwAb, BswM, Dem);
 *  - define the task bodies, i.e. the order in which BSW main functions and SWC
 *    runnables execute.
 * This is the only place where application and basic software meet.
 */
#include "Rte_BenchPanel.h"
#include "Rte_Lights.h"
#include "Rte_Main.h"
#include "Rte_StatusLed.h"
#include "Rte_VehicleMode.h"

#include "BenchPanel.h"
#include "Lights.h"
#include "StatusLed.h"
#include "VehicleMode.h"

#include "BswM.h"
#include "Dem.h"
#include "EcuM.h"
#include "IoHwAb.h"
#include "Log.h"
#include "Os.h"
#include "RingBuf.h"
#include "Swc_Cfg.h"
#include "WdgM.h"

STD_STATIC_ASSERT((int)RTE_FAULT_EMERGENCY == (int)DEM_CLASS_EMERGENCY, "Rte/Dem fault class mismatch");
STD_STATIC_ASSERT((int)RTE_FAULT_ADVISORY == (int)DEM_CLASS_ADVISORY, "Rte/Dem fault class mismatch");

#define RTE_OPREQ_QUEUE_SIZE  16u
#define RTE_LIGHTS_QUEUE_SIZE 8u

/* ---- signal buffers ---------------------------------------------------------- */

static Rte_VehicleModeType Rte_Mode;
static bool                Rte_PropulsionEnable;
static float               Rte_AuthorityLimit;

static uint8_t      Rte_OpReqStorage[RTE_OPREQ_QUEUE_SIZE];
static RingBuf_Type Rte_OpReqQueue;
static uint8_t      Rte_LightsStorage[RTE_LIGHTS_QUEUE_SIZE];
static RingBuf_Type Rte_LightsQueue;

static Std_ReturnType Rte_QueuePut(RingBuf_Type *q, uint8_t value)
{
    Std_ReturnType ret;

    Os_EnterCritical(); /* queues may have several senders */
    ret = RingBuf_Put(q, value);
    Os_ExitCritical();
    return ret;
}

static Std_ReturnType Rte_QueueGet(RingBuf_Type *q, uint8_t *value)
{
    return (RingBuf_Get(q, value) == E_OK) ? RTE_E_OK : RTE_E_NO_DATA;
}

const char *Rte_GetVehicleModeName(Rte_VehicleModeType mode)
{
    static const char *const names[RTE_VEHMODE_COUNT] = {
        "SAFE", "STANDBY", "ARMED_MANUAL", "ARMED_ASSISTED", "ARMED_DEGRADED", "FAILSAFE", "EMERGENCY_STOP",
    };
    return ((uint32_t)mode < (uint32_t)RTE_VEHMODE_COUNT) ? names[mode] : "?";
}

/* ---- VehicleMode ports ------------------------------------------------------- */

Std_ReturnType Rte_Read_VehicleMode_OperatorRequest(Rte_OperatorRequestType *req)
{
    uint8_t        v   = 0u;
    Std_ReturnType ret = Rte_QueueGet(&Rte_OpReqQueue, &v);

    *req = (Rte_OperatorRequestType)v;
    return ret;
}

Std_ReturnType Rte_Read_VehicleMode_FaultClass(Rte_FaultClassType *cls)
{
    *cls = (Rte_FaultClassType)BswM_GetFaultClass();
    return RTE_E_OK;
}

Std_ReturnType Rte_Read_VehicleMode_ArmingInputs(Rte_ArmingInputsType *inputs)
{
    inputs->selfTestPassed = BswM_IsSelfTestPassed();

    /* BENCH STUBS - there is no propulsion hardware, link, estimator or power
     * monitoring yet. Each line is replaced by a real provider in its milestone:
     *   commandFresh   <- Com heartbeat age        (M2)
     *   pilotNeutral   <- Com pilot axes           (M2)
     *   estimatorValid <- StateEst health          (M4)
     *   batteryOk      <- PowerMon                 (M6)
     *   leakDetected   <- LeakMon                  (M6)
     *   estopReleased  <- e-stop input / topside   (M2/M6)
     * Do NOT connect thrusters before these stubs are gone. */
    inputs->commandFresh   = true;
    inputs->pilotNeutral   = true;
    inputs->estimatorValid = true;
    inputs->batteryOk      = true;
    inputs->leakDetected   = false;
    inputs->estopReleased  = true;
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_VehicleMode_Mode(Rte_VehicleModeType mode)
{
    if (mode != Rte_Mode)
    {
        LOG_INFO("VehicleMode", "%s -> %s", Rte_GetVehicleModeName(Rte_Mode), Rte_GetVehicleModeName(mode));
        Rte_Mode = mode;
    }
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_VehicleMode_PropulsionEnable(bool enable)
{
    if (enable != Rte_PropulsionEnable)
    {
        LOG_INFO("VehicleMode", "propulsion %s", enable ? "ENABLED" : "inhibited");
        Rte_PropulsionEnable = enable;
    }
    BswM_NotifyPropulsionEnabled(enable);
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_VehicleMode_AuthorityLimit(float authority)
{
    Rte_AuthorityLimit = authority; /* consumer: thrust allocation (M3) */
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_VehicleMode_ArmRejectReason(Rte_ArmGuardMaskType mask)
{
    if (mask != 0u)
    {
        LOG_WARN("VehicleMode", "ARM rejected, failed guards 0x%04x", (unsigned)mask);
    }
    return RTE_E_OK;
}

Std_ReturnType Rte_Call_VehicleMode_ClearFaults(void)
{
    Dem_ClearAllEvents();
    return RTE_E_OK;
}

/* ---- StatusLed ports ---------------------------------------------------------- */

Std_ReturnType Rte_Read_StatusLed_Mode(Rte_VehicleModeType *mode)
{
    *mode = Rte_Mode;
    return RTE_E_OK;
}

Std_ReturnType Rte_Read_StatusLed_FaultClass(Rte_FaultClassType *cls)
{
    *cls = (Rte_FaultClassType)BswM_GetFaultClass();
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_StatusLed_GreenLed(bool on)
{
    return IoHwAb_SetDigitalOutput(IOHWAB_OUT_STATUS_LED, on);
}

Std_ReturnType Rte_Write_StatusLed_RedLed(bool on)
{
    return IoHwAb_SetDigitalOutput(IOHWAB_OUT_FAULT_LED, on);
}

/* ---- BenchPanel ports --------------------------------------------------------- */

Std_ReturnType Rte_Read_BenchPanel_UserButton(bool *pressed)
{
    return IoHwAb_GetDigitalInput(IOHWAB_IN_USER_BUTTON, pressed);
}

Std_ReturnType Rte_Read_BenchPanel_Mode(Rte_VehicleModeType *mode)
{
    *mode = Rte_Mode;
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_BenchPanel_OperatorRequest(Rte_OperatorRequestType req)
{
    return Rte_QueuePut(&Rte_OpReqQueue, (uint8_t)req);
}

/* ---- Lights ports ------------------------------------------------------------- */

Std_ReturnType Rte_Read_Lights_Request(Rte_LightsRequestType *req)
{
    uint8_t        v   = 0u;
    Std_ReturnType ret = Rte_QueueGet(&Rte_LightsQueue, &v);

    *req = (Rte_LightsRequestType)v;
    return ret;
}

Std_ReturnType Rte_Write_Lights_Output(bool on)
{
    return IoHwAb_SetDigitalOutput(IOHWAB_OUT_LIGHTS, on);
}

Std_ReturnType Rte_Send_LightsRequest(Rte_LightsRequestType req)
{
    return Rte_QueuePut(&Rte_LightsQueue, (uint8_t)req);
}

/* ---- life cycle and task bodies ----------------------------------------------- */

Std_ReturnType Rte_Start(void)
{
    Rte_Mode             = RTE_VEHMODE_SAFE;
    Rte_PropulsionEnable = false;
    Rte_AuthorityLimit   = 0.0f;
    (void)RingBuf_Init(&Rte_OpReqQueue, Rte_OpReqStorage, RTE_OPREQ_QUEUE_SIZE);
    (void)RingBuf_Init(&Rte_LightsQueue, Rte_LightsStorage, RTE_LIGHTS_QUEUE_SIZE);

    VehicleMode_Init(&VehicleMode_Config);
    BenchPanel_Init(&BenchPanel_Config);
    StatusLed_Init();
    Lights_Init();
    return E_OK;
}

void Rte_Task_Control10ms(void)
{
    WdgM_CheckpointReached(WDGM_SE_CONTROL_10MS);
    IoHwAb_MainFunction();    /* 1. sample + debounce inputs        */
    BswM_MainFunction();      /* 2. arbitrate BSW state             */
    BenchPanel_MainFunction(); /* 3. operator requests              */
    VehicleMode_MainFunction(); /* 4. mode decision (outputs gated) */
    Lights_MainFunction();    /* 5. auxiliary outputs               */
    /* M3+: StateEst -> DepthCtrl/HeadingCtrl -> ThrustAlloc -> IoHwAb thrusters */
}

void Rte_Task_Bsw100ms(void)
{
    StatusLed_MainFunction();
    EcuM_MainFunction();
    WdgM_MainFunction(); /* last-but-one: judge the cycle that just ended */
    Log_MainFunction();  /* last: lowest value, may block on the UART     */
}
