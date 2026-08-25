#include "syscalls_common.h"

static const UTF16 __READ_BINARY[] = _BUL("rb");
static const UTF16 __RW_BINARY[] = _BUL("rb+");
static const UTF16 __RW_BINARY_TRUNC[] = _BUL("wb+");
static const UTF16 __APPEND_BINARY[] = _BUL("ab+");

static const UTF16 *open_flag_to_besta(int flags) {
    int accmode = flags & O_ACCMODE;
    switch (accmode) {
    case O_WRONLY:
    case O_RDWR:
        if (flags & O_APPEND) {
            return __APPEND_BINARY;
        }
        if (flags & O_TRUNC) {
            return __RW_BINARY_TRUNC;
        }
        return __RW_BINARY;
    case O_RDONLY:
        return __READ_BINARY;
    default:
        return NULL;
    }
}

static bxc_find_context_t *__muteki_wfind_under_dir_r(struct _reent *r, const UTF16 *wname) {
    // Process wname into a fnmatch pattern
    UTF16 *fnmatch = __muteki_dir_to_fnmatch(wname);
    if (fnmatch == NULL) {
        _REENT_ERRNO(r) = EINVAL;
        return NULL;
    }

    // Create a new find context and use it as the system directory handle
    bxc_find_context_t *find_ctx = malloc(sizeof(bxc_find_context_t));
    if (find_ctx == NULL) {
        _REENT_ERRNO(r) = ENOMEM;
        free(fnmatch);
        return NULL;
    }

    if (_wfindfirst(fnmatch, find_ctx, 0) < 0) {
        _REENT_ERRNO(r) = __muteki_kerrno_to_errno(_GetLastError());
        free(fnmatch);
        free(find_ctx);
        return NULL;
    }

    free(fnmatch);
    return find_ctx;
}

// open
int _open_r(struct _reent *r, const char *name, int flags, int mode) {
    void *sys_fd = NULL;
    bool exists, is_dir;

    if (strlen(name) == 0) {
        _REENT_ERRNO(r) = ENOENT;
        return -1;
    }

    UTF16 *wname = __nowide_prep_path_for_syscall_r(r, name);
    if (wname == NULL) {
        return -1;
    }

    short attr = _wfgetattr(wname);

    if (attr < 0) {
        exists = false;
        is_dir = false;
    } else {
        exists = true;
        is_dir = (attr & BXC_FS_ATTR_DIR) != 0;
    }

    if (!(flags & O_CREAT) && !exists) {
        _REENT_ERRNO(r) = ENOENT;
        free(wname);
        return -1;
    }

    if (is_dir) {
        if (!(flags & O_DIRECTORY)) {
            _REENT_ERRNO(r) = EISDIR;
            free(wname);
            return -1;
        }

        sys_fd = __muteki_wfind_under_dir_r(r, wname);

        if (sys_fd == NULL) {
            free(wname);
            return -1;
        }
    } else {
        if (flags & O_DIRECTORY) {
            _REENT_ERRNO(r) = ENOTDIR;
            free(wname);
            return -1;
        }

        const UTF16 *besta_mode = open_flag_to_besta(flags);
        if (besta_mode == NULL) {
            _REENT_ERRNO(r) = EINVAL;
            free(wname);
            return -1;
        }

        sys_fd = __wfopen(wname, besta_mode);
        if (sys_fd == NULL) {
            _REENT_ERRNO(r) = __muteki_kerrno_to_errno(_GetLastError());
            free(wname);
            return -1;
        }
    }

    // Create the descriptor and keep the path string alive
    int fd = __muteki_acquire_descriptor();
    DescriptorTranslation *dt = __muteki_fd_grab(fd);
    dt->handle = sys_fd;
    dt->type = is_dir ? MUTEKI_DESCRIPTOR_DIRECTORY : MUTEKI_DESCRIPTOR_FILE;
    dt->filename = wname;
    __muteki_fd_drop(dt);
    return fd;
}
