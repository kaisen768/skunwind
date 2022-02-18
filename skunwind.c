#include <stdio.h>
#include <string.h>
#include <unwind.h>
#include <inttypes.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>
#include <signal.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "skunwind.h"

static char *filename = NULL;

static _Unwind_Reason_Code unwind_backtrace_callback(struct _Unwind_Context *context, void *arg)
{

    int loginfo;
    char buffer[100];

    uint64_t pc = _Unwind_GetIP(context);
    Dl_info info;

    if (pc) {
        dladdr((const void*)pc, &info);

	    fprintf(stderr, "fname: %s, sname: %s, off: 0x%p\n", info.dli_fname, info.dli_sname, info.dli_saddr);
        
        if (arg) {
            loginfo = *((int *)arg);
            sprintf(buffer, "fname: %s, sname: %s, off: 0x%p\n", info.dli_fname, info.dli_sname, info.dli_saddr);
            write(loginfo, buffer, strlen(buffer));
        }
    }

    return _URC_NO_REASON;
}

static void signal_exit_handler(int sig)
{
    int loginfo;

    if (filename) {
        loginfo = open(filename, O_RDWR|O_CREAT|O_APPEND, 0666);
        _Unwind_Backtrace(unwind_backtrace_callback, (void *)&loginfo);
    } else {
        _Unwind_Backtrace(unwind_backtrace_callback, NULL);
    }

    exit(1);
}

int skunwind_register(const char *lpathname)
{
    filename = (char*)lpathname;

    signal(SIGSEGV, signal_exit_handler);

    return 0;
}
