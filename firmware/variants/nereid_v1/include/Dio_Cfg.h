/**
 * @file    Dio_Cfg.h
 * @brief   Variant nereid_v1: logical Dio channels. Physical pins per MCU port are in
 *          stm32f412/Dio_PBcfg.c (see docs/hardware/ecu_resource_allocation.md).
 */
#ifndef DIO_CFG_H
#define DIO_CFG_H

enum
{
    DIO_CH_LED_GREEN = 0, /* Nucleo LD1 (PB0)  */
    DIO_CH_LED_BLUE,      /* Nucleo LD2 (PB7)  */
    DIO_CH_LED_RED,       /* Nucleo LD3 (PB14) */
    DIO_CH_USER_BUTTON,   /* Nucleo B1  (PC13) */
    DIO_CHANNEL_COUNT
};

#endif /* DIO_CFG_H */
