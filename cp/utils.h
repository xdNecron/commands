#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BUFSIZE 1024 /* default buffer size */
#define NULLB 1 /* null byte size macro to enhance readability */

// Simple error handler
static inline int die(const char *msg) {
    perror(msg);
    return 1;
}

static inline int diefn(const char *msg) {
    perror(msg);
    return -1;
}

#endif
