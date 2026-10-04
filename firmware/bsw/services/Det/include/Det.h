/**
 * @file    Det.h
 * @brief   Development Error Tracer: reports programming errors (bad parameters,
 *          calls before init). Cross-cutting: every BSW layer may call it.
 *
 * Errors are counted, logged and mirrored into the Dem event DEM_EVENT_DEV_ERROR so
 * they are visible in telemetry. Det never changes control flow by itself.
 */
#ifndef DET_H
#define DET_H

#include "Std_Types.h"

void           Det_Init(void);
Std_ReturnType Det_ReportError(uint16_t moduleId, uint8_t instanceId, uint8_t apiId, uint8_t errorId);
uint32_t       Det_GetErrorCount(void);

#endif /* DET_H */
