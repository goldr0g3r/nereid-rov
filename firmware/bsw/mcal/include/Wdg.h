/**
 * @file    Wdg.h
 * @brief   MCAL: independent hardware watchdog. Once started it cannot be stopped.
 *          Only WdgM may call Wdg_Trigger().
 */
#ifndef WDG_H
#define WDG_H

#include "Std_Types.h"

void Wdg_Init(uint32_t timeoutMs);
void Wdg_Trigger(void);

#endif /* WDG_H */
