#ifndef __MUTEKI_LIBC_NOWIDE_H__
#define __MUTEKI_LIBC_NOWIDE_H__

#include <reent.h>

#include <muteki/common.h>

#if __WCHAR_MAX__ < 0x10000 && defined(_MB_CAPABLE)
#define _HAS_SHORT_WCHAR
#endif

#ifdef _HAS_SHORT_WCHAR
#include <stdlib.h>
#include <wchar.h>
#define __nowide_mbstate_t mbstate_t
#define __nowide_mbstobestawcs_r _mbstowcs_r
#define __nowide_bestawcstombs_r _wcstombs_r
#define __nowide_bestawcslen wcslen
#else
typedef struct {
    int __count;
    union {
        uint32_t __wch;
        unsigned char __wchb[4];
    } __value;		/* Value so far.  */
} __nowide_mbstate_t;

extern size_t __nowide_mbstobestawcs_r(struct _reent *r, UTF16 *__restrict pwcs, const char *__restrict s, size_t n, __nowide_mbstate_t *state);
extern size_t __nowide_bestawcstombs_r (struct _reent *r, char *__restrict s, const UTF16 *__restrict pwcs, size_t n, __nowide_mbstate_t *state);
extern size_t __nowide_bestawcslen(const UTF16 *s);
#endif
extern UTF16 *__nowide_prep_path_for_syscall_r(struct _reent *r, const char *path);
extern UTF16 *__nowide_mbstobestawcs_dup_r(struct _reent *r, const char *a);

#endif // __MUTEKI_LIBC_NOWIDE_H__
