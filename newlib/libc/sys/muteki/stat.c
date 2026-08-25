#include "syscalls_common.h"

static time_t find_timestamp_to_unix(unsigned int find_ts) {
    struct tm dt;

    dt.tm_year = BXC_FIND_TS_YEAR(find_ts) - 1900;
    dt.tm_mon = BXC_FIND_TS_MONTH(find_ts) - 1;
    dt.tm_mday = BXC_FIND_TS_DAY(find_ts);
    dt.tm_hour = BXC_FIND_TS_HOUR(find_ts);
    dt.tm_min = BXC_FIND_TS_MINUTE(find_ts);
    dt.tm_sec = BXC_FIND_TS_SECOND(find_ts);
    dt.tm_isdst = -1;

    return mktime(&dt);
}

static int stat_from_find_ctx(struct stat *out, bxc_find_context_t *in) {
    memset(out, 0, sizeof(struct stat));

    out->st_size = (off_t) (in->size & 0x7fffffffl);
    out->st_atime = find_timestamp_to_unix(in->atime);
    out->st_mtime = find_timestamp_to_unix(in->mtime);
    // Linux maps ctime to FAT btime, but Besta's btime is not usable due to a bug (?) on the OS side. So we map ctime to mtime instead.
    out->st_ctime = out->st_mtime;

    if (in->attrib & BXC_FS_ATTR_DIR) {
        out->st_mode |= _IFDIR;
    } else {
        out->st_mode |= _IFREG;
    }

    if ((sizeof(off_t) == 4) && (in->size & 0x80000000ul)) {
        return -EOVERFLOW;
    }
    return 0;
}

// fstat
int _fstat_r(struct _reent *r, int fd, struct stat *st) {
    bxc_find_context_t find_ctx;
    DescriptorTranslation *dt = __muteki_fd_grab(fd);

    if (_wfindfirst(dt->filename, &find_ctx, 0) < 0) {
        _REENT_ERRNO(r) = __muteki_kerrno_to_errno(_GetLastError());
        __muteki_fd_drop(dt);
        return -1;
    }

    int result = stat_from_find_ctx(st, &find_ctx);
    _findclose(&find_ctx);
    if (result < 0) {
        _REENT_ERRNO(r) = -result;
        __muteki_fd_drop(dt);
        return -1;
    }

    __muteki_fd_drop(dt);
    return 0;
}

// stat
int _stat_r(struct _reent *r, const char *name, struct stat *st) {
    bxc_find_context_t find_ctx;

    UTF16 *wname = __nowide_prep_path_for_syscall_r(r, name);
    if (wname == NULL) {
        return -1;
    }

    if (_wfindfirst(wname, &find_ctx, 0) < 0) {
        free(wname);
        _REENT_ERRNO(r) = __muteki_kerrno_to_errno(_GetLastError());
        return -1;
    }
    free(wname);

    int result = stat_from_find_ctx(st, &find_ctx);
    _findclose(&find_ctx);
    if (result < 0) {
        free(wname);
        _REENT_ERRNO(r) = -result;
        return -1;
    }

    return 0;
}
