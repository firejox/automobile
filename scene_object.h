#ifndef __AUTO_MOBILE_SCENE_OBJECT_H__
#define __AUTO_MOBILE_SCENE_OBJECT_H__

#include <cairo/cairo.h>

typedef struct _scene_object scene_object_t;

struct _scene_object {
    void (*paint) (scene_object_t *obj, cairo_t *cr);
};

#define SCENE_OBJECT(x) ((scene_object_t*)x)

extern void scene_object_paint (scene_object_t *obj, cairo_t *cr);

#endif
