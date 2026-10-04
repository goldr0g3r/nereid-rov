/* Unit tests: WdgM alive supervision. */
#include "Dem.h"
#include "WdgM.h"
#include "unity.h"

extern uint32_t            Stub_WdgTimeoutMs;
extern uint32_t            Stub_WdgTriggerCount;
extern uint32_t            Stub_DemReportCount[DEM_EVENT_COUNT];

static const WdgM_SupervisedEntityCfgType g_entities[] = {{"Control", 8u, 12u}};
static const WdgM_ConfigType              g_cfg        = {500u, 1u, g_entities, 1u};

static void Alive(uint32_t n)
{
    while (n-- > 0u)
    {
        WdgM_CheckpointReached(WDGM_SE_CONTROL_10MS);
    }
}

void setUp(void)
{
    Stub_WdgTriggerCount = 0u;
    Stub_DemReportCount[DEM_EVENT_WDGM_SUPERVISION] = 0u;
    WdgM_Init(&g_cfg);
    WdgM_MainFunction(); /* warm-up cycle always triggers */
    Stub_WdgTriggerCount = 0u;
}

void tearDown(void)
{
}

static void test_init_starts_watchdog_with_configured_timeout(void)
{
    TEST_ASSERT_EQUAL_UINT32(500u, Stub_WdgTimeoutMs);
}

static void test_healthy_entity_triggers_every_cycle(void)
{
    uint32_t i;
    for (i = 0u; i < 5u; i++)
    {
        Alive(10u);
        WdgM_MainFunction();
    }
    TEST_ASSERT_EQUAL_UINT32(5u, Stub_WdgTriggerCount);
    TEST_ASSERT_EQUAL(WDGM_GLOBAL_STATUS_OK, WdgM_GetGlobalStatus());
}

static void test_too_few_checkpoints_fail_then_expire(void)
{
    Alive(3u);
    WdgM_MainFunction(); /* failed cycle 1: within tolerance, still triggers */
    TEST_ASSERT_EQUAL(WDGM_GLOBAL_STATUS_FAILED, WdgM_GetGlobalStatus());
    TEST_ASSERT_EQUAL_UINT32(1u, Stub_WdgTriggerCount);
    TEST_ASSERT_EQUAL_UINT32(1u, Stub_DemReportCount[DEM_EVENT_WDGM_SUPERVISION]);

    WdgM_MainFunction(); /* failed cycle 2: tolerance exceeded, stop triggering */
    TEST_ASSERT_EQUAL(WDGM_GLOBAL_STATUS_EXPIRED, WdgM_GetGlobalStatus());
    TEST_ASSERT_EQUAL_UINT32(1u, Stub_WdgTriggerCount);
}

static void test_too_many_checkpoints_also_fail(void)
{
    Alive(20u); /* runaway task */
    WdgM_MainFunction();
    TEST_ASSERT_EQUAL(WDGM_GLOBAL_STATUS_FAILED, WdgM_GetGlobalStatus());
}

static void test_expired_never_recovers(void)
{
    WdgM_MainFunction();
    WdgM_MainFunction();
    TEST_ASSERT_EQUAL(WDGM_GLOBAL_STATUS_EXPIRED, WdgM_GetGlobalStatus());
    Stub_WdgTriggerCount = 0u;
    Alive(10u);
    WdgM_MainFunction();
    TEST_ASSERT_EQUAL_UINT32(0u, Stub_WdgTriggerCount);
}

static void test_recovery_within_tolerance(void)
{
    Alive(3u);
    WdgM_MainFunction();
    Alive(10u);
    WdgM_MainFunction();
    TEST_ASSERT_EQUAL(WDGM_GLOBAL_STATUS_OK, WdgM_GetGlobalStatus());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_starts_watchdog_with_configured_timeout);
    RUN_TEST(test_healthy_entity_triggers_every_cycle);
    RUN_TEST(test_too_few_checkpoints_fail_then_expire);
    RUN_TEST(test_too_many_checkpoints_also_fail);
    RUN_TEST(test_expired_never_recovers);
    RUN_TEST(test_recovery_within_tolerance);
    return UNITY_END();
}
