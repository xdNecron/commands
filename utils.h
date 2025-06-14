#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// Macro for buffer size
#define BUFSIZE 1024

// Simple error handler
static inline int die(const char *msg) {
    perror(msg);
    return 1;
}

#endif
