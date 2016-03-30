#include "mobile.h"
#include "common.h"
#include <math.h>
#include "scene_object.h"
#include "vector2d.h"
#include <stdio.h>

struct _mobile {
    scene_object_t scene;
    world_t *w;
    point_t pos;
    double radius;
    double theta;
};

static void paint(scene_object_t *obj, cairo_t *cr) {
    const mobile_t *mob = (mobile_t*)obj;

    cairo_save (cr);
    
    { /* draw the shape of car */

        cairo_new_path (cr);
        cairo_set_source_rgb (cr, 0.2, 1.0, 0.0);
        cairo_set_line_width (cr, 0.3);
        cairo_arc (cr, mob->pos.x, mob->pos.y, mob->radius, 0, 2*M_PI);
        cairo_stroke_preserve (cr);
    }

    { /* draw the toward arrow */
        cairo_save (cr);
        cairo_translate (cr, mob->pos.x, mob->pos.y);

        double x = (mob->radius + 1.0) * cos(mob->theta);
        double y = (mob->radius + 1.0) * sin(mob->theta);

        cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);

        cairo_new_path (cr);

        cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
        cairo_move_to (cr, 0.0, 0.0);
        cairo_line_to (cr, x, y);
        cairo_stroke_preserve (cr); 

        cairo_new_path (cr);
        
        cairo_move_to (cr, x, y);
        cairo_line_to (cr, x, y - 0.3);
        cairo_line_to (cr, x - 0.3, y);
        
        cairo_close_path (cr);
        
        cairo_stroke_preserve (cr);
        cairo_fill (cr);
        

        cairo_restore (cr);
    }
    cairo_restore (cr);
}


mobile_t *mobile_create (world_t *w, point_t pos,
        double r, double toward_theta) {
    mobile_t *mob;

    mob = xcalloc (1, sizeof(mobile_t));
    mob->scene.paint = paint;
    mob->w = w;
    mob->pos = pos;
    mob->radius = r;
    mob->theta = toward_theta;

    return mob;
}

const world_t *mobile_get_world (const mobile_t *mob) {
    return mob->w;
}

point_t mobile_get_pos (const mobile_t *mob) {
    return mob->pos;
}

void mobile_set_pos (mobile_t *mob, point_t pos) {
    if (mob) 
        mob->pos = pos;
}

double mobile_get_radius (const mobile_t *mob) {
    return mob->radius;
}

void mobile_set_radius (mobile_t *mob, double r) {
    if (mob) 
        mob->radius = r;
}

double mobile_get_toward_theta (const mobile_t *mob) {
    return mob->theta;
}

void mobile_set_toward_theta (mobile_t *mob, double theta) {
    if (mob)
        mob->theta = theta;
}

void mobile_update(mobile_t *mob, steer_func_t func, void *user_data) {
    if (mob == NULL)
        return;

    double theta = func(user_data);
    double v = cos(theta);

    mob->pos.x += v * cos(mob->theta);
    mob->pos.y += v * sin(mob->theta);

    mob->theta -= asin(sin(theta) / 3.0);
    
}

int is_mobile_out_of_world(const mobile_t *mob) {
    const world_t *w = mob->w;
    const segment_t *segs = VECTOR_GET_ARRAY(segment_t)(
            world_get_segments(w));
    const size_t segs_sz = VECTOR_GET_SIZE(segment_t)(
            world_get_segments(w));

    for (size_t i = 0; i < segs_sz; i++) {
        vector2d_t va = vector2d_gen_by_point(mob->pos, segs[i].a);
        vector2d_t vb = vector2d_gen_by_point(mob->pos, segs[i].b);
        vector2d_t vc = vector2d_gen_by_point(segs[i].a, segs[i].b);

        double a = va.entry[0] * vc.entry[0] + va.entry[1] * vc.entry[1];
        double b = vb.entry[0] * vc.entry[0] + vb.entry[1] * vc.entry[1];
        double c = fabs(va.entry[0] * vb.entry[1] - va.entry[1] * vb.entry[0]);
        double va_len = vector2d_length(va);
        double vb_len = vector2d_length(vb);
        double vc_len = vector2d_length(vc);

        if (va_len >= mob->radius - AUTOMOBILE_ROUND_ERROR && 
                vb_len >= mob->radius - AUTOMOBILE_ROUND_ERROR) {
            if (a * b <= AUTOMOBILE_ROUND_ERROR) {


                if ((c / vc_len) < mob->radius - AUTOMOBILE_ROUND_ERROR) {
                    //fprintf (stderr, "pos : %lf %lf \n",
                    //        mob->pos.x, mob->pos.y);
                    return 1;
                }
            }
        } else {
            //fprintf (stderr, "seg a: %lf %lf "
            //            " b: %lf %lf\n",
            //            segs[i].a.x, segs[i].a.y,
            //            segs[i].b.x, segs[i].b.y);
            //fprintf (stderr, "va_len : %lf vb_len : %lf\n"
            //        "pos : %lf %lf \n",
            //        va_len, vb_len,
            //        mob->pos.x, mob->pos.y);
            return 1;
        }
    }

    return 0;
}

void mobile_destroy(mobile_t *mob) {
    xfree (mob);
}
