#include "applet_lifecycle.h"
#include <alloca.h>
#include <string.h>

__attribute__((weak))
int applet_startup(int exec_proto_ver, applet_args_v4_t *applet_args, uintptr_t _sbz) {
    char *dospath = NULL;
    int subroutine = APPLET_SUBROUTINE_MAIN;
    void *applet_arg1 = NULL;
    void *applet_arg2 = NULL;

    if (exec_proto_ver != 4) {
        const char *dospath_ro = GetCurrentPathA();
        size_t bufsize = strlen(dospath_ro) + 1;
        dospath = alloca(bufsize);
        memcpy(dospath, dospath_ro, bufsize);
    } else {
        dospath = applet_args->dospath;
        subroutine = *(applet_args->subroutine);
        applet_arg1 = *(applet_args->applet_arg1);
        applet_arg2 = *(applet_args->applet_arg2);
    }

    return applet_main(dospath, subroutine, applet_arg1, applet_arg2);
}
