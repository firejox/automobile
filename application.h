#ifndef __AUTO_MOBILE_APPLICATION_H__
#define __AUTO_MOBILE_APPLICATION_H__

#include "world.h"
#include "mobile.h"
#include "steer_control.h"

#include <gtk/gtk.h>

typedef struct {
    world_t *w;
    mobile_t *mob;
    steer_control_t *con;
    
} application_data_t;


extern void application_init (GtkApplication *app, void *use_data);


#endif
