#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

#include <newlib.h>
#include <_ansi.h>

#ifdef _HAS_MUTEKI_SUBMODULE
#include <muteki/threading.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Provide minimal definitions when muteki is unavailable. These must be kept
 * in sync with the shims although it should be stable enough to not require
 * any changes. */
#ifndef _HAS_MUTEKI_SUBMODULE
struct bxc_thread_s;
typedef struct bxc_thread_s bxc_thread_t;

typedef enum bxc_wait_result_e {
    BXC_WAIT_RESULT_TIMEOUT = 0x82,
    BXC_WAIT_RESULT_RESOLVED,
    BXC_WAIT_RESULT_ERROR,
} bxc_wait_result_t;

struct bxc_cs_s {
    int magic;
    bxc_thread_t *thr;
    unsigned short refcount;
    struct {
        unsigned char active_bytes;
        unsigned char waiting_by[8];
    } wait_state;
    char _padding_0x13;
};
struct bxc_semaphore_s;

typedef struct bxc_cs_s bxc_cs_t;
typedef struct bxc_semaphore_s bxc_semaphore_t;

extern bxc_semaphore_t *OSCreateSemaphore(short init_ctr);
extern bool OSCloseSemaphore(bxc_semaphore_t *semaphore);
extern bxc_wait_result_t OSWaitForSemaphore(bxc_semaphore_t *semaphore, short timeout);
extern bool OSReleaseSemaphore(bxc_semaphore_t *semaphore);

extern void OSInitCriticalSection(bxc_cs_t *cs);
extern void OSDeleteCriticalSection(bxc_cs_t *cs);
extern void OSEnterCriticalSection(bxc_cs_t *cs);
extern void OSLeaveCriticalSection(bxc_cs_t *cs);
#endif

typedef bxc_semaphore_t *_LOCK_T;
typedef bxc_cs_t _LOCK_RECURSIVE_T;

static inline int __lock_try_acquire_recursive_impl(_LOCK_RECURSIVE_T *plock) {
    OSEnterCriticalSection(plock);
    return 0;
}

#define __LOCK_INIT(class,lock) class _LOCK_T lock;
#define __LOCK_INIT_RECURSIVE(class,lock) class _LOCK_RECURSIVE_T lock;
#define __lock_init(lock) ({ lock = OSCreateSemaphore(1); })
#define __lock_init_recursive(lock) (OSInitCriticalSection(&lock))
#define __lock_close(lock) ({ OSCloseSemaphore(lock); lock = NULL; })
#define __lock_close_recursive(lock) (OSDeleteCriticalSection(&lock))
#define __lock_acquire(lock) ({ while (OSWaitForSemaphore(lock, 32767) != WAIT_RESULT_RESOLVED); })
#define __lock_acquire_recursive(lock) (OSEnterCriticalSection(&lock))
#define __lock_try_acquire(lock) ((OSWaitForSemaphore(lock, 0) == WAIT_RESULT_RESOLVED) ? 0 : -1)
#define __lock_try_acquire_recursive(lock) (__lock_try_acquire_recursive_impl(&lock))
#define __lock_release(lock) ((void) OSReleaseSemaphore(lock))
#define __lock_release_recursive(lock) (OSLeaveCriticalSection(&lock))

#ifdef __cplusplus
}
#endif

#endif /* __SYS_LOCK_H__ */
