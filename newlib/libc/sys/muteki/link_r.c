#include "syscalls_common.h"

// link
int _link_r(struct _reent *r, const char *old, const char *new) {
    _REENT_ERRNO(r) = ENOSYS;
    return -1;
}
