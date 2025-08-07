#include <sys/tsrctl.h>

static int __tsr_flag = 0;

void tsrctl_set_flag(int flag) {
  __tsr_flag = flag;
}

int tsrctl_get_flag(void) {
  return __tsr_flag;
}
