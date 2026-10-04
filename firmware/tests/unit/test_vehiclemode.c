/*
 * Unit tests: VehicleMode SWC.
 *
 * The SWC is linked against a fake RTE defined in this file, which is the standard
 * way to test an AUTOSAR-style SWC in isolation. After every cycle the safety
 * invariant "propulsion enabled <=> mode is ARMED_*" is asserted.
 */
#include <string.h>

#include "Rte_VehicleMode.h"
#include "VehicleMode.h"
#include "unity.h"

/* ---- fake RTE ------------------------------------------------------------------ */

#define QUEUE_LEN 16u

static Rte_OperatorRequestType g_queue[QUEUE_LEN];
static uint32_t                g_qHead;
static uint32_t                g_qTail;
static Rte_FaultClassType      g_fault;
static Rte_FaultClassType      g_faultAfterClear;
static Rte_ArmingInputsType    g_inputs;
static Rte_VehicleModeType     g_mode;
static bool                    g_propulsion;
static float                   g_authority;
static Rte_ArmGuardMaskType    g_rejectMask;
static uint32_t                g_clearCalls;
static Rte_VehicleModeType     g_modeTrace[32];
static uint32_t                g_modeTraceLen;

Std_ReturnType Rte_Read_VehicleMode_OperatorRequest(Rte_OperatorRequestType *req)
{
    if (g_qHead == g_qTail)
    {
        return RTE_E_NO_DATA;
    }
    *req    = g_queue[g_qTail % QUEUE_LEN];
    g_qTail++;
    return RTE_E_OK;
}

Std_ReturnType Rte_Read_VehicleMode_FaultClass(Rte_FaultClassType *cls)
{
    *cls = g_fault;
    return RTE_E_OK;
}

Std_ReturnType Rte_Read_VehicleMode_ArmingInputs(Rte_ArmingInputsType *inputs)
{
    *inputs = g_inputs;
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_VehicleMode_Mode(Rte_VehicleModeType mode)
{
    if ((mode != g_mode) && (g_modeTraceLen < 32u))
    {
        g_modeTrace[g_modeTraceLen++] = mode;
    }
    g_mode = mode;
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_VehicleMode_PropulsionEnable(bool enable)
{
    g_propulsion = enable;
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_VehicleMode_AuthorityLimit(float authority)
{
    g_authority = authority;
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_VehicleMode_ArmRejectReason(Rte_ArmGuardMaskType mask)
{
    g_rejectMask = mask;
    return RTE_E_OK;
}

Std_ReturnType Rte_Call_VehicleMode_ClearFaults(void)
{
    g_clearCalls++;
    g_fault = g_faultAfterClear;
    return RTE_E_OK;
}

/* ---- helpers -------------------------------------------------------------------- */

static const VehicleMode_ConfigType g_cfg = {
    .manualAuthority   = 1.0f,
    .assistedAuthority = 0.8f,
    .degradedAuthority = 0.5f,
};

static bool IsArmed(Rte_VehicleModeType mode)
{
    return (mode == RTE_VEHMODE_ARMED_MANUAL) || (mode == RTE_VEHMODE_ARMED_ASSISTED) ||
           (mode == RTE_VEHMODE_ARMED_DEGRADED);
}

static void Cycle(void)
{
    VehicleMode_MainFunction();
    /* Safety invariant, checked after every single cycle. */
    TEST_ASSERT_EQUAL_MESSAGE(IsArmed(g_mode), g_propulsion, "propulsion must be enabled iff ARMED");
    if (!g_propulsion)
    {
        TEST_ASSERT_EQUAL_FLOAT(0.0f, g_authority);
    }
}

static void Request(Rte_OperatorRequestType req)
{
    g_queue[g_qHead % QUEUE_LEN] = req;
    g_qHead++;
    Cycle();
}

static void AllInputsGood(void)
{
    g_inputs.selfTestPassed = true;
    g_inputs.commandFresh   = true;
    g_inputs.estimatorValid = true;
    g_inputs.batteryOk      = true;
    g_inputs.leakDetected   = false;
    g_inputs.pilotNeutral   = true;
    g_inputs.estopReleased  = true;
}

static void ToStandby(void)
{
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_mode);
}

static void ToManual(void)
{
    ToStandby();
    Request(RTE_OPREQ_ARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_MANUAL, g_mode);
}

static void ToAssisted(void)
{
    ToManual();
    Request(RTE_OPREQ_ASSIST_ON);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_ASSISTED, g_mode);
}

static void ToDegraded(void)
{
    ToAssisted();
    g_inputs.estimatorValid = false;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_DEGRADED, g_mode);
}

static void ToFailsafe(void)
{
    ToManual();
    g_fault           = RTE_FAULT_FAILSAFE;
    g_faultAfterClear = RTE_FAULT_FAILSAFE;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_FAILSAFE, g_mode);
}

