#include "syscalls_common.h"

// sbrk
void *_sbrk_r(struct _reent *r, int incr) {
    // Does not make sense for Besta. Stub it.
    _REENT_ERRNO(r) = ENOSYS;
    return NULL;
}
