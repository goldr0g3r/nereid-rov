/**
 * @file    IoHwAb_Cfg.h
 * @brief   Variant nereid_v1: logical I/O signals seen by the RTE.
 */
#ifndef IOHWAB_CFG_H
#define IOHWAB_CFG_H

typedef enum
{
    IOHWAB_OUT_STATUS_LED = 0, /* mode indication            */
    IOHWAB_OUT_FAULT_LED,      /* fault indication           */
    IOHWAB_OUT_LIGHTS,         /* vehicle lights (bench: LED) */
    IOHWAB_OUT_COUNT
} IoHwAb_OutputIdType;

typedef enum
{
    IOHWAB_IN_USER_BUTTON = 0,
    IOHWAB_IN_COUNT
} IoHwAb_InputIdType;

#endif /* IOHWAB_CFG_H */
