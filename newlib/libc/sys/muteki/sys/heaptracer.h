#ifndef __SYS_HEAPTRACER_H__
#define __SYS_HEAPTRACER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <osdep/heap.h>

#define heaptracer_start() osdep_heap_trace_start()
#define heaptracer_stop() osdep_heap_trace_stop()

#ifdef __cplusplus
}
#endif

#endif
