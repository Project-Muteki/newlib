#include "syscalls_common.h"

// execve
int _execve_r(struct _reent *r, const char *name, char * const *argv, char * const *env) {
    _REENT_ERRNO(r) = ENOSYS;
    return -1;
}
