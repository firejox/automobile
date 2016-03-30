#include <cairo.h>
#include <assert.h>
#include "common.h"
#include "canvas.h"
#include "scene_object.h"
#include <math.h>

typedef struct {
    application_data_t p;
    cairo_surface_t *track;
    cairo_surface_t *surf;
} canvasbacken_data_t;

void canvas_draw_track (application_data_t *a_data) {
    canvasbacken_data_t *data = (canvasbacken_data_t*) a_data;

    assert (data);

    if (data->track && world_get_state(data->p.w) == WORLD_TIME_RUNNING) {
        cairo_t *cr = cairo_create(data->track);
        cairo_translate (cr, 100, 100);
        cairo_scale (cr, 8.0, 8.0);
        point_t pos = mobile_get_pos(data->p.mob);
        
        cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
        cairo_arc (cr, pos.x, pos.y, 0.5, 0, 2*M_PI);
        cairo_fill (cr);

        cairo_destroy (cr);
    }
}

void canvas_draw (application_data_t *a_data) {
    canvasbacken_data_t *data = (canvasbacken_data_t*)a_data;

    assert (data);

    canvas_draw_track (a_data);

    if (data->surf) {
        cairo_t *cr = cairo_create(data->surf);
        cairo_save (cr);
        cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint (cr);
        cairo_restore (cr);

        cairo_translate (cr, 200.0, 400.0);

        cairo_scale (cr, 1.0, -1.0);
        cairo_set_source_surface(cr, data->track, -100.0, -100.0);
        cairo_paint (cr);

        cairo_scale (cr, 8.0, 8.0);

        scene_object_paint (data->p.w, cr);
        scene_object_paint (data->p.mob, cr);
        scene_object_paint (data->p.con, cr);

        cairo_destroy (cr);
    }

}

static void clear_surface (cairo_surface_t *face) {
    cairo_t *cr = cairo_create (face);
    
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint (cr);

    cairo_destroy (cr);
}

static gboolean configure_cb (GtkWidget *widget,
        GdkEventConfigure *evt, gpointer user_data) {
    canvasbacken_data_t *data = user_data;
    assert (data);

    int w = gtk_widget_get_allocated_width (widget);
    int h = gtk_widget_get_allocated_height (widget);

    if (data->track) 
        cairo_surface_destroy (data->track);
    data->track = gdk_window_create_similar_surface (
            gtk_widget_get_window (widget),
            CAIRO_CONTENT_COLOR_ALPHA,
            w, h);
    clear_surface (data->track);

    if (data->surf)
        cairo_surface_destroy (data->surf);

    data->surf = cairo_surface_create_similar (
            data->track,
            CAIRO_CONTENT_COLOR_ALPHA,
            w, h);

    clear_surface (data->surf);

    canvas_draw (data);
    gtk_widget_queue_draw (widget);

    return TRUE;
}

static void draw_widget (GtkWidget *w, cairo_t *cr, gpointer user_data) {
    canvasbacken_data_t *data = user_data;

    cairo_save (cr);
    //cairo_scale (cr, 1.0, 1.0);
    cairo_set_source_surface (cr, data->surf, 0.0, 0.0);
    cairo_paint (cr);

    cairo_restore (cr);


    return FALSE;
}

static surface_destroy (GtkWidget *w, gpointer user_data) {
    canvasbacken_data_t *data = user_data;

    assert (data);

    if (data->track)
        cairo_surface_destroy (data->track);
    if (data->surf)
        cairo_surface_destroy (data->surf);
}

void canvas_clean_track (application_data_t *user_data) {
    canvasbacken_data_t *data = user_data;

    assert (data);

    clear_surface (data->track);
}

void canvas_init (GtkWidget *w, application_data_t *data) {


    g_signal_connect (w, "configure-event", configure_cb, data);
    g_signal_connect (w, "draw", draw_widget, data);
    g_signal_connect (w, "destroy", surface_destroy, data);

}
