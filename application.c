#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <math.h>
#include <string.h>
#include <cairo.h>

#include "application.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

static void manual_change_mobile_x (GtkWidget *, gpointer);
static void manual_change_mobile_y (GtkWidget *, gpointer);
static void manual_change_mobile_toward_theta (GtkWidget *, gpointer);

typedef struct {
    application_data_t p;
    cairo_surface_t *track;
    cairo_surface_t *surface;

    GtkWidget *darea;
    GtkEntry *x, *y, *theta;
    GtkLabel *sensor[3];
    GtkLabel *steer_angle;
    GtkToggleButton *start_stop_btn;
    gint64 clock;
} appbakend_data_t;

static void mobile_info_update (appbakend_data_t *);
static void button_toggled (GtkWidget *, gpointer);
static void set_mobile_info (GtkButtonBox*, appbakend_data_t*);
static gboolean update (GtkWidget *widget, GdkFrameClock *frame,
        gpointer user_data);

void application_init (GtkApplication *app, void *user_data) {
    GtkWidget *window;
    GtkWidget *box;
    application_data_t *data = (application_data_t*) user_data;
    
    appbakend_data_t *b_data = xcalloc (1, sizeof (appbakend_data_t));
    b_data->p = *data;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW(window), "steer control");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    
    gtk_container_add (GTK_CONTAINER(window), box);

    b_data->darea = gtk_drawing_area_new ();
    //gtk_widget_set_size_request (b_data->darea, 100, 100);
    gtk_box_pack_start (GTK_BOX(box), b_data->darea, TRUE, TRUE, 0);

    canvas_init (b_data->darea, b_data);

    GtkWidget *bbox = gtk_button_box_new (GTK_ORIENTATION_VERTICAL);
    gtk_button_box_set_layout (GTK_BUTTON_BOX(bbox),
            GTK_BUTTONBOX_CENTER);

    gtk_box_set_homogeneous (GTK_BOX(bbox), TRUE);
    set_mobile_info(GTK_BUTTON_BOX(bbox), b_data);
    gtk_box_pack_start (GTK_BOX(box), bbox, FALSE, FALSE, 0);

    gtk_widget_add_tick_callback(box, update, b_data, xfree);

    gtk_widget_show_all (window);
}

void set_mobile_info (GtkButtonBox *bbox, appbakend_data_t *b_data) {
    GtkWidget *box, *label;

    {
        box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_box_set_homogeneous (GTK_BOX(box), TRUE);
        
        label = gtk_label_new ("x : ");
        b_data->x = gtk_entry_new ();

        gtk_entry_set_input_purpose (b_data->x, GTK_INPUT_PURPOSE_NUMBER);

        gtk_box_pack_start (GTK_BOX(box), label, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX(box), b_data->x, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX(bbox), box, TRUE, TRUE, 0);

        g_signal_connect (b_data->x, "activate",
               manual_change_mobile_x, b_data);
    }

    {
        box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_box_set_homogeneous (GTK_BOX(box), TRUE);
        
        label = gtk_label_new ("y : ");
        b_data->y = gtk_entry_new ();

        gtk_entry_set_input_purpose (b_data->y, GTK_INPUT_PURPOSE_NUMBER);
        gtk_box_pack_start (GTK_BOX(box), label, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX(box), b_data->y, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX(bbox), box, TRUE, TRUE, 0);

        g_signal_connect (b_data->y, "activate",
                manual_change_mobile_y, b_data);
    }

    { 
        box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_box_set_homogeneous (GTK_BOX(box), TRUE);

        label = gtk_label_new ("theta : ");
        b_data->theta = gtk_entry_new ();

        gtk_entry_set_input_purpose (b_data->theta, GTK_INPUT_PURPOSE_NUMBER);
        gtk_box_pack_start (GTK_BOX(box), label, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX(box), b_data->theta, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX(bbox), box, TRUE, TRUE, 0);

        g_signal_connect (b_data->theta, "activate", 
                manual_change_mobile_toward_theta, b_data);
    }

    for (int i = 0; i < 3; i++) {
        b_data->sensor[i] = gtk_label_new("");

        gtk_box_pack_start (GTK_BOX(bbox), b_data->sensor[i],
                TRUE, TRUE, 0);
    }

    b_data->steer_angle = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX(bbox), b_data->steer_angle, 
            TRUE, TRUE, 0);

    b_data->start_stop_btn = gtk_toggle_button_new_with_label (
            "media-playback-start");

    gtk_box_pack_start (GTK_BOX(bbox), b_data->start_stop_btn,
            TRUE, TRUE, 0);


    g_signal_connect (b_data->start_stop_btn, "toggled", 
            button_toggled, b_data);

    g_object_bind_property (b_data->start_stop_btn, "active",
            b_data->x, "editable", G_BINDING_INVERT_BOOLEAN);

    g_object_bind_property (b_data->start_stop_btn, "active",
            b_data->y, "editable", G_BINDING_INVERT_BOOLEAN);
    
    g_object_bind_property (b_data->start_stop_btn, "active",
            b_data->theta, "editable", G_BINDING_INVERT_BOOLEAN);

}


