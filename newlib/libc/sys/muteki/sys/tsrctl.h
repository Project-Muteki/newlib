#ifndef _SYS_TSRCTL_H
#define _SYS_TSRCTL_H

#ifdef __cplusplus
extern "C" {
#endif

extern void tsrctl_set_flag(int flag);
extern int tsrctl_get_flag(void);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // _SYS_TSRCTL_H
