#include "syscalls_common.h"

// getpid
int _getpid_r(struct _reent *r) {
    _REENT_ERRNO(r) = ENOSYS;
    return -1;
}
