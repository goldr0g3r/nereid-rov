/**
 * @file    Crc.h
 * @brief   CRC routines used by the communication stack and NvM.
 */
#ifndef CRC_H
#define CRC_H

#include "Std_Types.h"

#define CRC16_CCITT_INITIAL 0xFFFFu

/**
 * CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF, no reflection, no final xor).
 * Check value for "123456789" is 0x29B1. Pass the previous result as @p crc to
 * process data in chunks.
 */
uint16_t Crc_CalculateCRC16(const uint8_t *data, uint32_t len, uint16_t crc);

#endif /* CRC_H */