static void ToEstop(void)
{
    ToStandby();
    Request(RTE_OPREQ_ESTOP);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_EMERGENCY_STOP, g_mode);
}

void setUp(void)
{
    g_qHead = g_qTail = 0u;
    g_fault           = RTE_FAULT_NONE;
    g_faultAfterClear = RTE_FAULT_NONE;
    g_mode            = RTE_VEHMODE_SAFE;
    g_propulsion      = false;
    g_authority       = 0.0f;
    g_rejectMask      = 0u;
    g_clearCalls      = 0u;
    g_modeTraceLen    = 0u;
    AllInputsGood();
    VehicleMode_Init(&g_cfg);
}

void tearDown(void)
{
}

/* ---- start-up ------------------------------------------------------------------- */

static void test_boots_in_safe_with_propulsion_inhibited(void)
{
    TEST_ASSERT_EQUAL(RTE_VEHMODE_SAFE, g_mode);
    TEST_ASSERT_FALSE(g_propulsion);
}

static void test_safe_to_standby_when_self_test_passed(void)
{
    ToStandby();
}

static void test_safe_stays_without_self_test(void)
{
    g_inputs.selfTestPassed = false;
    Cycle();
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_SAFE, g_mode);
}

static void test_safe_stays_with_failsafe_fault(void)
{
    g_fault = RTE_FAULT_FAILSAFE;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_SAFE, g_mode);
}

static void test_arm_ignored_in_safe(void)
{
    g_inputs.selfTestPassed = false;
    Request(RTE_OPREQ_ARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_SAFE, g_mode);
}

static void test_standby_falls_back_to_safe_when_self_test_lost(void)
{
    ToStandby();
    g_inputs.selfTestPassed = false;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_SAFE, g_mode);
}

/* ---- arming --------------------------------------------------------------------- */

static void test_arm_with_all_guards_enters_manual(void)
{
    ToManual();
    TEST_ASSERT_TRUE(g_propulsion);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, g_authority);
    TEST_ASSERT_EQUAL_HEX16(0u, g_rejectMask);
}

static void test_each_failed_guard_blocks_arming(void)
{
    static const struct
    {
        const char          *name;
        Rte_ArmGuardMaskType bit;
    } guards[] = {
        {"command", RTE_ARM_GUARD_COMMAND_STALE}, {"estimator", RTE_ARM_GUARD_ESTIMATOR},
        {"battery", RTE_ARM_GUARD_BATTERY},       {"leak", RTE_ARM_GUARD_LEAK},
        {"pilot", RTE_ARM_GUARD_PILOT_INPUT},
    };
    uint32_t i;

    for (i = 0u; i < (sizeof(guards) / sizeof(guards[0])); i++)
    {
        setUp();
        ToStandby();
        g_inputs.commandFresh   = (guards[i].bit != RTE_ARM_GUARD_COMMAND_STALE);
        g_inputs.estimatorValid = (guards[i].bit != RTE_ARM_GUARD_ESTIMATOR);
        g_inputs.batteryOk      = (guards[i].bit != RTE_ARM_GUARD_BATTERY);
        g_inputs.leakDetected   = (guards[i].bit == RTE_ARM_GUARD_LEAK);
        g_inputs.pilotNeutral   = (guards[i].bit != RTE_ARM_GUARD_PILOT_INPUT);
        Request(RTE_OPREQ_ARM);
        TEST_ASSERT_EQUAL_MESSAGE(RTE_VEHMODE_STANDBY, g_mode, guards[i].name);
        TEST_ASSERT_EQUAL_HEX16_MESSAGE(guards[i].bit, g_rejectMask, guards[i].name);
        TEST_ASSERT_EQUAL_HEX16(guards[i].bit, VehicleMode_GetLastArmRejectMask());
    }
}

static void test_degraded_fault_blocks_arming_but_advisory_does_not(void)
{
    ToStandby();
    g_fault = RTE_FAULT_DEGRADED;
    Request(RTE_OPREQ_ARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_mode);
    TEST_ASSERT_EQUAL_HEX16(RTE_ARM_GUARD_FAULT_ACTIVE, g_rejectMask);

    g_fault = RTE_FAULT_ADVISORY;
    Request(RTE_OPREQ_ARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_MANUAL, g_mode);
}

