#include "syscalls_common.h"

// unlink
int _unlink_r(struct _reent *r, const char *name) {
    if (strlen(name) == 0) {
        _REENT_ERRNO(r) = ENOENT;
        return -1;
    }

    UTF16 *wname = __nowide_prep_path_for_syscall_r(r, name);
    if (wname == NULL) {
        return -1;
    }

    short attr = _wfgetattr(wname);
    if ((attr & ATTR_DIR) != 0) {
        free(wname);
        _REENT_ERRNO(r) = EISDIR;
        return -1;
    }

    bool result = __wremove(wname);
    free(wname);

    if (!result) {
        _REENT_ERRNO(r) = __muteki_kerrno_to_errno(_GetLastError());
        return -1;
    }
    return 0;
}
