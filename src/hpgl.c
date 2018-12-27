/*
*       hpgl.c
*
*       This file is part of PlotterController project.
*       https://github.com/berk76/plottercontroller
*
*       PlotterController is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 3 of the License, or
*       (at your option) any later version. <http://www.gnu.org/licenses/>
*
*       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 8.2.2014
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "printer.h"
#include "graph.h"


/* pen up = 0, down = 1 */
static int pen = 0;
static double scale = 1;


static void process_cmd(PRINTER *p, char *cmd);
static void hpgl_pa(PRINTER *p, char *cmd);
static void hpgl_pr(PRINTER *p, char *cmd);
static void hpgl_ci(PRINTER *p, char *cmd);
static int getParamAsInt(char *cmd, int which);


/* Draw hpgl file */
void hpgl_draw_from_file(PRINTER *p, char *file_name, double scale_factor) {
        FILE *fr;
        char c;
        char *cmd;
        int maxlen = 50;
        int len;


        scale = scale_factor;

        cmd = (char *) malloc(sizeof(char) * (maxlen + 1));
        if (cmd == NULL) {
                fprintf(stderr, "Could not allocate memory\n");
                exit(-1);
        } 

        if ((fr = fopen(file_name, "r")) == NULL) {
                printf("Error: Cannot open file %s for reading\n", file_name);
                return;
        }

        pr_init(p);
        *cmd = '\0';
        len = 0;

        while ((c = getc(fr)) != EOF) {

                if (c < ' ') {
                        continue;
                }

                if (c == ';') {
                        process_cmd(p, cmd);
                        *cmd = '\0';
                        len = 0;
                } else {
                        strncat(cmd, &c, 1);
                        len++;
                }

                if (strlen(cmd) == maxlen) {
                        maxlen = maxlen * 2;
                        cmd = (char *) realloc(cmd, sizeof(char) * (maxlen + 1));
                        if (cmd == NULL) {
                                fprintf(stderr, "Could not allocate memory\n");
                                exit(-1);
                        } 
                }
        }

        fclose(fr);
        xy_hm(p);
        free((void *) cmd);
}


static void process_cmd(PRINTER *p, char *cmd) {
        if (!strncmp(cmd, "PA", 2)) {
                hpgl_pa(p, cmd);
                return;
        }
        if (!strncmp(cmd, "PR", 2)) {
                hpgl_pr(p, cmd);
                return;
        }
        if (!strncmp(cmd, "PU", 2)) {
                pen = 0;
                return;
        }
        if (!strncmp(cmd, "PD", 2)) {
                pen = 1;
                return;
        }
        if (!strncmp(cmd, "EA", 2)) {
                fprintf(stderr, "Warning: Edge Rectangle Absolute is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "ER", 2)) {
                fprintf(stderr, "Warning: Edge Rectangle Relative is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "RA", 2)) {
                fprintf(stderr, "Warning: Shade Rectangle Absolute is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "RR", 2)) {
                fprintf(stderr, "Warning: Shade Rectangle Relative is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "AA", 2)) {
                fprintf(stderr, "Warning: Arc Absolute is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "AR", 2)) {
                fprintf(stderr, "Warning: Arc Relative is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "CI", 2)) {
                hpgl_ci(p, cmd);
                return;
        }
        if (!strncmp(cmd, "EW", 2)) {
                fprintf(stderr, "Warning: Edge Wedge is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "WG", 2)) {
                fprintf(stderr, "Warning: Shade Wedge is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "FT", 2)) {
                fprintf(stderr, "Warning: Fill Type is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "LT", 2)) {
                fprintf(stderr, "Warning: Line Type is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "PM", 2)) {
                fprintf(stderr, "Warning: Polygon Mode is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "PT", 2)) {
                fprintf(stderr, "Warning: Pen Thickness is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "SP", 2)) {
                fprintf(stderr, "Warning: Select Pen is not implemented\n");
                return;
        }
        if (!strncmp(cmd, "VS", 2)) {
                fprintf(stderr, "Warning: Velocity Select is not implemented\n");
                return;
        }

        printf("Unknown command: %s\n", cmd);
}


/* Plot Absolute */
static void hpgl_pa(PRINTER *p, char *cmd) {
        int x, y;

        x = scale * (double) getParamAsInt(cmd, 0);
        y = scale * (double) getParamAsInt(cmd, 1);

        if (pen) {
                xy_va(p, x, y);
        } else {
                xy_ma(p, x, y);
        }
}


/* Plot Relative */
static void hpgl_pr(PRINTER *p, char *cmd) {
        int x, y;

        x = scale * (double) getParamAsInt(cmd, 0);
        y = scale * (double) getParamAsInt(cmd, 1);

        if (pen) {
                xy_vr(p, x, y);
        } else {
                xy_mr(p, x, y);
        }
}


/* Circle */
static void hpgl_ci(PRINTER *p, char *cmd) {
        int r;
        r = scale * (double) getParamAsInt(cmd, 0);
        xy_cr(p, r);
}


/* Extracts parameters from cmd */
static int getParamAsInt(char *cmd, int which) {
        char *p;
        int i;

        p = cmd;

        if (*p != '\0' && *(p + 1) != '\0') {
                p += 2;
        }

        i = 0;

        while (*p != '\0') {
                if (i == which) {
                        return atoi(p);
                }
                if ((*p == ',') || (*p == ' ')) {
                        i++;
                }
                p++;
        }

        return 0;
}
