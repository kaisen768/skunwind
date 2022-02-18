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
    int logfile;
    char buf[128];

    uint64_t pc = _Unwind_GetIP(context);
    Dl_info info;

    if (pc) {
        dladdr((const void*)pc, &info);

	    fprintf(stderr, "file: %-42s \tfunc: %-32s \tOffset Addr: 0x%p\n", info.dli_fname, info.dli_sname, info.dli_saddr);
        
        if (arg) {
            logfile = *((int *)arg);
            sprintf(buf, "file: %-42s \tfunc: %-32s \tOffset Addr: 0x%p\n", info.dli_fname, info.dli_sname, info.dli_saddr);
            write(logfile, buf, strlen(buf));
        }
    }

    return _URC_NO_REASON;
}

static void signal_exit_handler(int sig)
{
    static int logfile;
    void *exta;

    if (filename) {
        logfile = open(filename, O_RDWR|O_CREAT|O_APPEND, 0666);
        exta = &logfile;
    } else {
        exta = NULL;
    }

    _Unwind_Backtrace(unwind_backtrace_callback, exta);
    exit(1);
}

int skunwind_register(const char *lpathname)
{
    filename = (char*)lpathname;

    signal(SIGSEGV, signal_exit_handler);

    return 0;
}
