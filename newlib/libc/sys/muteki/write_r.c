#include "syscalls_common.h"

// write
_ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t len) {
    DescriptorTranslation *dt = __muteki_fd_grab(fd);
    if (dt == NULL) {
        _REENT_ERRNO(r) = EBADF;
        return -1;
    }

    switch (dt->type) {
    case MUTEKI_DESCRIPTOR_DEVNULL: {
        __muteki_fd_drop(dt);
        return len;
    }
    case MUTEKI_DESCRIPTOR_FILE: {
        size_t actual = _fwrite(buf, 1, len, dt->file);
        if (actual == 0) {
            int errno_converted = __muteki_kerrno_to_errno(_GetLastError());
            __muteki_fd_drop(dt);
            if (errno_converted != 0) {
                _REENT_ERRNO(r) = errno_converted;
                return -1;
            }
            return 0;
        }
        __muteki_fd_drop(dt);
        return (_ssize_t) (actual & 0x7fffffff);
    }
    case MUTEKI_DESCRIPTOR_DIRECTORY: {
        __muteki_fd_drop(dt);
        _REENT_ERRNO(r) = EISDIR;
        return -1;
    }
    case MUTEKI_DESCRIPTOR_DEBUG: {
        for (size_t i = 0; i < len; i++) {
            WriteComDebugMsg("%c", ((const char *) buf)[i]);
        }
        return len;
    }
    case MUTEKI_DESCRIPTOR_CHARDEV: // TODO
    default: {
        __muteki_fd_drop(dt);
        _REENT_ERRNO(r) = ENOSYS;
        return -1;
    }
    }
}

