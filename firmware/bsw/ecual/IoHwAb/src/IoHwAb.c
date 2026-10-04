/**
 * @file    IoHwAb.c
 * @brief   I/O hardware abstraction: polarity handling and input debouncing.
 */
#include "IoHwAb.h"

#include "Det.h"
#include "Dio.h"
#include "ModuleIds.h"

#define IOHWAB_API_SET_OUTPUT 1u
#define IOHWAB_API_GET_INPUT  2u
#define IOHWAB_E_PARAM        1u

typedef struct
{
    bool    stable;
    bool    lastSample;
    uint8_t sameCount;
} IoHwAb_InputStateType;

static IoHwAb_InputStateType IoHwAb_InputState[IOHWAB_IN_COUNT];

static bool IoHwAb_SampleInput(const IoHwAb_InputCfgType *cfg)
{
    Dio_LevelType level = Dio_ReadChannel(cfg->dioChannel);
    return cfg->activeHigh ? (level == STD_HIGH) : (level == STD_LOW);
}

void IoHwAb_Init(void)
{
    uint32_t i;

    IoHwAb_SetAllOutputsSafe();
    for (i = 0u; i < (uint32_t)IOHWAB_IN_COUNT; i++)
    {
        bool sample                       = IoHwAb_SampleInput(&IoHwAb_InputCfg[i]);
        IoHwAb_InputState[i].stable     = sample;
        IoHwAb_InputState[i].lastSample = sample;
        IoHwAb_InputState[i].sameCount  = 0u;
    }
}

void IoHwAb_MainFunction(void)
{
    uint32_t i;

    for (i = 0u; i < (uint32_t)IOHWAB_IN_COUNT; i++)
    {
        IoHwAb_InputStateType *st     = &IoHwAb_InputState[i];
        bool                   sample = IoHwAb_SampleInput(&IoHwAb_InputCfg[i]);

        if (sample != st->lastSample)
        {
            st->lastSample = sample;
            st->sameCount  = 1u;
        }
        else if (st->sameCount < 0xFFu)
        {
            st->sameCount++;
        }

        if ((sample != st->stable) && (st->sameCount >= IoHwAb_InputCfg[i].debounceSamples))
        {
            st->stable = sample;
        }
    }
}

Std_ReturnType IoHwAb_SetDigitalOutput(IoHwAb_OutputIdType id, bool active)
{
    const IoHwAb_OutputCfgType *cfg;

    if ((uint32_t)id >= (uint32_t)IOHWAB_OUT_COUNT)
    {
        Det_ReportError(MODULE_ID_IOHWAB, 0u, IOHWAB_API_SET_OUTPUT, IOHWAB_E_PARAM);
        return E_NOT_OK;
    }
    cfg = &IoHwAb_OutputCfg[id];
    Dio_WriteChannel(cfg->dioChannel, (active == cfg->activeHigh) ? STD_HIGH : STD_LOW);
    return E_OK;
}

Std_ReturnType IoHwAb_GetDigitalInput(IoHwAb_InputIdType id, bool *active)
{
    if (((uint32_t)id >= (uint32_t)IOHWAB_IN_COUNT) || (active == NULL))
    {
        Det_ReportError(MODULE_ID_IOHWAB, 0u, IOHWAB_API_GET_INPUT, IOHWAB_E_PARAM);
        return E_NOT_OK;
    }
    *active = IoHwAb_InputState[id].stable;
    return E_OK;
}

void IoHwAb_SetAllOutputsSafe(void)
{
    uint32_t i;

    for (i = 0u; i < (uint32_t)IOHWAB_OUT_COUNT; i++)
    {
        (void)IoHwAb_SetDigitalOutput((IoHwAb_OutputIdType)i, false);
    }
}
