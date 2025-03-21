#include "syscalls_common.h"

// gettimeofday
int _gettimeofday_r(struct _reent *r, struct timeval *tp, void *tzp) {
    struct tm dt_unix;
    datetime_t dt_besta;

    if (tp == NULL) {
        _REENT_ERRNO(r) = EFAULT;
        return -1;
    }

    GetSysTime(&dt_besta);

    dt_unix.tm_year = dt_besta.year - 1900;
    dt_unix.tm_mon = dt_besta.month - 1;
    dt_unix.tm_mday = dt_besta.day;
    dt_unix.tm_hour = dt_besta.hour;
    dt_unix.tm_min = dt_besta.minute;
    dt_unix.tm_sec = dt_besta.second;
    dt_unix.tm_isdst = -1;

    tp->tv_sec = mktime(&dt_unix);
    tp->tv_usec = dt_besta.millis * 1000;

    return 0;
}
