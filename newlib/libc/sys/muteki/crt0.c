#include <newlib.h>

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reent.h>
#include <sys/tsrctl.h>

#include "applet_lifecycle.h"
#include "mutekishims_utils.h"

#ifdef _ENABLE_MUTEKI_LIBC_HEAP_TRACE
#include <osdep/heap.h>
#endif

#include <osdep/utls.h>

int __exit_value;
jmp_buf __exit_jmp_buf;

extern void __libc_init_array(void);
extern void __libc_fini_array(void);

static void zap_sglue(struct _glue *next) {
    if (next == NULL) {
        return;
    }
    if (next->_next != NULL) {
        zap_sglue(next->_next);
    }
    free(next);
}

// Clean up __sglue because Besta RTOS won't do it for us
static void goo_gone() {
    zap_sglue(__sglue._next);
}

static void __attribute__((constructor(1))) on_init() {
    _init_muteki_io();
}

__attribute__((used))
static int _start_after_fix(int exec_proto_ver, applet_args_v4_t *app_ctx, uintptr_t _sbz) {
#ifdef _ENABLE_MUTEKI_LIBC_HEAP_TRACE
    osdep_heap_trace_start();
#endif

    // Run all initialization hooks
    if (!tsrctl_get_flag()) {
        __libc_init_array();
    }

    // Save the execution context for exit() and start the app.
    if (!setjmp(__exit_jmp_buf)) {
#ifdef _MB_CAPABLE
        // Set the locale to UTF-8 by default. TODO: Why setting it in locale.c didn't work?
        setlocale(LC_ALL, "C.UTF-8");
#endif
        int exit_code = applet_startup(exec_proto_ver, app_ctx, _sbz);
        if (tsrctl_get_flag()) {
            // Skip STDIO auto-close if TSR flag is set.
            __stdio_exit_handler = NULL;
        }
        exit(exit_code);
    }

    if (!tsrctl_get_flag()) {
        _free_muteki_io();
        goo_gone();
        // We do not define __libc_fini so the destructors need to be called manually.
        // This is intentional as otherwise it's difficult to ensure that this gets executed the last of all libc
        // destructors, and only when TSR flag is unset.
        __libc_fini_array();
        osdep_utls_cfini();
    }

#ifdef _ENABLE_MUTEKI_LIBC_HEAP_TRACE
    osdep_heap_trace_stop();
#endif
    return __exit_value;
}

// Ensure 8-byte stack alignment for EABI compatibility.
// Note: This actually has arguments and return value but they are not declared here to prevent potential issue with naked functions.
//       See _start_after_fix for the declaration.
// Note 2: Do NOT clobber r0-r2 here as they store parameters passed from the applet caller.
__attribute__((naked))
void _start() {
    asm volatile (
        // Align to 8-bytes ourselves
        "push {r4, lr}\n\t"
        // Check for alignment
        "tst sp, #7\n\t"
        // If not aligned, the dummy value flag itself will ensure the alignment, otherwise push a dummy value to maintain the alignment.
        "mov r4, #0\n\t"
        "subeq sp, sp, #4\n\t"
        // Flag is set when dummy value is pushed.
        "moveq r4, #1\n\t"
        // Push the flag value
        "stmfd sp!, {r4}\n\t"
        // Run the actual start routine
        "bl _start_after_fix\n\t"
        // After finished, pop the dummy value flag and check it
        "ldmfd sp!, {r4}\n\t"
        "cmp r4, #0\n\t"
        // If set, pop the dummy value.
        "addne sp, sp, #4\n\t"
        // Return
        "pop {r4, pc}"
    );
}
