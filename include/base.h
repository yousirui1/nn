#ifndef __BASE_H__
#define __BASE_H__

#include <stdio.h>
#include <sys/errno.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#include "log.h"

#define SUCCESS     0
#define ERROR       -1


#define SCHED_PRIORITY_DECODE       0
#define SCHED_PRIORITY_ANALYSIS     1

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define STRPREFIX(a,b) (strncmp((a),(b),strlen((b))) == 0)

#endif //  __BASE_H__
