#include "syscalls_common.h"

// environ
char *__env[1] = { 0 };
char **environ = __env;
