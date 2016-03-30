#ifndef __AUTO_MOBILE_WORLD_H__
#define __AUTO_MOBILE_WORLD_H__

#include "vector_segment.h"

typedef struct _world world_t;

typedef enum {
    WORLD_TIME_STOP,
    WORLD_TIME_RUNNING
} world_state_t;


extern world_t *world_create(void);

extern void world_set_state (world_t *w, world_state_t st);

extern world_state_t world_get_state (const world_t *w);

extern void world_add_segment(world_t *w, segment_t seg);

extern const VECTOR(segment_t) *world_get_segments(const world_t *w);

extern void world_destroy(world_t *w);


#endif
