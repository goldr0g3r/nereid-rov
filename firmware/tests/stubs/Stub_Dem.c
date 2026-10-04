/* Test stub: Dem (records the last reported status per event). */
#include "Dem.h"

Dem_EventStatusType Stub_DemLastStatus[DEM_EVENT_COUNT];
uint32_t            Stub_DemReportCount[DEM_EVENT_COUNT];

Std_ReturnType Dem_SetEventStatus(Dem_EventIdType id, Dem_EventStatusType status)
{
    if (id >= DEM_EVENT_COUNT)
    {
        return E_NOT_OK;
    }
    Stub_DemLastStatus[id] = status;
    Stub_DemReportCount[id]++;
    return E_OK;
}
