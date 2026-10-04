/**
 * @file    Lights.h
 * @brief   Lights SWC: OFF/ON state machine for the vehicle lights (formalises the
 *          "Hand Controller Unit - Lights" sketch). Dimming levels become sub-states of
 *          ON once a PWM output exists (milestone M3).
 */
#ifndef LIGHTS_H
#define LIGHTS_H

#include "Rte_Types.h"

void Lights_Init(void);
/** Runnable, 10 ms: consume queued light requests, drive the output. */
void Lights_MainFunction(void);
bool Lights_IsOn(void);

#endif /* LIGHTS_H */
