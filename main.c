#include <gtk/gtk.h>
#include <math.h>
#include "application.h"
#include <stdio.h>
#include "common.h"
#include "vector_macro.h"
#include <stdint.h>
#include <assert.h>
#include <time.h>

#define BASEFUNC_NUM 10
#define group_num 2000
//static int group_num = 2000;
static double cross_prob = 0.8;
static double mutation_prob = 0.013;
static int iterate_num = 200;
#define input_num 3

typedef struct {
    double theta;
    double left;
    double mid;
    double right;
} nposdata_t;

typedef struct {
    nposdata_t data;
    double x;
    double y;
} posdata_t;

typedef struct {
    double w[BASEFUNC_NUM + 1];
    double m[BASEFUNC_NUM][input_num];
    double var[BASEFUNC_NUM];
    double fitness;
} gene_t;

static gene_t optimal_gene;

#define destroy_func(x) ((void)0)

VECTOR_MODULE_DEC(posdata_t)
VECTOR_MODULE_DEF(posdata_t, destroy_func)

VECTOR(posdata_t) *train_data = NULL;

static double dist(double x[input_num], double y[input_num]) {
    double ans = 0.0;
    double c;
    for (int i = 0; i < input_num; i++) {
        c = x[i] - y[i];
        ans += c * c;
    }

    return ans;
}

double rbfn_theta(const double d[input_num], gene_t *gene) {
    double val = gene->w[BASEFUNC_NUM];
    for (int j = 0; j < BASEFUNC_NUM; j++) {
        double w = gene->w[j];
        double var = gene->var[j]*gene->var[j];
        double base_ret = 
            exp(-dist(d, gene->m[j])/(var*2));

        val += base_ret * w;
    }

    return val;
}

double gene_fitness_calc (gene_t *gene) {
    const posdata_t *data = VECTOR_GET_ARRAY(posdata_t)(train_data);
    size_t data_sz = VECTOR_GET_SIZE(posdata_t)(train_data);
    double val;
    double fit = 0.0;
    
    for (int i = 0; i < data_sz; i++) {
        val = data[i].data.theta - 
            rbfn_theta(&data[i].data.left, gene);
        fit += val * val;
    }

    return fit / 2;        
}

typedef gene_t gagroup_t[group_num];

void gagroup_init(gagroup_t *group) {
    
    for (int i = 0; i < group_num; i++) {


        for (int j = 0; j < (BASEFUNC_NUM+1); j++) {
            (*group)[i].w[j] = runif();
        }

        for (int j = 0; j < BASEFUNC_NUM; j++)
            for (int k = 0; k < input_num; k++) {
                (*group)[i].m[j][k] = runif()*30;
            }

        for (int j = 0; j < BASEFUNC_NUM; j++) {
            (*group)[i].var[j] = runif()*10;

        }

        (*group)[i].fitness = gene_fitness_calc(&(*group)[i]);
    }
}

void gagroup_repoduct(gagroup_t pa_group, gagroup_t ch_group) {
    int p, q;
    double noise_lim = 0.033;
    double noise;

    p = (int)floor(runif()*group_num);
    
    for (int i = 0; i < group_num; i++) {
        q = (int)floor(runif()*group_num);
        
        if (pa_group[p].fitness > pa_group[q].fitness) 
            p = q;

        ch_group[i] =  pa_group[p];

        if (runif() > noise_lim)
            continue;
        
        for (int j = 0; j < (BASEFUNC_NUM+1); j++) {
            noise = (runif() - 0.5) * 3e-2;
            ch_group[i].w[j] = fmax(fmin(ch_group[i].w[j] + noise, 1.0), 0.0);
        }

        for (int j = 0; j < BASEFUNC_NUM; j++) {
            for (int k = 0; k < input_num; k++) {
                noise = (runif() - 0.5) * 4;
                ch_group[i].m[j][k] = fmax(
                        fmin(ch_group[i].m[j][k] + noise, 30.0), 0.0);
            }
        }

        for (int j = 0; j < BASEFUNC_NUM; j++) {
            noise = (runif() - 0.5) * 1;
            ch_group[i].var[j] = fmax(
                    fmin(ch_group[i].var[j] + noise, 10.0), 0.0);
        }
    }
}

static void gagroup_cross(gagroup_t group) {
    int i;
    int p, q;
    double sig;
    double c, fit;

    for (i = 0; i < group_num; i += 2) {
        if (runif() > cross_prob)
            continue;
        

        p = (int)floor(runif()*group_num);
        q = (int)floor(runif()*group_num);

        sig = runif();
        
        for (int j = 0; j < (BASEFUNC_NUM+1); j++) {
            c = group[p].w[j] - group[q].w[j];
            group[p].w[j] = fmax(fmin(group[p].w[j] + sig*c, 1.0), 0.0);
            group[q].w[j] = fmin(fmax(group[q].w[j] - sig*c, 0.0), 1.0); 
        }

        for (int j = 0; j < BASEFUNC_NUM; j++) {
            for (int k = 0; k < input_num; k++) {
                c = group[p].m[j][k] - group[q].m[j][k];
                group[p].m[j][k] = fmax(
                        fmin(group[p].m[j][k] + sig*c, 30.0), 0.0);
                group[q].m[j][k] = fmin(
                        fmax(group[q].m[j][k] - sig*c, 0.0), 30.0);
            }
        }

        for (int j = 0; j < BASEFUNC_NUM; j++) {
            c = group[p].var[j] - group[q].var[j];
            group[p].var[j] = fmax(fmin(group[p].var[j] + sig*c, 10.0), 0.0);
            group[q].var[j] = fmin(fmax(group[q].var[j] - sig*c, 0.0), 10.0);
        }

        group[p].fitness = gene_fitness_calc(&group[p]);
        group[q].fitness = gene_fitness_calc(&group[q]);
    }
}

