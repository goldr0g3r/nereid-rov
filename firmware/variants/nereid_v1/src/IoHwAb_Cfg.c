/**
 * @file    IoHwAb_Cfg.c
 * @brief   Variant nereid_v1: logical signal -> Dio channel, polarity, debounce.
 *          On the bench the vehicle "lights" are the blue Nucleo LED.
 */
#include "Dio.h"
#include "IoHwAb.h"

const IoHwAb_OutputCfgType IoHwAb_OutputCfg[IOHWAB_OUT_COUNT] = {
    [IOHWAB_OUT_STATUS_LED] = {.dioChannel = DIO_CH_LED_GREEN, .activeHigh = true},
    [IOHWAB_OUT_FAULT_LED]  = {.dioChannel = DIO_CH_LED_RED, .activeHigh = true},
    [IOHWAB_OUT_LIGHTS]     = {.dioChannel = DIO_CH_LED_BLUE, .activeHigh = true},
};

const IoHwAb_InputCfgType IoHwAb_InputCfg[IOHWAB_IN_COUNT] = {
    /* B1 drives PC13 high when pressed; 3 samples x 10 ms = 30 ms debounce */
    [IOHWAB_IN_USER_BUTTON] = {.dioChannel = DIO_CH_USER_BUTTON, .activeHigh = true, .debounceSamples = 3u},
};
