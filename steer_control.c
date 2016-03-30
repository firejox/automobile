#include "steer_control.h"
#include "common.h"
#include "sensor.h"
#include <assert.h>
#include <math.h>
#include "scene_object.h"
#include <stdio.h>

struct _steer_control {
    scene_object_t scene;
    mobile_t *mob;
    sensor_t *left;
    sensor_t *mid;
    sensor_t *right;
};

static paint(scene_object_t *obj, cairo_t *cr) {
    steer_control_t *con = (steer_control_t*)obj;

    scene_object_paint (con->left, cr);
    scene_object_paint (con->mid, cr);
    scene_object_paint (con->right, cr);
}

steer_control_t *steer_control_create (mobile_t *mob) {
    assert (mob);
    steer_control_t *con = xcalloc (1, sizeof (steer_control_t));

    con->scene.paint = paint;
    con->mob = mob;
    con->left = sensor_create (mob, M_PI_4);
    con->right = sensor_create (mob, -M_PI_4);
    con->mid = sensor_create (mob, 0);

    return con;
}

const sensor_t * const *steer_control_get_sensors(
        const steer_control_t *con, size_t *sz) {
    assert (con && sz);
    *sz = 3;
    return &con->left;
}

void steer_control_destroy (steer_control_t *con) {
    xfree (con);
}

static inline double fuzzy_not (double a) {
    return 1.0 - a;
}

static inline double fuzzy_and (double a, double b) {
    return a*b;
}

static inline double fuzzy_or (double a, double b) {
    return a+b-a*b;
}

static double seg_pt[] = {3, 6};


double steer_control_get_angle (const steer_control_t *con) {
    assert (con);

    double r = mobile_get_radius (con->mob);
    const sensor_t **sen = &con->left;

    double dist[3], fuzzy_val[3];
    double fire_str[3];
    double result[3] = {M_PI * 2.0 / 9.0,
            M_PI/ 180.0, -M_PI * 2.0 / 9.0};

    

    for (int i = 0; i < 3; i++) {
        dist[i] = sensor_get_distance (sen[i]);
        fuzzy_val[i] = r / fmax(dist[i], r);
    }

    fuzzy_val[1] = 1.0 / (1.0 + pow(fmax(dist[1], r) - r, M_2_PI)); 

    //double c = fuzzy_val[0] + fuzzy_val[2];
    //fuzzy_val[0] /= c;
    //fuzzy_val[2] /= c;

    /* left * angle -40 */
    fire_str[0] = fuzzy_val[0];


    /* mid and ((left and right) or (not left and not right)) */
    fire_str[1] = fuzzy_and(fuzzy_val[1],
            fuzzy_or(
                fuzzy_and(fuzzy_val[0], fuzzy_val[2]),
                fuzzy_and(fuzzy_not(fuzzy_val[0]), fuzzy_not(fuzzy_val[2])))
            );

    /* right * angle 40 */
    fire_str[2] = fuzzy_val[2];
    
    double re = 0.0;
    double fire_sum = 0.0;

    for (int i = 0; i < 3; i++) {
        fprintf (stderr, "fire_str %d : %lf\n", i, fire_str[i]);
        re += fire_str[i] * result[i];
        fire_sum += fire_str[i];
    }
    fprintf (stderr, "\n");
    
    return re / fire_sum;
}



