#include <_ansi.h>
#include <errno.h>
#include <reent.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <osdep/heap.h>

void *_malloc_r(struct _reent *r, size_t size) {
    void *q = osdep_heap_alloc(size);

    if (q == NULL) {
        _REENT_ERRNO(r) = ENOMEM;
        return NULL;
    }

    return q;
}

void *_calloc_r(struct _reent *r, size_t nmemb, size_t size) {
    void *p = _malloc_r(r, nmemb * size);

    if (p == NULL) {
        _REENT_ERRNO(r) = ENOMEM;
        return NULL;
    }

    memset(p, 0, nmemb * size);
    return p;
}

void *_realloc_r(struct _reent *r, void *ptr, size_t size) {
    size_t orig_size = osdep_heap_get_alloc_size(ptr);
    size_t copy_size = (size <= orig_size) ? size : orig_size;

    void *p = _malloc_r(r, size);
    if (p == NULL) {
        _REENT_ERRNO(r) = ENOMEM;
        return NULL;
    }

    memcpy(p, ptr, copy_size);
    _free_r(r, ptr);
    return p;
}

void _free_r(struct _reent *r, void *ptr) {
    if (ptr == NULL) {
        return;
    }

    osdep_heap_free(ptr);
}

/* Below are copied from newlib's reentrent to C standard functions. */

void *malloc(size_t nbytes) {
    return _malloc_r(_REENT, nbytes);
}

void free(void *aptr) {
    _free_r(_REENT, aptr);
}

void *calloc(size_t n, size_t size) {
    return _calloc_r(_REENT, n, size);
}

void *realloc(void *ap, size_t nbytes) {
    return _realloc_r(_REENT, ap, nbytes);
}
