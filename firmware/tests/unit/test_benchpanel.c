/* Unit tests: BenchPanel SWC (button press classification). */
#include "BenchPanel.h"
#include "Rte_BenchPanel.h"
#include "unity.h"

static bool                    g_pressed;
static Rte_VehicleModeType     g_mode;
static Rte_OperatorRequestType g_sent[8];
static uint32_t                g_sentCount;

Std_ReturnType Rte_Read_BenchPanel_UserButton(bool *pressed)
{
    *pressed = g_pressed;
    return RTE_E_OK;
}

Std_ReturnType Rte_Read_BenchPanel_Mode(Rte_VehicleModeType *mode)
{
    *mode = g_mode;
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_BenchPanel_OperatorRequest(Rte_OperatorRequestType req)
{
    if (g_sentCount < 8u)
    {
        g_sent[g_sentCount++] = req;
    }
    return RTE_E_OK;
}

static const BenchPanel_ConfigType g_cfg = {.periodMs = 10u, .minPressMs = 30u, .longPressMs = 1000u};

static void Hold(uint32_t ms)
{
    uint32_t t;
    g_pressed = true;
    for (t = 0u; t <= ms; t += 10u)
    {
        BenchPanel_MainFunction();
    }
    g_pressed = false;
    BenchPanel_MainFunction();
}

void setUp(void)
{
    g_pressed   = false;
    g_mode      = RTE_VEHMODE_STANDBY;
    g_sentCount = 0u;
    BenchPanel_Init(&g_cfg);
}

void tearDown(void)
{
}

static void test_short_press_maps_by_mode(void)
{
    static const struct
    {
        Rte_VehicleModeType     mode;
        Rte_OperatorRequestType req;
    } map[] = {
        {RTE_VEHMODE_STANDBY, RTE_OPREQ_ARM},          {RTE_VEHMODE_ARMED_MANUAL, RTE_OPREQ_DISARM},
        {RTE_VEHMODE_ARMED_ASSISTED, RTE_OPREQ_DISARM}, {RTE_VEHMODE_ARMED_DEGRADED, RTE_OPREQ_DISARM},
        {RTE_VEHMODE_FAILSAFE, RTE_OPREQ_RESET},        {RTE_VEHMODE_EMERGENCY_STOP, RTE_OPREQ_RESET},
    };
    uint32_t i;

    for (i = 0u; i < (sizeof(map) / sizeof(map[0])); i++)
    {
        setUp();
        g_mode = map[i].mode;
        Hold(150u);
        TEST_ASSERT_EQUAL_UINT32(1u, g_sentCount);
        TEST_ASSERT_EQUAL(map[i].req, g_sent[0]);
    }
}

static void test_short_press_in_safe_sends_nothing(void)
{
    g_mode = RTE_VEHMODE_SAFE;
    Hold(150u);
    TEST_ASSERT_EQUAL_UINT32(0u, g_sentCount);
}

static void test_glitch_is_ignored(void)
{
    Hold(10u);
    TEST_ASSERT_EQUAL_UINT32(0u, g_sentCount);
}

static void test_long_press_fires_estop_while_held_and_only_once(void)
{
    uint32_t t;
    g_pressed = true;
    for (t = 0u; t < 990u; t += 10u)
    {
        BenchPanel_MainFunction();
    }
    TEST_ASSERT_EQUAL_UINT32(0u, g_sentCount);
    for (; t < 3000u; t += 10u)
    {
        BenchPanel_MainFunction();
    }
    TEST_ASSERT_EQUAL_UINT32(1u, g_sentCount);
    TEST_ASSERT_EQUAL(RTE_OPREQ_ESTOP, g_sent[0]);
    g_pressed = false;
    BenchPanel_MainFunction();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1u, g_sentCount, "release after long press must not send a short press");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_short_press_maps_by_mode);
    RUN_TEST(test_short_press_in_safe_sends_nothing);
    RUN_TEST(test_glitch_is_ignored);
    RUN_TEST(test_long_press_fires_estop_while_held_and_only_once);
    return UNITY_END();
}
