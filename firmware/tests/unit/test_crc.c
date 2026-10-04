/* Unit tests: lib/Crc. */
#include "Crc.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

static void test_crc16_ccitt_false_check_value(void)
{
    const uint8_t data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    TEST_ASSERT_EQUAL_HEX16(0x29B1u, Crc_CalculateCRC16(data, sizeof(data), CRC16_CCITT_INITIAL));
}

static void test_crc16_chunked_equals_whole(void)
{
    const uint8_t data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint16_t      crc    = Crc_CalculateCRC16(data, 4u, CRC16_CCITT_INITIAL);
    crc                  = Crc_CalculateCRC16(&data[4], 5u, crc);
    TEST_ASSERT_EQUAL_HEX16(0x29B1u, crc);
}

static void test_crc16_empty_returns_initial(void)
{
    TEST_ASSERT_EQUAL_HEX16(0xFFFFu, Crc_CalculateCRC16(NULL, 0u, CRC16_CCITT_INITIAL));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_crc16_ccitt_false_check_value);
    RUN_TEST(test_crc16_chunked_equals_whole);
    RUN_TEST(test_crc16_empty_returns_initial);
    return UNITY_END();
}
