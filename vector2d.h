#ifndef __AUTO_MOBILE_VECTOR2D_H__
#define __AUTO_MOBILE_VECTOR2D_H__

#include "point.h"
#include <math.h>

typedef struct _vector2d vector2d_t;

struct _vector2d {
    double entry[2];
};

#define vector2d_gen_by_point(from, to) \
    ((vector2d_t){.entry = {to.x - from.x, to.y - from.y}})

#define vector2d_length(vec) hypot(vec.entry[0], vec.entry[1])


#endif