static void test_disarm_from_every_armed_substate(void)
{
    ToManual();
    Request(RTE_OPREQ_DISARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_mode);

    setUp();
    ToAssisted();
    Request(RTE_OPREQ_DISARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_mode);

    setUp();
    ToDegraded();
    Request(RTE_OPREQ_DISARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_mode);
}

/* ---- assisted / degraded -------------------------------------------------------- */

static void test_assist_on_and_off(void)
{
    ToAssisted();
    TEST_ASSERT_EQUAL_FLOAT(0.8f, g_authority);
    Request(RTE_OPREQ_ASSIST_OFF);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_MANUAL, g_mode);
}

static void test_assist_rejected_without_estimator(void)
{
    ToManual();
    g_inputs.estimatorValid = false;
    Request(RTE_OPREQ_ASSIST_ON);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_MANUAL, g_mode);
}

static void test_estimator_loss_in_assisted_degrades(void)
{
    ToDegraded();
    TEST_ASSERT_EQUAL_FLOAT(0.5f, g_authority);
    TEST_ASSERT_TRUE(g_propulsion);
}

static void test_degraded_fault_while_armed_degrades(void)
{
    ToManual();
    g_fault = RTE_FAULT_DEGRADED;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_DEGRADED, g_mode);
    Request(RTE_OPREQ_ASSIST_ON);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_DEGRADED, g_mode);
}

static void test_resume_from_degraded_only_when_fault_gone(void)
{
    ToManual();
    g_fault = RTE_FAULT_DEGRADED;
    Cycle();
    Request(RTE_OPREQ_ASSIST_OFF);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_DEGRADED, g_mode);

    g_fault = RTE_FAULT_ADVISORY;
    Cycle();
    TEST_ASSERT_EQUAL_MESSAGE(RTE_VEHMODE_ARMED_DEGRADED, g_mode, "no automatic resume");
    Request(RTE_OPREQ_ASSIST_OFF);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_MANUAL, g_mode);
}

/* ---- failsafe ------------------------------------------------------------------- */

static void test_failsafe_fault_from_every_armed_substate(void)
{
    ToFailsafe();
    TEST_ASSERT_FALSE(g_propulsion);

    setUp();
    ToAssisted();
    g_fault = RTE_FAULT_FAILSAFE;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_FAILSAFE, g_mode);

    setUp();
    ToDegraded();
    g_fault = RTE_FAULT_FAILSAFE;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_FAILSAFE, g_mode);
}

static void test_failsafe_ignores_arm(void)
{
    ToFailsafe();
    g_fault = RTE_FAULT_NONE;
    Request(RTE_OPREQ_ARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_FAILSAFE, g_mode);
}

static void test_failsafe_reset_requires_fault_gone(void)
{
    ToFailsafe();
    Request(RTE_OPREQ_RESET);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_FAILSAFE, g_mode);
    TEST_ASSERT_EQUAL_UINT32(1u, g_clearCalls);

    g_faultAfterClear = RTE_FAULT_NONE;
    Request(RTE_OPREQ_RESET);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_mode);
    TEST_ASSERT_EQUAL_UINT32(2u, g_clearCalls);
}

static void test_rearm_after_failsafe_needs_deliberate_arm(void)
{
    ToFailsafe();
    g_faultAfterClear = RTE_FAULT_NONE;
    Request(RTE_OPREQ_RESET);
    Cycle();
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_mode);
    Request(RTE_OPREQ_ARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_ARMED_MANUAL, g_mode);
}

/* ---- emergency stop -------------------------------------------------------------- */

static void test_estop_request_from_every_state(void)
{
    static void (*const reach[])(void) = {ToStandby, ToManual, ToAssisted, ToDegraded, ToFailsafe};
    uint32_t i;

    /* SAFE */
    g_inputs.selfTestPassed = false;
    Request(RTE_OPREQ_ESTOP);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_EMERGENCY_STOP, g_mode);

    for (i = 0u; i < (sizeof(reach) / sizeof(reach[0])); i++)
    {
        setUp();
        reach[i]();
        Request(RTE_OPREQ_ESTOP);
        TEST_ASSERT_EQUAL(RTE_VEHMODE_EMERGENCY_STOP, g_mode);
        TEST_ASSERT_FALSE(g_propulsion);
    }
}

