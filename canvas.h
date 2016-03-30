#ifndef __AUTO_MOBILE_CANVAS_H__
#define __AUTO_MOBILE_CANVAS_H__

#include "application.h"
#include <gtk/gtk.h>

extern void canvas_draw_track (application_data_t*);

extern void canvas_draw (application_data_t*);

extern void canvas_init (GtkWidget*, application_data_t*);



#endif
