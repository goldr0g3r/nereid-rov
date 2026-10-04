/* Test stub: Os (single-threaded tests, no critical sections needed). */
#include "Os.h"

uint32_t Stub_OsTimeMs;

void Os_EnterCritical(void) {}
void Os_ExitCritical(void) {}
uint32_t Os_GetTimeMs(void) { return Stub_OsTimeMs; }
bool Os_IsRunning(void) { return true; }
