/**
 * @file    BenchPanel.h
 * @brief   BenchPanel SWC: turns the Nucleo user button into operator requests so the
 *          mode logic can be exercised on the bench before a topside link exists.
 *
 *  - hold >= longPressMs        -> EMERGENCY STOP (fires while still held)
 *  - short press, by mode:
 *        STANDBY                -> ARM
 *        ARMED_*                -> DISARM
 *        FAILSAFE / E-STOP      -> RESET
 *        SAFE                   -> (nothing)
 * Replaced/complemented by the topside command source (Com) in milestone M2.
 */
#ifndef BENCHPANEL_H
#define BENCHPANEL_H

#include "Rte_Types.h"

typedef struct
{
    uint32_t periodMs;     /**< runnable period */
    uint32_t minPressMs;   /**< shorter presses are ignored (glitches) */
    uint32_t longPressMs;  /**< e-stop hold time */
} BenchPanel_ConfigType;

void BenchPanel_Init(const BenchPanel_ConfigType *config);
void BenchPanel_MainFunction(void);

#endif /* BENCHPANEL_H */
