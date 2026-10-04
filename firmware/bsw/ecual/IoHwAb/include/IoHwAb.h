/**
 * @file    IoHwAb.h
 * @brief   ECU abstraction: logical, active-level-independent I/O signals.
 *
 * Upper layers speak about *signals* ("status LED", "user button", later "thruster 3",
 * "leak sensor") and never about pins, polarities or peripherals. The mapping lives in
 * the variant's IoHwAb_Cfg.c.
 */
#ifndef IOHWAB_H
#define IOHWAB_H

#include "IoHwAb_Cfg.h"
#include "Std_Types.h"

typedef struct
{
    uint8_t dioChannel;
    bool    activeHigh;
} IoHwAb_OutputCfgType;

typedef struct
{
    uint8_t dioChannel;
    bool    activeHigh;
    uint8_t debounceSamples; /**< consecutive equal samples needed to change state */
} IoHwAb_InputCfgType;

extern const IoHwAb_OutputCfgType IoHwAb_OutputCfg[IOHWAB_OUT_COUNT];
extern const IoHwAb_InputCfgType  IoHwAb_InputCfg[IOHWAB_IN_COUNT];

void           IoHwAb_Init(void);
/** Sample and debounce inputs. Call periodically (10 ms task). */
void           IoHwAb_MainFunction(void);
Std_ReturnType IoHwAb_SetDigitalOutput(IoHwAb_OutputIdType id, bool active);
/** Debounced logical state of a digital input. */
Std_ReturnType IoHwAb_GetDigitalInput(IoHwAb_InputIdType id, bool *active);
/** Drive every output to its inactive (safe) level. Used at start-up and shutdown. */
void           IoHwAb_SetAllOutputsSafe(void);

#endif /* IOHWAB_H */