static void test_emergency_fault_triggers_estop(void)
{
    ToManual();
    g_fault = RTE_FAULT_EMERGENCY;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_EMERGENCY_STOP, g_mode);

    setUp();
    ToStandby();
    g_fault = RTE_FAULT_EMERGENCY;
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_EMERGENCY_STOP, g_mode);
}

static void test_emergency_fault_preempts_requests_in_same_cycle(void)
{
    ToStandby();
    g_fault = RTE_FAULT_EMERGENCY;
    Request(RTE_OPREQ_ARM);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_EMERGENCY_STOP, g_mode);
}

static void test_estop_is_latched(void)
{
    ToEstop();
    Request(RTE_OPREQ_ARM);
    Request(RTE_OPREQ_DISARM);
    Request(RTE_OPREQ_ASSIST_ON);
    Request(RTE_OPREQ_ESTOP);
    Cycle();
    TEST_ASSERT_EQUAL(RTE_VEHMODE_EMERGENCY_STOP, g_mode);
}

static void test_estop_reset_conditions(void)
{
    ToEstop();

    g_inputs.estopReleased = false;
    Request(RTE_OPREQ_RESET);
    TEST_ASSERT_EQUAL_MESSAGE(RTE_VEHMODE_EMERGENCY_STOP, g_mode, "e-stop still engaged");

    g_inputs.estopReleased = true;
    g_inputs.pilotNeutral  = false;
    Request(RTE_OPREQ_RESET);
    TEST_ASSERT_EQUAL_MESSAGE(RTE_VEHMODE_EMERGENCY_STOP, g_mode, "pilot not neutral");

    g_inputs.pilotNeutral = true;
    g_fault               = RTE_FAULT_EMERGENCY;
    g_faultAfterClear     = RTE_FAULT_EMERGENCY;
    Request(RTE_OPREQ_RESET);
    TEST_ASSERT_EQUAL_MESSAGE(RTE_VEHMODE_EMERGENCY_STOP, g_mode, "emergency fault active");

    g_faultAfterClear = RTE_FAULT_NONE;
    g_modeTraceLen    = 0u;
    Request(RTE_OPREQ_RESET);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_mode);
    /* passes through SAFE (self-test re-run) in the same cycle */
    TEST_ASSERT_EQUAL_UINT32(2u, g_modeTraceLen);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_SAFE, g_modeTrace[0]);
    TEST_ASSERT_EQUAL(RTE_VEHMODE_STANDBY, g_modeTrace[1]);
}

/* ---- robustness ------------------------------------------------------------------ */

static void test_requests_per_cycle_are_bounded(void)
{
    uint32_t i;

    ToStandby();
    for (i = 0u; i < 6u; i++)
    {
        g_queue[g_qHead % QUEUE_LEN] = RTE_OPREQ_NONE;
        g_qHead++;
    }
    Cycle();
    TEST_ASSERT_EQUAL_UINT32(2u, g_qHead - g_qTail);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_boots_in_safe_with_propulsion_inhibited);
    RUN_TEST(test_safe_to_standby_when_self_test_passed);
    RUN_TEST(test_safe_stays_without_self_test);
    RUN_TEST(test_safe_stays_with_failsafe_fault);
    RUN_TEST(test_arm_ignored_in_safe);
    RUN_TEST(test_standby_falls_back_to_safe_when_self_test_lost);
    RUN_TEST(test_arm_with_all_guards_enters_manual);
    RUN_TEST(test_each_failed_guard_blocks_arming);
    RUN_TEST(test_degraded_fault_blocks_arming_but_advisory_does_not);
    RUN_TEST(test_disarm_from_every_armed_substate);
    RUN_TEST(test_assist_on_and_off);
    RUN_TEST(test_assist_rejected_without_estimator);
    RUN_TEST(test_estimator_loss_in_assisted_degrades);
    RUN_TEST(test_degraded_fault_while_armed_degrades);
    RUN_TEST(test_resume_from_degraded_only_when_fault_gone);
    RUN_TEST(test_failsafe_fault_from_every_armed_substate);
    RUN_TEST(test_failsafe_ignores_arm);
    RUN_TEST(test_failsafe_reset_requires_fault_gone);
    RUN_TEST(test_rearm_after_failsafe_needs_deliberate_arm);
    RUN_TEST(test_estop_request_from_every_state);
    RUN_TEST(test_emergency_fault_triggers_estop);
    RUN_TEST(test_emergency_fault_preempts_requests_in_same_cycle);
    RUN_TEST(test_estop_is_latched);
    RUN_TEST(test_estop_reset_conditions);
    RUN_TEST(test_requests_per_cycle_are_bounded);
    return UNITY_END();
}
