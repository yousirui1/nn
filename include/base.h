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

#define NCHW      1
//#define NHWC      1

#ifdef NCHW
// torch (batch_size, channels, height, width)
// darknet (batch_size, channels, height, width)
#define N 0
#define C 1
#define H 2
#define W 3
#elif NHWC
// tf (batch_size, height, width, channels)
#define N 0
#define H 1
#define W 2
#define C 3
#else   //NHWC

#endif  //NCHW


#endif //  __BASE_H__
