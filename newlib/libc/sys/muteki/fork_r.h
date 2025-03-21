#include "syscalls_common.h"

// fork
int _fork_r(struct _reent *r) {
    _REENT_ERRNO(r) = ENOSYS;
    return -1;
}