static void gagroup_mutate (gagroup_t group) {
    double noise;
    double fit;

    for (int i = 0; i < group_num; i++) {
        if (runif() > mutation_prob)
            continue;

        for (int j = 0; j < (BASEFUNC_NUM+1); j++) {
            noise = (runif() - 0.5) * 7e-2;
            group[i].w[j] = fmax(fmin(group[i].w[j] + noise, 1.0), 0.0);
        }

        for (int j = 0; j < BASEFUNC_NUM; j++) {
            for (int k = 0; k < input_num; k++) {
                noise = (runif() - 0.5) * 7;
                group[i].m[j][k] = fmax(
                        fmin(group[i].m[j][k] + noise, 30.0), 0.0);
            }
        }

        for (int j = 0; j < BASEFUNC_NUM; j++) {
            noise = (runif() - 0.5) * 3;
            group[i].var[j] = fmax(
                    fmin(group[i].var[j] + noise, 10.0), 0.0);
        }

        group[i].fitness = gene_fitness_calc(&group[i]);
    }
}

void ga_init(const char *fn, double r) {
    FILE *fp = fopen(fn, "r");
    gagroup_t group[2];
    posdata_t pdata;
    nposdata_t *npdata = &pdata;
    int roll = 0;

    VECTOR_CLEAR(posdata_t)(train_data);

    assert (fp != NULL);
    //if (input_num == 3) {
        while (fscanf (fp, "%lf%lf%lf%lf",
                    &npdata->mid,
                    &npdata->right,
                    &npdata->left,
                    &npdata->theta) != EOF) {
            npdata->theta = (npdata->theta + 40.0) / 80.0;

            VECTOR_APPEND(posdata_t)(train_data, pdata);
        }        

        fclose(fp);
    /*
    } else {
        while (fscanf (fp, "%lf%lf%lf%lf%lf%lf",
                    &pdata.x,
                    &pdata.y,
                    &npdata->mid,
                    &npdata->right,
                    &npdata->left,
                    &npdata->theta) != EOF) {
            npdata->theta = (npdata->theta + 40.0) / 80.0;
            npdata->mid = (npdata->mid - r) / npdata->mid;
            npdata->right = (npdata->right - r) / npdata->right;
            npdata->left = (npdata->left - r) / npdata->left;

            pdata.x /= 30.0;
            pdata.y /= 30.0;

            VECTOR_APPEND(posdata_t)(train_data, pdata);
        }
    }
    */

    gagroup_init(&group[0]);

    for (int i = 0; i < iterate_num; i++) {

        gagroup_repoduct(group[(i&1)], group[!(i&1)]);
        //fprintf (stderr, "i : %d\n", i);
        gagroup_cross(group[!(i&1)]);
        //fprintf (stderr, "i : %d\n", i);
        gagroup_mutate(group[!(i&1)]);
        //fprintf (stderr, "i : %d\n", i);


       /* 
        int min_id = 0;
        for (int j = 1; j < group_num; j++) 
            if (group[roll][min_id].fitness > group[roll][j].fitness)
                min_id = j;

        fprintf (stderr, "iter [%d] : %lf\n", i, group[roll][min_id].fitness);
        */
    }

    int min_id = 0;
    for (int j = 1; j < group_num; j++) 
        if (group[roll][min_id].fitness > group[roll][j].fitness)
            min_id = j;

    optimal_gene = group[roll][min_id];
}

static double ga_angle(mobile_t *mob,
        double left, double mid, double right) {
    double r = mobile_get_radius(mob);
    double arr[] = {fmin(left, 30.0), fmin(mid, 30.0), fmin(right, 30.0)};
    double theta = rbfn_theta(arr, &optimal_gene);
    
    theta = fmin(fmax(theta, 0.0), 1.0);

    fprintf (stderr, "theta : %lf \n", theta);
    
    return (theta-0.5)*2*M_PI/4.5;
}


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
        srand(time(NULL));
        train_data = VECTOR_CREATE(posdata_t)();

        ga_init("../data_no_pos/train_1_D1.txt", 3);
        point_t pos = {0, 0};

        data.w = world_create();
        data.mob = mobile_create (data.w, pos, 3.0, M_PI_2);
        data.con = steer_control_create (data.mob, ga_angle);

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
