/* Unit tests: StatusLed SWC (pattern table and sequencing). */
#include "Rte_StatusLed.h"
#include "StatusLed.h"
#include "unity.h"

static Rte_VehicleModeType g_mode;
static Rte_FaultClassType  g_cls;
static bool                g_green;
static bool                g_red;

Std_ReturnType Rte_Read_StatusLed_Mode(Rte_VehicleModeType *mode)
{
    *mode = g_mode;
    return RTE_E_OK;
}
Std_ReturnType Rte_Read_StatusLed_FaultClass(Rte_FaultClassType *cls)
{
    *cls = g_cls;
    return RTE_E_OK;
}
Std_ReturnType Rte_Write_StatusLed_GreenLed(bool on)
{
    g_green = on;
    return RTE_E_OK;
}
Std_ReturnType Rte_Write_StatusLed_RedLed(bool on)
{
    g_red = on;
    return RTE_E_OK;
}

void setUp(void)
{
    g_mode = RTE_VEHMODE_SAFE;
    g_cls  = RTE_FAULT_NONE;
    StatusLed_Init();
}

void tearDown(void)
{
}

static void test_every_mode_has_a_distinct_pattern(void)
{
    uint32_t a;
    uint32_t b;
    for (a = 0u; a < RTE_VEHMODE_COUNT; a++)
    {
        for (b = a + 1u; b < RTE_VEHMODE_COUNT; b++)
        {
            uint32_t pa = ((uint32_t)StatusLed_GetGreenPattern(a) << 16) | StatusLed_GetRedPattern(a, RTE_FAULT_NONE);
            uint32_t pb = ((uint32_t)StatusLed_GetGreenPattern(b) << 16) | StatusLed_GetRedPattern(b, RTE_FAULT_NONE);
            TEST_ASSERT_NOT_EQUAL(pa, pb);
        }
    }
}

static void test_armed_shows_solid_green_and_estop_solid_red(void)
{
    TEST_ASSERT_EQUAL_HEX16(0xFFFFu, StatusLed_GetGreenPattern(RTE_VEHMODE_ARMED_MANUAL));
    TEST_ASSERT_EQUAL_HEX16(0xFFFFu, StatusLed_GetRedPattern(RTE_VEHMODE_EMERGENCY_STOP, RTE_FAULT_NONE));
    TEST_ASSERT_EQUAL_HEX16(0x0000u, StatusLed_GetGreenPattern(RTE_VEHMODE_EMERGENCY_STOP));
}

static void test_advisory_fault_adds_red_blip(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x0000u, StatusLed_GetRedPattern(RTE_VEHMODE_STANDBY, RTE_FAULT_NONE));
    TEST_ASSERT_NOT_EQUAL(0x0000u, StatusLed_GetRedPattern(RTE_VEHMODE_STANDBY, RTE_FAULT_ADVISORY));
}

static void test_sequencer_follows_pattern_bits(void)
{
    uint32_t step;
    g_mode = RTE_VEHMODE_STANDBY; /* 0xC000: on for two steps, then off */
    for (step = 0u; step < 16u; step++)
    {
        StatusLed_MainFunction();
        TEST_ASSERT_EQUAL(step < 2u, g_green);
        TEST_ASSERT_FALSE(g_red);
    }
}

static void test_unknown_mode_shows_alarm(void)
{
    TEST_ASSERT_EQUAL_HEX16(0xFFFFu, StatusLed_GetRedPattern(RTE_VEHMODE_COUNT, RTE_FAULT_NONE));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_every_mode_has_a_distinct_pattern);
    RUN_TEST(test_armed_shows_solid_green_and_estop_solid_red);
    RUN_TEST(test_advisory_fault_adds_red_blip);
    RUN_TEST(test_sequencer_follows_pattern_bits);
    RUN_TEST(test_unknown_mode_shows_alarm);
    return UNITY_END();
}
