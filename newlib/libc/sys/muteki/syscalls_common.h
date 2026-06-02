/**
 * Core syscalls used by newlib.
 */

#include <errno.h>
#include <fcntl.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "bestadescriptor.h"
#include "nowide.h"
#include "mutekishims_utils.h"

#include <muteki/common.h>
#include <muteki/datetime.h>
#include <muteki/errno.h>
#include <muteki/file.h>
#include <muteki/fs.h>
#include <muteki/utils.h>
