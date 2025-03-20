#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

#include <newlib.h>
#include <_ansi.h>

#include <muteki/threading.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef semaphore_t *_LOCK_T;
typedef critical_section_t _LOCK_RECURSIVE_T;

#define __LOCK_INIT(class,lock) class _LOCK_T lock;
#define __LOCK_INIT_RECURSIVE(class,lock) class _LOCK_RECURSIVE_T lock;
#define __lock_init(lock) ({ lock = OSCreateSemaphore(1); })
#define __lock_init_recursive(lock) (OSInitCriticalSection(&lock))
#define __lock_close(lock) ({ OSCloseSemaphore(lock); lock = NULL; })
#define __lock_close_recursive(lock) (OSDeleteCriticalSection(&lock))
#define __lock_acquire(lock) ({ while (OSWaitForSemaphore(lock, 32767) != WAIT_RESULT_RESOLVED); })
#define __lock_acquire_recursive(lock) (OSEnterCriticalSection(&lock))
#define __lock_try_acquire(lock) ((OSWaitForSemaphore(lock, 0) == WAIT_RESULT_RESOLVED) ? 1 : 0)
#define __lock_try_acquire_recursive(lock) ((lock.refcount == 0) ? 1 : 0)
#define __lock_release(lock) ((void) OSReleaseSemaphore(lock))
#define __lock_release_recursive(lock) (OSLeaveCriticalSection(&lock))

#ifdef __cplusplus
}
#endif

#endif /* __SYS_LOCK_H__ */
