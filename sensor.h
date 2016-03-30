#ifndef __AUTO_MOBILE_SENSOR_H__
#define __AUTO_MOBILE_SENSOR_H__

#include "mobile.h"

typedef struct _sensor sensor_t;

sensor_t *sensor_create(mobile_t *mob, double theta);

double sensor_get_theta(const sensor_t *sensor);

double sensor_get_distance(const sensor_t *sensor);

void sensor_destroy (sensor_t *sensor);

#endif
