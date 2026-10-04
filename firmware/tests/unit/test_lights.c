/* Unit tests: Lights SWC state machine. */
#include "Lights.h"
#include "Rte_Lights.h"
#include "unity.h"

static Rte_LightsRequestType g_queue[8];
static uint32_t              g_head;
static uint32_t              g_tail;
static bool                  g_output;
static uint32_t              g_writes;

Std_ReturnType Rte_Read_Lights_Request(Rte_LightsRequestType *req)
{
    if (g_head == g_tail)
    {
        return RTE_E_NO_DATA;
    }
    *req = g_queue[g_tail++ % 8u];
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_Lights_Output(bool on)
{
    g_output = on;
    g_writes++;
    return RTE_E_OK;
}

Std_ReturnType Rte_Send_LightsRequest(Rte_LightsRequestType req)
{
    g_queue[g_head++ % 8u] = req;
    return RTE_E_OK;
}

void setUp(void)
{
    g_head = g_tail = 0u;
    g_output        = true;
    g_writes        = 0u;
    Lights_Init();
}

void tearDown(void)
{
}

static void test_starts_off_and_drives_output_off(void)
{
    TEST_ASSERT_FALSE(Lights_IsOn());
    TEST_ASSERT_FALSE(g_output);
}

static void test_toggle_twice(void)
{
    Rte_Send_LightsRequest(RTE_LIGHTS_REQ_TOGGLE);
    Lights_MainFunction();
    TEST_ASSERT_TRUE(g_output);
    Rte_Send_LightsRequest(RTE_LIGHTS_REQ_TOGGLE);
    Lights_MainFunction();
    TEST_ASSERT_FALSE(g_output);
}

static void test_explicit_on_off_are_idempotent(void)
{
    Rte_Send_LightsRequest(RTE_LIGHTS_REQ_ON);
    Rte_Send_LightsRequest(RTE_LIGHTS_REQ_ON);
    Lights_MainFunction();
    TEST_ASSERT_TRUE(Lights_IsOn());
    TEST_ASSERT_EQUAL_UINT32(2u, g_writes); /* init OFF + one ON, second ON ignored */
    Rte_Send_LightsRequest(RTE_LIGHTS_REQ_OFF);
    Lights_MainFunction();
    TEST_ASSERT_FALSE(Lights_IsOn());
}

static void test_no_request_no_change(void)
{
    Lights_MainFunction();
    TEST_ASSERT_EQUAL_UINT32(1u, g_writes);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_starts_off_and_drives_output_off);
    RUN_TEST(test_toggle_twice);
    RUN_TEST(test_explicit_on_off_are_idempotent);
    RUN_TEST(test_no_request_no_change);
    return UNITY_END();
}
