#include "syscalls_common.h"

// isatty
int _isatty_r(struct _reent *r, int fd) {
    // std* is definitely TTY.
    // TODO: un-hardcode this after custom std* is possible
    if (fd < 3) {
        return 1;
    }

    if (!is_fd_valid(fd)) {
        _REENT_ERRNO(r) = EBADF;
        return 0;
    }

    _REENT_ERRNO(r) = ENOTTY;
    return 0;
}
