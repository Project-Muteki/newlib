#include "syscalls_common.h"

static int whence_to_besta(int whence, int *err) {
    switch (whence) {
    case SEEK_SET:
        return BXC_SEEK_SET;
    case SEEK_CUR:
        return BXC_SEEK_CUR;
    case SEEK_END:
        return BXC_SEEK_END;
    default:
        if (err != NULL) {
            *err = EINVAL;
        }
        return 0;
    }
}

// lseek
_off_t _lseek_r(struct _reent *r, int fd, _off_t offset, int whence) {
    DescriptorTranslation *dt = __muteki_fd_grab(fd);
    if (dt == NULL) {
        _REENT_ERRNO(r) = EBADF;
        return -1;
    }

    switch (dt->type) {
    case MUTEKI_DESCRIPTOR_DEVNULL:
    case MUTEKI_DESCRIPTOR_DEBUG: {
        __muteki_fd_drop(dt);
        return 0;
    }
    case MUTEKI_DESCRIPTOR_FILE: {
        // TODO check off_t sizes
        int whence_err = 0;
        int sys_whence = whence_to_besta(whence, &whence_err);
        if (whence_err != 0) {
            _REENT_ERRNO(r) = whence_err;
            __muteki_fd_drop(dt);
            return -1;
        }
        int result = __fseek(dt->file, offset, sys_whence);
        if (result < 0) {
            _REENT_ERRNO(r) = __muteki_kerrno_to_errno(_GetLastError());
            __muteki_fd_drop(dt);
            return -1;
        }
        _off_t current_pos = _ftell(dt->file);
        __muteki_fd_drop(dt);
        return current_pos;
    }
    case MUTEKI_DESCRIPTOR_DIRECTORY: {
        __muteki_fd_drop(dt);
        _REENT_ERRNO(r) = EISDIR;
        return -1;
    }
    case MUTEKI_DESCRIPTOR_CHARDEV: // TODO
    default: {
        __muteki_fd_drop(dt);
        _REENT_ERRNO(r) = ENOSYS;
        return -1;
    }
    }
}
