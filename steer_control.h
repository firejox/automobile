#ifndef __AUTO_MOBILE_STEER_CONTROL_H__
#define __AUTO_MOBILE_STEER_CONTROL_H__

#include "mobile.h"
#include "sensor.h"

typedef struct _steer_control steer_control_t;

extern steer_control_t *steer_control_create (mobile_t *mob);

extern double steer_control_get_angle(const steer_control_t *con);

extern const sensor_t * const *steer_control_get_sensors(
        const steer_control_t *con, size_t *sz);

extern void steer_control_destroy(steer_control_t *con);

#endif
