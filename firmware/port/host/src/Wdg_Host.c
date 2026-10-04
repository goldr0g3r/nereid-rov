/**
 * @file    Wdg_Host.c
 * @brief   Host MCAL: watchdog emulated by a monitor thread. Expiry ends the simulation
 *          with exit code 3, just like the IWDG would reset the real MCU.
 */
#include "Gpt.h"
#include "HostSim.h"
#include "Wdg.h"

#include <pthread.h>
#include <signal.h>
#include <time.h>

static volatile uint32_t Wdg_LastTriggerUs;
static uint32_t          Wdg_TimeoutUs;

static void *Wdg_Monitor(void *arg)
{
    const struct timespec period = {0, 10 * 1000 * 1000};
    (void)arg;

    for (;;)
    {
        (void)nanosleep(&period, NULL);
        if (Gpt_ElapsedUs(Wdg_LastTriggerUs, Gpt_GetTimeUs()) > Wdg_TimeoutUs)
        {
            HostSim_Exit(3, "WATCHDOG RESET (independent watchdog expired)");
        }
    }
    return NULL;
}

void Wdg_Init(uint32_t timeoutMs)
{
    pthread_t thread;
    sigset_t  all;
    sigset_t  old;

    Wdg_TimeoutUs     = timeoutMs * 1000u;
    Wdg_LastTriggerUs = Gpt_GetTimeUs();

    /* Keep FreeRTOS' tick signal away from this non-FreeRTOS thread. */
    (void)sigfillset(&all);
    (void)pthread_sigmask(SIG_SETMASK, &all, &old);
    (void)pthread_create(&thread, NULL, Wdg_Monitor, NULL);
    (void)pthread_sigmask(SIG_SETMASK, &old, NULL);
    (void)pthread_detach(thread);
}

void Wdg_Trigger(void)
{
    Wdg_LastTriggerUs = Gpt_GetTimeUs();
}
