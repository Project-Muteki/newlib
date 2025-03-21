#include "syscalls_common.h"

// times
clock_t _times_r(struct _reent *r, struct tms *buf) {
    _REENT_ERRNO(r) = ENOSYS;
    return 0;
}
