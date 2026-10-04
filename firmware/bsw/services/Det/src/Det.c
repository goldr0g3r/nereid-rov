/**
 * @file    Det.c
 * @brief   Development Error Tracer.
 */
#include "Det.h"

#include "Dem.h"
#include "Log.h"

static volatile uint32_t Det_ErrorCount;

void Det_Init(void)
{
    Det_ErrorCount = 0u;
}

Std_ReturnType Det_ReportError(uint16_t moduleId, uint8_t instanceId, uint8_t apiId, uint8_t errorId)
{
    Det_ErrorCount++;
    LOG_ERROR("Det", "module=%u inst=%u api=%u err=%u", (unsigned)moduleId, (unsigned)instanceId,
              (unsigned)apiId, (unsigned)errorId);
    (void)Dem_SetEventStatus(DEM_EVENT_DEV_ERROR, DEM_EVENT_STATUS_FAILED);
    return E_OK;
}

uint32_t Det_GetErrorCount(void)
{
    return Det_ErrorCount;
}
