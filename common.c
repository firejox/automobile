
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "common.h"
#include <math.h>

int is_double_equal(double a, double b) {
    return fabs(a - b) < AUTOMOBILE_ROUND_ERROR; 
}

double radian_to_degree(double radian, double min_lim) {
    double mod = remainder (radian, M_PI * 2);

    if (mod < min_lim)
        mod += M_PI * 2;

    return mod / M_PI * 180.0;
}

void *xmalloc (size_t sz) {
    void *tmp = malloc(sz);

    if (tmp == NULL) {
        perror("xmalloc failed : ");
        
        assert(tmp);
    } 

    return tmp;
}

void *xcalloc (size_t num, size_t sz) {
    void *tmp = calloc(num, sz);

    if (tmp == NULL) {
        perror("xcalloc failed : ");

        assert(tmp);
    }

    return tmp;
}


void *xrealloc (void *ptr, size_t sz) {
    void *tmp = realloc(ptr, sz);

    if (tmp == NULL) {
        perror("xrealloc failed : ");

        assert(tmp);
    }

    return tmp;
}

void xfree (void *ptr) {
    free(ptr);
}
