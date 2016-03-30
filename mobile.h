#ifndef __AUTO_MOBILE_MOBILE_H__
#define __AUTO_MOBILE_MOBILE_H__

#include "world.h"

typedef struct _mobile mobile_t;
typedef double (*steer_func_t)(void*);

extern mobile_t *mobile_create(world_t *world, point_t pos,
        double r, double toward_theta);

extern const world_t *mobile_get_world(const mobile_t *mob);

extern point_t mobile_get_pos (const mobile_t *mob);
extern void mobile_set_pos (mobile_t *mob, point_t pos);

extern double mobile_get_radius (const mobile_t *mob);
extern void mobile_set_radius (mobile_t *mob, double r);

extern double mobile_get_toward_theta (const mobile_t *mob);
extern void mobile_set_toward_theta (mobile_t *mob, double theta);

extern void mobile_update(mobile_t *mob, steer_func_t func, void *user_data);

extern int is_mobile_out_of_world (const mobile_t *mob);

extern void mobile_destroy(mobile_t *mob);


#endif
