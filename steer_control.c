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
    anglefunc_t getangle;
};

static paint(scene_object_t *obj, cairo_t *cr) {
    steer_control_t *con = (steer_control_t*)obj;

    scene_object_paint (con->left, cr);
    scene_object_paint (con->mid, cr);
    scene_object_paint (con->right, cr);
}

steer_control_t *steer_control_create (mobile_t *mob, anglefunc_t func) {
    assert (mob);
    steer_control_t *con = xcalloc (1, sizeof (steer_control_t));

    con->scene.paint = paint;
    con->mob = mob;
    con->left = sensor_create (mob, M_PI_4);
    con->right = sensor_create (mob, -M_PI_4);
    con->mid = sensor_create (mob, 0);
    con->getangle = func;

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



double steer_control_get_angle (const steer_control_t *con) {
    assert (con);

    const sensor_t **sen = &con->left;
    
    return con->getangle(con->mob, 
            sensor_get_distance(con->left),
            sensor_get_distance(con->mid), 
            sensor_get_distance(con->right));
}