static gboolean update (GtkWidget *widget, GdkFrameClock *frame,
        gpointer user_data) {
    gint64 cur_clock = gdk_frame_clock_get_frame_time (frame);
    appbakend_data_t *data = user_data;

    if (data->clock + 100000 <= cur_clock) {
        if (world_get_state(data->p.w) == WORLD_TIME_RUNNING) {
            point_t pos = mobile_get_pos (data->p.mob);

            if (is_mobile_out_of_world(data->p.mob) || pos.y > 37.0) {
                gtk_toggle_button_set_active (data->start_stop_btn, FALSE);

                fprintf (stderr, "x : %lf y: %lf\n", pos.x, pos.y);
            }
            else
                mobile_update (data->p.mob,
                        steer_control_get_angle, data->p.con);

            canvas_draw (user_data);

            mobile_info_update (data);

            gtk_widget_queue_draw (data->darea);
        }
        data->clock = cur_clock;
    }

    return G_SOURCE_CONTINUE;
}

static void mobile_info_update (appbakend_data_t *data) {
    char s[32];
    point_t pos = mobile_get_pos (data->p.mob);

    snprintf (s, sizeof (s), "%lf", pos.x);
    gtk_entry_set_text (data->x, s);

    snprintf (s, sizeof (s), "%lf", pos.y);
    gtk_entry_set_text (data->y, s);

    snprintf (s, sizeof (s), "%lf",
            radian_to_degree(
                mobile_get_toward_theta(data->p.mob), -M_PI_2));
    gtk_entry_set_text (data->theta, s);

    size_t sen_sz;
    const sensor_t * const * sens = steer_control_get_sensors(
            data->p.con, &sen_sz);

    snprintf (s, sizeof (s), "left : %lf",
            sensor_get_distance(sens[0]));
    gtk_label_set_text (data->sensor[0], s);

    snprintf (s, sizeof (s), "mid : %lf",
            sensor_get_distance(sens[1]));
    gtk_label_set_text (data->sensor[1], s);

    snprintf (s, sizeof (s), "right : %lf",
            sensor_get_distance(sens[2]));
    gtk_label_set_text (data->sensor[2], s);

    snprintf (s, sizeof (s), "steer angle : %lf",
            radian_to_degree(
                steer_control_get_angle(data->p.con), -M_PI_2));
    gtk_label_set_text (data->steer_angle, s);
}

static void manual_change_mobile_x (GtkWidget *w, gpointer data) {
    appbakend_data_t *b_data = (appbakend_data_t*)data;
    GtkEntry *entry = GTK_ENTRY(w);
    point_t pos = mobile_get_pos(b_data->p.mob);

    point_t new_pos = {
        .x = atof (gtk_entry_get_text(entry)),
        .y = pos.y
    };



    fprintf (stderr, "new pos : %lf %lf \n", new_pos.x, new_pos.y);

    mobile_set_pos (b_data->p.mob, new_pos);

    mobile_info_update(b_data);
}

static void manual_change_mobile_y (GtkWidget *w, gpointer data) {
    appbakend_data_t *b_data = (appbakend_data_t*)data;
    GtkEntry *entry = GTK_ENTRY(w);
    point_t pos = mobile_get_pos(b_data->p.mob);

    point_t new_pos = {
        .x = pos.x,
        .y = atof(gtk_entry_get_text(entry))
    };

    mobile_set_pos (b_data->p.mob, new_pos);

    mobile_info_update(b_data);
}

static void manual_change_mobile_toward_theta (GtkWidget *w, gpointer data) {
    appbakend_data_t *b_data = (appbakend_data_t*)data;
    GtkEntry *entry = GTK_ENTRY(w);

    double theta = atof (gtk_entry_get_text(entry)) / 180.0 * M_PI;

    mobile_set_toward_theta (b_data->p.mob, theta);

    mobile_info_update(b_data);
}

static void button_toggled (GtkWidget *w, gpointer data) {
    GtkToggleButton *btn = (GtkToggleButton*)w;
    application_data_t *b_data = (application_data_t*) data;

    if (gtk_toggle_button_get_active(btn)) {
        world_set_state(b_data->w, WORLD_TIME_RUNNING);

        gtk_button_set_label(btn, "media-playbck-pause");
    } else {
        world_set_state(b_data->w, WORLD_TIME_STOP);

        gtk_button_set_label(btn, "media-playbck-start");
    }

}

