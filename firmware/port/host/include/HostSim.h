/**
 * @file    HostSim.h
 * @brief   Host port simulation hooks (used only by port/host code and the host main).
 */
#ifndef HOSTSIM_H
#define HOSTSIM_H

#include "Dio.h"

/** Drive a simulated input pin (thread-safe for a single writer). */
void HostSim_SetDioLevel(Dio_ChannelType channel, Dio_LevelType level);

/** Terminate the simulation (flushes stdout). */
void HostSim_Exit(int code, const char *reason) __attribute__((noreturn));

#endif /* HOSTSIM_H */
