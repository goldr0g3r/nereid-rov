/**
 * @file    main_host.c
 * @brief   Host (Linux) entry point: runs the complete ECU software on the FreeRTOS
 *          POSIX port with simulated I/O. Used for smoke tests now and SIL later.
 *
 * Usage:
 *   nereid_host                     interactive: type stimulus tokens + Enter
 *   nereid_host --script "<tokens>" run a scripted scenario
 *
 * Stimulus tokens (whitespace separated):
 *   w<ms>   wait            s   short button press (150 ms)
 *   l       long press (1.5 s, emergency stop)       p<ms> press for <ms>
 *   q       quit (exit code 0)
 */
#include "Dio.h"
#include "EcuM.h"
#include "HostSim.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *Host_Script;

static void Host_SleepMs(unsigned long ms)
{
    struct timespec ts;
    ts.tv_sec  = (time_t)(ms / 1000u);
    ts.tv_nsec = (long)((ms % 1000u) * 1000000u);
    (void)nanosleep(&ts, NULL);
}

static void Host_Press(unsigned long ms)
{
    HostSim_SetDioLevel(DIO_CH_USER_BUTTON, STD_HIGH);
    Host_SleepMs(ms);
    HostSim_SetDioLevel(DIO_CH_USER_BUTTON, STD_LOW);
}

static void Host_RunToken(const char *tok)
{
    switch (tok[0])
    {
    case 'w':
        Host_SleepMs(strtoul(&tok[1], NULL, 10));
        break;
    case 's':
        (void)printf("[host] stimulus: short press\n");
        Host_Press(150u);
        break;
    case 'l':
        (void)printf("[host] stimulus: long press\n");
        Host_Press(1500u);
        break;
    case 'p':
        Host_Press(strtoul(&tok[1], NULL, 10));
        break;
    case 'q':
        Host_SleepMs(200u); /* let the logger drain */
        HostSim_Exit(0, "quit");
    default:
        (void)printf("[host] unknown token '%s'\n", tok);
        break;
    }
}

static void Host_RunLine(char *line)
{
    char *save = NULL;
    char *tok  = strtok_r(line, " \t\r\n", &save);

    while (tok != NULL)
    {
        Host_RunToken(tok);
        tok = strtok_r(NULL, " \t\r\n", &save);
    }
}

static void *Host_StimulusThread(void *arg)
{
    char line[512];
    (void)arg;

    if (Host_Script != NULL)
    {
        (void)snprintf(line, sizeof(line), "%s", Host_Script);
        Host_RunLine(line);
        return NULL;
    }
    while (fgets(line, (int)sizeof(line), stdin) != NULL)
    {
        Host_RunLine(line);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t thread;
    sigset_t  all;
    sigset_t  old;

    if ((argc == 3) && (strcmp(argv[1], "--script") == 0))
    {
        Host_Script = argv[2];
    }
    else if (argc != 1)
    {
        (void)fprintf(stderr, "usage: %s [--script \"w500 s w300 l q\"]\n", argv[0]);
        return 2;
    }
    (void)setvbuf(stdout, NULL, _IOLBF, 0);

    (void)sigfillset(&all);
    (void)pthread_sigmask(SIG_SETMASK, &all, &old);
    (void)pthread_create(&thread, NULL, Host_StimulusThread, NULL);
    (void)pthread_sigmask(SIG_SETMASK, &old, NULL);
    (void)pthread_detach(thread);

    EcuM_Init(); /* does not return */
    return 1;
}
