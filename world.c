#include "world.h"
#include "common.h"
#include <assert.h>
#include "scene_object.h"

struct _world {
    scene_object_t scene;
    VECTOR(segment_t) *segments;
    world_state_t state;
};

static void wall_paint (const segment_t *seg, void *user_data) {
    cairo_t *cr = user_data;

    cairo_save (cr);

    cairo_new_path (cr);

    cairo_move_to (cr, seg->a.x, seg->a.y);
    cairo_line_to (cr, seg->b.x, seg->b.y);

    cairo_stroke_preserve (cr);

    cairo_restore (cr);
}

static void paint (scene_object_t *obj, cairo_t *cr) {
    const world_t *w = (world_t *)obj;

    cairo_save (cr);
    cairo_set_line_width (cr, 1.0);
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
  
    VECTOR_FOR_EACH(segment_t) (w->segments, wall_paint, cr);

    cairo_restore (cr);
}

world_t *world_create (void) {
    world_t *w = xmalloc(sizeof (world_t));

    w->scene.paint = paint;
    w->segments = VECTOR_CREATE(segment_t)();
    w->state = WORLD_TIME_STOP;
    return w;
}

void world_set_state (world_t *w, world_state_t st) {
    assert (w);
    w->state = st;
}

world_state_t world_get_state (const world_t *w) {
    assert (w);
    return w->state;
}

void world_add_segment (world_t *w, segment_t seg) {
    assert(w);
    
    VECTOR_APPEND(segment_t)(w->segments, seg);
}

const VECTOR(segment_t) *world_get_segments(const world_t *w) {
    assert(w);

    return w->segments;
}


void world_destroy(world_t *w) {
    VECTOR_DESTROY(segment_t)(w->segments);
    xfree (w);
}
