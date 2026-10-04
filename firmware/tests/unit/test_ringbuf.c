/* Unit tests: lib/RingBuf. */
#include "RingBuf.h"
#include "unity.h"

static uint8_t      g_storage[8];
static RingBuf_Type g_rb;

void setUp(void)
{
    TEST_ASSERT_EQUAL(E_OK, RingBuf_Init(&g_rb, g_storage, sizeof(g_storage)));
}

void tearDown(void)
{
}

static void test_empty_after_init(void)
{
    uint8_t b;
    TEST_ASSERT_EQUAL_UINT32(0u, RingBuf_Count(&g_rb));
    TEST_ASSERT_EQUAL_UINT32(7u, RingBuf_Free(&g_rb));
    TEST_ASSERT_EQUAL(E_NOT_OK, RingBuf_Get(&g_rb, &b));
}

static void test_fifo_order(void)
{
    uint8_t b;
    RingBuf_Put(&g_rb, 1u);
    RingBuf_Put(&g_rb, 2u);
    RingBuf_Get(&g_rb, &b);
    TEST_ASSERT_EQUAL_UINT8(1u, b);
    RingBuf_Get(&g_rb, &b);
    TEST_ASSERT_EQUAL_UINT8(2u, b);
}

static void test_capacity_is_size_minus_one(void)
{
    uint32_t i;
    for (i = 0u; i < 7u; i++)
    {
        TEST_ASSERT_EQUAL(E_OK, RingBuf_Put(&g_rb, (uint8_t)i));
    }
    TEST_ASSERT_EQUAL(E_NOT_OK, RingBuf_Put(&g_rb, 99u));
    TEST_ASSERT_EQUAL_UINT32(7u, RingBuf_Count(&g_rb));
    TEST_ASSERT_EQUAL_UINT32(0u, RingBuf_Free(&g_rb));
}

static void test_wraparound_bulk_read_write(void)
{
    const uint8_t in[5] = {10, 11, 12, 13, 14};
    uint8_t       out[5];
    uint32_t      round;

    for (round = 0u; round < 10u; round++)
    {
        TEST_ASSERT_EQUAL_UINT32(5u, RingBuf_Write(&g_rb, in, 5u));
        TEST_ASSERT_EQUAL_UINT32(5u, RingBuf_Read(&g_rb, out, 5u));
        TEST_ASSERT_EQUAL_UINT8_ARRAY(in, out, 5u);
    }
}

static void test_partial_write_when_nearly_full(void)
{
    const uint8_t in[10] = {0};
    TEST_ASSERT_EQUAL_UINT32(7u, RingBuf_Write(&g_rb, in, 10u));
}

static void test_clear(void)
{
    RingBuf_Put(&g_rb, 1u);
    RingBuf_Clear(&g_rb);
    TEST_ASSERT_EQUAL_UINT32(0u, RingBuf_Count(&g_rb));
}

static void test_init_rejects_bad_args(void)
{
    TEST_ASSERT_EQUAL(E_NOT_OK, RingBuf_Init(&g_rb, g_storage, 1u));
    TEST_ASSERT_EQUAL(E_NOT_OK, RingBuf_Init(&g_rb, NULL, 8u));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_empty_after_init);
    RUN_TEST(test_fifo_order);
    RUN_TEST(test_capacity_is_size_minus_one);
    RUN_TEST(test_wraparound_bulk_read_write);
    RUN_TEST(test_partial_write_when_nearly_full);
    RUN_TEST(test_clear);
    RUN_TEST(test_init_rejects_bad_args);
    return UNITY_END();
}
