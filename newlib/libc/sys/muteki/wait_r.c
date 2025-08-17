#include "syscalls_common.h"

// wait
int _wait_r(struct _reent *r, int *status) {
    _REENT_ERRNO(r) = ENOSYS;
    return -1;
}
