#include <gtk/gtk.h>
#include <math.h>
#include "application.h"

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    application_data_t data;

    double segs[][4] = {
        {-6, 0, -6, 22},
        {-6, 22, 18, 22},
        {18, 22, 18, 37},
        {30, 37, 30, 10}, 
        {30, 10, 6, 10},
//        {6, -3, -6, -3},
//        {6, 0, 6, -3},
//        {-6, 0, -6, -3},
        {6, 10, 6, 0}
    };

    {
        point_t pos = {0, 0};

        data.w = world_create();
        data.mob = mobile_create (data.w, pos, 3.0, M_PI_2);
        data.con = steer_control_create (data.mob);

        for (int i= 0; i < (sizeof(segs)/sizeof(segs[0])); i++) {
            segment_t seg = {
                .a = { segs[i][0], segs[i][1] },
                .b = { segs[i][2], segs[i][3] }
            };

            world_add_segment (data.w, seg);
        }
    }

    app = gtk_application_new ("org.automobile.ai",
            G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", application_init, &data);

    status = g_application_run (G_APPLICATION(app), argc, argv);

    g_object_unref (app);

    world_destroy (data.w);
    mobile_destroy (data.mob);
    steer_control_destroy (data.con);

    return 0;
}
