/* Unit tests: Dem (debounce, latching, fault classes, operator clear). */
#include "Dem.h"
#include "unity.h"

/* Test configuration (replaces the variant's Dem_Cfg.c). */
const Dem_EventCfgType Dem_EventCfg[DEM_EVENT_COUNT] = {
    [DEM_EVENT_DEV_ERROR]        = {"DEV_ERROR", DEM_CLASS_ADVISORY, 3, -2, false},
    [DEM_EVENT_TASK_OVERRUN]     = {"TASK_OVERRUN", DEM_CLASS_DEGRADED, 1, -1, false},
    [DEM_EVENT_WDGM_SUPERVISION] = {"WDGM", DEM_CLASS_FAILSAFE, 1, -1, true},
    [DEM_EVENT_WATCHDOG_RESET]   = {"WDG_RESET", DEM_CLASS_EMERGENCY, 1, -1, true},
    [DEM_EVENT_CONFIG_INVALID]   = {"CONFIG", DEM_CLASS_FAILSAFE, 1, -1, false},
};

static bool Failed(Dem_EventIdType id)
{
    bool f = false;
    TEST_ASSERT_EQUAL(E_OK, Dem_GetEventFailed(id, &f));
    return f;
}

void setUp(void)
{
    Dem_Init();
}

void tearDown(void)
{
}

static void test_counter_debounce_qualifies_after_threshold(void)
{
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREFAILED);
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREFAILED);
    TEST_ASSERT_FALSE(Failed(DEM_EVENT_DEV_ERROR));
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREFAILED);
    TEST_ASSERT_TRUE(Failed(DEM_EVENT_DEV_ERROR));
}

static void test_prepassed_resets_failure_counting(void)
{
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREFAILED);
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREFAILED);
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREPASSED);
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREFAILED);
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREFAILED);
    TEST_ASSERT_FALSE(Failed(DEM_EVENT_DEV_ERROR));
}

static void test_unlatched_event_heals_after_pass_threshold(void)
{
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_FAILED);
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREPASSED);
    TEST_ASSERT_TRUE(Failed(DEM_EVENT_DEV_ERROR));
    Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_PREPASSED);
    TEST_ASSERT_FALSE(Failed(DEM_EVENT_DEV_ERROR));
}

static void test_latched_event_stays_failed_until_cleared(void)
{
    Dem_SetEventStatus(DEM_EVENT_WDGM_SUPERVISION, DEM_EVENT_STATUS_FAILED);
    Dem_SetEventStatus(DEM_EVENT_WDGM_SUPERVISION, DEM_EVENT_STATUS_PASSED);
    TEST_ASSERT_TRUE(Failed(DEM_EVENT_WDGM_SUPERVISION));
    Dem_ClearAllEvents();
    TEST_ASSERT_FALSE(Failed(DEM_EVENT_WDGM_SUPERVISION));
}

static void test_clear_keeps_events_whose_condition_is_still_active(void)
{
    Dem_SetEventStatus(DEM_EVENT_WDGM_SUPERVISION, DEM_EVENT_STATUS_FAILED);
    Dem_ClearAllEvents();
    TEST_ASSERT_TRUE(Failed(DEM_EVENT_WDGM_SUPERVISION));
}

static void test_worst_active_class(void)
{
    TEST_ASSERT_EQUAL(DEM_CLASS_NONE, Dem_GetWorstActiveClass());
    Dem_SetEventStatus(DEM_EVENT_TASK_OVERRUN, DEM_EVENT_STATUS_FAILED);
    TEST_ASSERT_EQUAL(DEM_CLASS_DEGRADED, Dem_GetWorstActiveClass());
    Dem_SetEventStatus(DEM_EVENT_CONFIG_INVALID, DEM_EVENT_STATUS_FAILED);
    TEST_ASSERT_EQUAL(DEM_CLASS_FAILSAFE, Dem_GetWorstActiveClass());
    Dem_SetEventStatus(DEM_EVENT_CONFIG_INVALID, DEM_EVENT_STATUS_PASSED);
    TEST_ASSERT_EQUAL(DEM_CLASS_DEGRADED, Dem_GetWorstActiveClass());
}

static void test_occurrence_counter_counts_qualified_failures(void)
{
    Dem_SetEventStatus(DEM_EVENT_TASK_OVERRUN, DEM_EVENT_STATUS_FAILED);
    Dem_SetEventStatus(DEM_EVENT_TASK_OVERRUN, DEM_EVENT_STATUS_FAILED);
    Dem_SetEventStatus(DEM_EVENT_TASK_OVERRUN, DEM_EVENT_STATUS_PASSED);
    Dem_SetEventStatus(DEM_EVENT_TASK_OVERRUN, DEM_EVENT_STATUS_FAILED);
    TEST_ASSERT_EQUAL_UINT16(2u, Dem_GetOccurrenceCount(DEM_EVENT_TASK_OVERRUN));
}

static void test_invalid_event_id_rejected(void)
{
    bool f;
    TEST_ASSERT_EQUAL(E_NOT_OK, Dem_SetEventStatus(DEM_EVENT_COUNT, DEM_EVENT_STATUS_FAILED));
    TEST_ASSERT_EQUAL(E_NOT_OK, Dem_GetEventFailed(DEM_EVENT_COUNT, &f));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_counter_debounce_qualifies_after_threshold);
    RUN_TEST(test_prepassed_resets_failure_counting);
    RUN_TEST(test_unlatched_event_heals_after_pass_threshold);
    RUN_TEST(test_latched_event_stays_failed_until_cleared);
    RUN_TEST(test_clear_keeps_events_whose_condition_is_still_active);
    RUN_TEST(test_worst_active_class);
    RUN_TEST(test_occurrence_counter_counts_qualified_failures);
    RUN_TEST(test_invalid_event_id_rejected);
    return UNITY_END();
}
