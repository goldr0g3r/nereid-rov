/**
 * @file    Crc.c
 * @brief   Bitwise CRC implementation (small flash footprint; table-driven if profiling demands).
 */
#include "Crc.h"

uint16_t Crc_CalculateCRC16(const uint8_t *data, uint32_t len, uint16_t crc)
{
    uint32_t i;
    uint8_t  bit;

    for (i = 0u; i < len; i++)
    {
        crc ^= (uint16_t)((uint16_t)data[i] << 8);
        for (bit = 0u; bit < 8u; bit++)
        {
            if ((crc & 0x8000u) != 0u)
            {
                crc = (uint16_t)((crc << 1) ^ 0x1021u);
            }
            else
            {
                crc = (uint16_t)(crc << 1);
            }
        }
    }
    return crc;
}
