/* Unit tests: IoHwAb (polarity, debouncing, safe outputs). */
#include "Dio.h"
#include "IoHwAb.h"
#include "unity.h"

extern Dio_LevelType Stub_DioLevels[DIO_CHANNEL_COUNT];
extern uint32_t      Stub_DetErrorCount;

/* Test configuration: fault LED is active-low to exercise polarity handling. */
const IoHwAb_OutputCfgType IoHwAb_OutputCfg[IOHWAB_OUT_COUNT] = {
    [IOHWAB_OUT_STATUS_LED] = {DIO_CH_LED_GREEN, true},
    [IOHWAB_OUT_FAULT_LED]  = {DIO_CH_LED_RED, false},
    [IOHWAB_OUT_LIGHTS]     = {DIO_CH_LED_BLUE, true},
};
const IoHwAb_InputCfgType IoHwAb_InputCfg[IOHWAB_IN_COUNT] = {
    [IOHWAB_IN_USER_BUTTON] = {DIO_CH_USER_BUTTON, true, 3u},
};

static bool Button(void)
{
    bool v = false;
    TEST_ASSERT_EQUAL(E_OK, IoHwAb_GetDigitalInput(IOHWAB_IN_USER_BUTTON, &v));
    return v;
}

void setUp(void)
{
    uint32_t i;
    for (i = 0u; i < DIO_CHANNEL_COUNT; i++)
    {
        Stub_DioLevels[i] = STD_LOW;
    }
    Stub_DetErrorCount = 0u;
    IoHwAb_Init();
}

void tearDown(void)
{
}

static void test_init_drives_outputs_inactive_respecting_polarity(void)
{
    TEST_ASSERT_EQUAL(STD_LOW, Stub_DioLevels[DIO_CH_LED_GREEN]);
    TEST_ASSERT_EQUAL(STD_HIGH, Stub_DioLevels[DIO_CH_LED_RED]); /* active-low -> high = off */
}

static void test_set_output_applies_polarity(void)
{
    IoHwAb_SetDigitalOutput(IOHWAB_OUT_FAULT_LED, true);
    TEST_ASSERT_EQUAL(STD_LOW, Stub_DioLevels[DIO_CH_LED_RED]);
    IoHwAb_SetDigitalOutput(IOHWAB_OUT_STATUS_LED, true);
    TEST_ASSERT_EQUAL(STD_HIGH, Stub_DioLevels[DIO_CH_LED_GREEN]);
}

static void test_input_debounce_needs_consecutive_samples(void)
{
    Stub_DioLevels[DIO_CH_USER_BUTTON] = STD_HIGH;
    IoHwAb_MainFunction();
    IoHwAb_MainFunction();
    TEST_ASSERT_FALSE(Button());
    IoHwAb_MainFunction();
    TEST_ASSERT_TRUE(Button());
}

static void test_input_glitch_is_filtered(void)
{
    Stub_DioLevels[DIO_CH_USER_BUTTON] = STD_HIGH;
    IoHwAb_MainFunction();
    IoHwAb_MainFunction();
    Stub_DioLevels[DIO_CH_USER_BUTTON] = STD_LOW;
    IoHwAb_MainFunction();
    Stub_DioLevels[DIO_CH_USER_BUTTON] = STD_HIGH;
    IoHwAb_MainFunction();
    IoHwAb_MainFunction();
    TEST_ASSERT_FALSE(Button());
}

static void test_invalid_ids_report_det(void)
{
    bool v;
    TEST_ASSERT_EQUAL(E_NOT_OK, IoHwAb_SetDigitalOutput(IOHWAB_OUT_COUNT, true));
    TEST_ASSERT_EQUAL(E_NOT_OK, IoHwAb_GetDigitalInput(IOHWAB_IN_COUNT, &v));
    TEST_ASSERT_EQUAL_UINT32(2u, Stub_DetErrorCount);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_drives_outputs_inactive_respecting_polarity);
    RUN_TEST(test_set_output_applies_polarity);
    RUN_TEST(test_input_debounce_needs_consecutive_samples);
    RUN_TEST(test_input_glitch_is_filtered);
    RUN_TEST(test_invalid_ids_report_det);
    return UNITY_END();
}
