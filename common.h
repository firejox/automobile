#ifndef __AUTO_MOBILE_COMMON_H__
#define __AUTO_MOBILE_COMMON_H__

#include <stddef.h>

#define AUTOMOBILE_ROUND_ERROR 1e-4
extern int is_double_equal(double a, double b);

extern double radian_to_degree (double rad, double min_lim);

extern double runif(void);

extern void *xmalloc(size_t sz);
extern void *xcalloc(size_t num, size_t sz);
extern void *xrealloc(void *ptr, size_t sz);

extern void xfree(void *ptr);



#endif
