#define _GNU_SOURCE

#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#include <sys/sysmacros.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <fcntl.h>

#define DEBUG 0
#define INFO 1
#define WARNING 2
#define ERROR 3

#define STACK_SIZE (1024 * 1024)

static int
pivot_root(const char *new_root, const char *put_old)
{
   return syscall(SYS_pivot_root, new_root, put_old);
}
