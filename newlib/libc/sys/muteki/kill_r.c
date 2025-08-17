#include "syscalls_common.h"

// kill
int _kill_r(struct _reent *r, int pid, int sig) {
    _REENT_ERRNO(r) = ENOSYS;
    return -1;
}
