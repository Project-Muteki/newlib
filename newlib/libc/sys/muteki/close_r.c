#include "syscalls_common.h"

// close
int _close_r(struct _reent *r, int fd) {
    if (__muteki_release_descriptor(fd) < 0) {
        _REENT_ERRNO(r) = EBADF;
        return -1;
    }
    return 0;
}
