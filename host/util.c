#include "util.h"

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

void* checked_malloc(size_t size) {
    void * p = malloc(size);
    if (p == NULL) {
        printf("malloc: failed to allocate %d bytes of memory", size);
        exit(EXIT_FAILURE);
    }
    return p;
}