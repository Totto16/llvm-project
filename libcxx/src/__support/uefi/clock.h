

#pragma once

#include <sys/time.h>
#include <time.h>

int clock_gettime(clockid_t clockid, struct timespec* tp);
