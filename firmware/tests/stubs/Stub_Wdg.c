/* Test stub: Wdg (records init timeout and trigger count). */
#include "Wdg.h"

uint32_t Stub_WdgTimeoutMs;
uint32_t Stub_WdgTriggerCount;

void Wdg_Init(uint32_t timeoutMs) { Stub_WdgTimeoutMs = timeoutMs; }
void Wdg_Trigger(void) { Stub_WdgTriggerCount++; }
