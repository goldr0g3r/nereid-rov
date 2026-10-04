/* Test stub: Log (counts lines, optionally echoes them with NEREID_TEST_LOG=1). */
#include "Log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t Stub_LogCount;

void Log_Write(Log_LevelType level, const char *module, const char *fmt, ...)
{
    Stub_LogCount++;
    if (getenv("NEREID_TEST_LOG") != NULL)
    {
        va_list args;
        va_start(args, fmt);
        (void)printf("  log[%d] %s: ", (int)level, module);
        (void)vprintf(fmt, args);
        (void)printf("\n");
        va_end(args);
    }
}

void Log_Flush(void) {}
