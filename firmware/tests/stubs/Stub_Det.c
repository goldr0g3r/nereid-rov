/* Test stub: Det (counts reported development errors). */
#include "Det.h"

uint32_t Stub_DetErrorCount;

Std_ReturnType Det_ReportError(uint16_t moduleId, uint8_t instanceId, uint8_t apiId, uint8_t errorId)
{
    (void)moduleId;
    (void)instanceId;
    (void)apiId;
    (void)errorId;
    Stub_DetErrorCount++;
    return E_OK;
}
