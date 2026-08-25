#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <muteki/errno.h>

#include "mutekishims_utils.h"
#include "nowide.h"

int __muteki_kerrno_to_errno(bxc_errno_t kerrno) {
    short err = BXC_ERRNO_ERR(kerrno);
    short ns = BXC_ERRNO_NS(kerrno);
    switch (ns) {
    case BXC_ERRNO_NS_KERNEL:
        switch (err) {
        case BXC_ERR_FS_INVALID_DRIVE_LETTER:
            return ENODEV;
        case BXC_ERR_FS_INVALID_FILENAME:
        case BXC_ERR_FS_CONFLICTING_ATTR:
        case BXC_ERR_FTL_INVALID_LBA:
            return EINVAL;
        case BXC_ERR_FTL_DATA_CORRUPTED:
        case BXC_ERR_FTL_ECC_FAILED:
        case BXC_ERR_FS_OPERATION_ERROR:
            return EIO;
        case BXC_ERR_FS_ENTRY_EXISTS:
            return EEXIST;
        case BXC_ERR_FS_FILE_UNAVAILABLE:
        case BXC_ERR_FS_NO_SUCH_ENTRY:
        case BXC_ERR_FS_PATH_TRAVERSAL:
            return ENOENT;
        case BXC_ERR_FS_DIR_RECORD_GROWTH:
        case BXC_ERR_FS_EOF_REACHED:
        case BXC_ERR_FS_FAT16_DIR_ALLOC:
        case BXC_ERR_FS_NO_SPACE_LEFT:
            return ENOSPC;
        case BXC_ERR_MEDIUM_WP_ENABLED:
        case BXC_ERR_FS_READ_ONLY_FILE:
            return EROFS;
        case BXC_ERR_FS_FILE_LOCKED:
            return EBUSY;
        case BXC_ERR_FS_PATH_TOO_LONG:
            return ENAMETOOLONG;
        case BXC_ERR_FS_TOO_MANY_OPEN_FILES:
            return ENFILE;
        case BXC_ERR_MEDIUM_UNLOADED:
            return ENOMEDIUM;
        case BXC_ERR_MEDIUM_INCOMPATIBLE:
            return EMEDIUMTYPE;
        // Explicitly ignore.
        case BXC_ERR_FS_FILE_ATTR_ERROR: // May be handled in specific C APIs.
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
}

UTF16 *__muteki_dir_to_fnmatch(const UTF16 *path) {
    size_t wlen = __nowide_bestawcslen(path);
    if (wlen < 1) {
        return NULL;
    }
    if (path[wlen - 1] == _BUL('\\')) {
        UTF16 *fnmatch = malloc((wlen + 2) * sizeof(UTF16));
        memcpy(fnmatch, path, wlen * sizeof(UTF16));
        fnmatch[wlen] = _BUL('*');
        fnmatch[wlen+1] = 0;
        return fnmatch;
    } else {
        UTF16 *fnmatch = malloc((wlen + 3) * sizeof(UTF16));
        memcpy(fnmatch, path, wlen * sizeof(UTF16));
        fnmatch[wlen] = _BUL('\\');
        fnmatch[wlen+1] = _BUL('*');
        fnmatch[wlen+2] = 0;
        return fnmatch;
    }
}
