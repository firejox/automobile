#include "sensor.h"
#include "common.h"
#include "vector2d.h"
#include <assert.h>
#include <math.h>
#include "scene_object.h"
#include <stdio.h>


struct _sensor {
    scene_object_t scene;
    mobile_t *mob;
    double theta; 
};

static void paint (scene_object_t *obj, cairo_t *cr) {
    const sensor_t *sen = (sensor_t*)obj;
    double theta = mobile_get_toward_theta (sen->mob) + sen->theta;
    const point_t pos = mobile_get_pos(sen->mob);
    double r = sensor_get_distance (sen);
    
    if (r == HUGE_VAL)
        r = 1000.0;

    double rx = r * cos(theta);
    double ry = r * sin(theta);

    cairo_save (cr);
    
    cairo_new_path (cr);

    cairo_set_line_width (cr, 0.3);
    cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
    
    cairo_move_to (cr, pos.x, pos.y);
    cairo_line_to (cr, rx + pos.x, ry +  pos.y);

    cairo_stroke_preserve (cr);

    cairo_restore (cr);

}

sensor_t *sensor_create(mobile_t *mob, double theta) {
    sensor_t *sensor;

    assert (mob);

    sensor = xcalloc (1, sizeof(sensor_t));

    sensor->scene.paint = paint;
    sensor->mob = mob;
    sensor->theta = theta;

    return sensor;
}


double sensor_get_theta(const sensor_t *sensor) {
    return sensor->theta;
}


static inline double calculate_det (double mat[2][2]) {
    return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

double sensor_get_distance(const sensor_t *sensor) {
    const mobile_t *mob = sensor->mob;
    const world_t *world = mobile_get_world(mob);
    
    double theta = sensor->theta + mobile_get_toward_theta(mob);
    const double arr[2] = {cos(theta), sin(theta)};

    const point_t mob_pos = mobile_get_pos(mob);
    const segment_t *segs = VECTOR_GET_ARRAY(segment_t)(
            world_get_segments(world));
    const size_t segs_sz = VECTOR_GET_SIZE(segment_t)(
            world_get_segments(world));

    double dist = HUGE_VAL;

    for (size_t i = 0; i < segs_sz; i++) {
        const vector2d_t va = vector2d_gen_by_point(mob_pos, segs[i].a);
        const vector2d_t vb = vector2d_gen_by_point(mob_pos, segs[i].b);

        //if ((va.entry[0] * arr[0] + va.entry[1] * arr[1]) < 0.0)
        //    continue;
        //if ((vb.entry[0] * arr[0] + vb.entry[1] * arr[1]) < 0.0)
        //    continue;

        const double mat[2][2] = {{va.entry[0], vb.entry[0]},
            {va.entry[1], vb.entry[1]}};

        const double det = calculate_det(mat);

        if (!is_double_equal(det, 0.0)) {
            const double invm[2][2] = {
                {mat[1][1] / det, -mat[0][1] / det},
                {-mat[1][0] / det, mat[0][0] / det}};
            const double a = invm[0][0] * arr[0] + invm[0][1] * arr[1];
            const double b = invm[1][0] * arr[0] + invm[1][1] * arr[1];
            const double c = a + b;

            if (a > -AUTOMOBILE_ROUND_ERROR && b > - AUTOMOBILE_ROUND_ERROR) {
                dist = fmin (dist, 
                        hypot(mat[0][0]*a/c + mat[0][1]*b/c, 
                            mat[1][0]*a/c + mat[1][1]*b/c));
            }
        } else {
            const double mmat[2][2] = {{mat[0][0], arr[0]},
                {mat[1][0], arr[1]}};

            if (is_double_equal(calculate_det(mmat) ,0.0)) {
                dist = fmin(dist,
                        fmin(hypot(mat[0][0], mat[1][0]),
                             hypot(mat[0][1], mat[1][1])));
            }
        }
    }

    return dist;
}

void sensor_destroy(sensor_t *sensor) {
    xfree (sensor);
}
