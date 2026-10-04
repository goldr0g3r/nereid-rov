/**
 * @file    Rte_Types.h
 * @brief   Data types exchanged between SWCs through the RTE.
 *
 * Application SWCs see only these types and their own Rte_<Swc>.h; they never include
 * BSW headers. Keep this file free of BSW dependencies.
 */
#ifndef RTE_TYPES_H
#define RTE_TYPES_H

#include "Std_Types.h"

#define RTE_E_OK      ((Std_ReturnType)0x00u)
#define RTE_E_NO_DATA ((Std_ReturnType)0x83u) /* queued port is empty (AUTOSAR value) */

/** Vehicle operating mode (docs/software/state_machines.md, docs/architecture/system_overview.md 5). */
typedef enum
{
    RTE_VEHMODE_SAFE = 0,
    RTE_VEHMODE_STANDBY,
    RTE_VEHMODE_ARMED_MANUAL,
    RTE_VEHMODE_ARMED_ASSISTED,
    RTE_VEHMODE_ARMED_DEGRADED,
    RTE_VEHMODE_FAILSAFE,
    RTE_VEHMODE_EMERGENCY_STOP,
    RTE_VEHMODE_COUNT
} Rte_VehicleModeType;

/** Worst active fault class, same ordering as the safety case fault response matrix. */
typedef enum
{
    RTE_FAULT_NONE = 0,
    RTE_FAULT_ADVISORY,
    RTE_FAULT_DEGRADED,
    RTE_FAULT_FAILSAFE,
    RTE_FAULT_EMERGENCY
} Rte_FaultClassType;

/** Operator requests (bench button today, topside commands from M2). */
typedef enum
{
    RTE_OPREQ_NONE = 0,
    RTE_OPREQ_ARM,
    RTE_OPREQ_DISARM,
    RTE_OPREQ_ASSIST_ON,
    RTE_OPREQ_ASSIST_OFF, /* also "resume manual" from DEGRADED */
    RTE_OPREQ_ESTOP,
    RTE_OPREQ_RESET
} Rte_OperatorRequestType;

typedef enum
{
    RTE_LIGHTS_REQ_NONE = 0,
    RTE_LIGHTS_REQ_TOGGLE,
    RTE_LIGHTS_REQ_ON,
    RTE_LIGHTS_REQ_OFF
} Rte_LightsRequestType;

/** Inputs to the arming / mode guards. */
typedef struct
{
    bool selfTestPassed; /**< ECU in RUN, configuration valid */
    bool commandFresh;   /**< operator link alive and recent */
    bool estimatorValid; /**< state estimate usable for assisted modes */
    bool batteryOk;
    bool leakDetected;
    bool pilotNeutral; /**< pilot axes centred */
    bool estopReleased; /**< physical / topside e-stop not active */
} Rte_ArmingInputsType;

/** Bit mask of failed arming guards (0 = arming permitted). */
typedef uint16_t Rte_ArmGuardMaskType;

#define RTE_ARM_GUARD_SELF_TEST     ((Rte_ArmGuardMaskType)0x0001u)
#define RTE_ARM_GUARD_COMMAND_STALE ((Rte_ArmGuardMaskType)0x0002u)
#define RTE_ARM_GUARD_ESTIMATOR     ((Rte_ArmGuardMaskType)0x0004u)
#define RTE_ARM_GUARD_BATTERY       ((Rte_ArmGuardMaskType)0x0008u)
#define RTE_ARM_GUARD_LEAK          ((Rte_ArmGuardMaskType)0x0010u)
#define RTE_ARM_GUARD_PILOT_INPUT   ((Rte_ArmGuardMaskType)0x0020u)
#define RTE_ARM_GUARD_FAULT_ACTIVE  ((Rte_ArmGuardMaskType)0x0040u)

const char *Rte_GetVehicleModeName(Rte_VehicleModeType mode);

#endif /* RTE_TYPES_H */
