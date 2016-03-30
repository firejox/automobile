#include "scene_object.h"
#include <stddef.h>

void scene_object_paint (scene_object_t *obj, cairo_t *cr) {
    if (obj !=  NULL && obj->paint != NULL) 
        obj->paint(obj, cr);
}
