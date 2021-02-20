/*
*       fit_hpgl.c
*
*       This file is part of PlotterController project.
*       https://github.com/berk76/plottercontroller
*
*       PlotterController is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 3 of the License, or
*       (at your option) any later version. <http://www.gnu.org/licenses/>
*
*       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 28.12.2018
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"


static int info = 0;


static void examine(char *file_name, int *px, int *py);
static void get_size_cmd(char *cmd, int *px, int *py);
static void get_size(char *cmd, int *px, int *py);
static void write(char *file_name, double scale, int flip, int xmax);
static void write_cmd(char *cmd, double scale, int flip, int xmax);
static void resize(char *cmd, double scale, int flip, int xmax);
static void resize_ci(char *cmd, double scale);
static int getParamAsInt(char *cmd, int which);
static void swap(int *a, int *b);
static void print_help();


int main(int argc, char **argv) {
        int i;
        char *val = NULL;
        char sw;
        char *file = NULL;
        int x, y;
        int nx, ny, flip;
        double f1, f2;
        

        info = 0;
        nx = 0; ny = 0;
        
        for (i = (argc - 1); i != 0; i--) {

                if (argv[i][0] != '-') {
                        val = argv[i];
                        continue;
                }

                sw = argv[i][1];

                switch (sw) {
                        case 'i' :
                                        info = 1;
                                        break;
                        case 'x' :
                                        if (val != NULL) {
                                                nx = atoi(val);
                                                val = NULL;
                                        }
                                        break;
                        case 'y' :
                                        if (val != NULL) {
                                                ny = atoi(val);
                                                val = NULL;
                                        }
                                        break;
                        case 'f' :
                                        if (val != NULL) {
                                                file = val;
                                                val = NULL;
                                        }
                                        break;
                        
                        default :
                                fprintf(stderr, "Unknown option %c\n", sw);
                }
        }

        if ((file == NULL) || (nx < 1) || (ny < 1)) {
                print_help();
                return -1;
        }

        /* check for original size */
        x = 0; y = 0;
        examine(file, &x, &y);

        /* compare orientation */
        flip = 0;
        if (((nx < ny) && (x < y)) || ((nx > ny) && (x > y))) {
                flip = 0;
        } else {
                flip = 1;
        }

        /* calc scale factor */
        if (flip) swap(&x, &y);
        f1 = (double) nx / x;
        f2 = (double) ny / y;
        f1 = (f1 < f2) ? f1 : f2;
        if (flip) swap(&x, &y);

        if (info) {
                printf("Drawing size is %d, %d\n", x, y);
                printf("Requested size is %d, %d\n", nx, ny);
                printf("Proposed scale is %f", f1);
                if (flip)
                        printf(" and flip image");
                printf("\n");
        } else {
                write(file, f1, flip, nx);
        }

        return 0;
}


void examine(char *file_name, int *px, int *py) {
        FILE *fr;
        int i;
        char c;
        char *cmd;
        int maxlen = 50;
        int len;


        cmd = (char *) malloc(sizeof(char) * (maxlen + 1));
        if (cmd == NULL) {
                fprintf(stderr, "Could not allocate memory\n");
                exit(-1);
        } 

        if ((fr = fopen(file_name, "r")) == NULL) {
                fprintf(stderr, "Error: Cannot open file %s for reading\n", file_name);
                exit(-1);
        }

        *cmd = '\0';
        len = 0;

        while ((i = getc(fr)) != EOF) {
                c = i;

                if (c < ' ') {
                        continue;
                }

                if (c == ';') {
                        /* printf("%s;\n",cmd); */
                        get_size_cmd(cmd, px, py);
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
        free((void *) cmd);
}


void get_size_cmd(char *cmd, int *px, int *py) {

        if (!strncmp(cmd, "PA", 2)) {
                get_size(cmd, px, py);
                return;
        }
        if (!strncmp(cmd, "PR", 2)) {
                get_size(cmd, px, py);
                return;
        }
        if (!strncmp(cmd, "PU", 2)) {
                get_size(cmd, px, py);
                return;
        }
        if (!strncmp(cmd, "PD", 2)) {
                get_size(cmd, px, py);
                return;
        }
        if (!strncmp(cmd, "CI", 2)) {
                return;
        }

        if (info) {
                printf("Unknown command: %s\n", cmd);
        }
}


void get_size(char *cmd, int *px, int *py) {
        int x, y, i;

        i = 0;

        while (1) {
                x = getParamAsInt(cmd, i++);
                y = getParamAsInt(cmd, i++);

                if (y == -1) {
                        return;
                } else {
                        if (x > *px)
                                *px = x;
                                
                        if (y > *py)
                                *py = y;
                }
        }
}


static void write(char *file_name, double scale, int flip, int xmax) {
        FILE *fr;
        int i;
        char c;
        char *cmd;
        int maxlen = 50;
        int len;


        cmd = (char *) malloc(sizeof(char) * (maxlen + 1));
        if (cmd == NULL) {
                fprintf(stderr, "Could not allocate memory\n");
                exit(-1);
        } 

        if ((fr = fopen(file_name, "r")) == NULL) {
                fprintf(stderr, "Error: Cannot open file %s for reading\n", file_name);
                exit(-1);
        }

        *cmd = '\0';
        len = 0;

        while ((i = getc(fr)) != EOF) {
                c = i;
                
                if (c < ' ') {
                        continue;
                }

                if (c == ';') {
                        /* printf("%s;\n",cmd); */
                        write_cmd(cmd, scale, flip, xmax);
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
        free((void *) cmd);
}


void write_cmd(char *cmd, double scale, int flip, int xmax) {
        if (!strncmp(cmd, "PA", 2)) {
                resize(cmd, scale, flip, xmax);
                return;
        }
        if (!strncmp(cmd, "PR", 2)) {
                resize(cmd, scale, flip, xmax);
                return;
        }
        if (!strncmp(cmd, "PU", 2)) {
                resize(cmd, scale, flip, xmax);
                return;
        }
        if (!strncmp(cmd, "PD", 2)) {
                resize(cmd, scale, flip, xmax);
                return;
        }
        if (!strncmp(cmd, "CI", 2)) {
                resize_ci(cmd, scale);
                return;
        }
        
        printf("%s;\n",cmd);
}


void resize(char *cmd, double scale, int flip, int xmax) {
        int x, y, i;

        i = 0;

        putchar(cmd[0]);
        putchar(cmd[1]);

        while (1) {
                x = getParamAsInt(cmd, i++);
                y = getParamAsInt(cmd, i++);

                if (y == -1) {
                        printf(";\n");
                        return;
                } else {
                        x = scale * (double) x;
                        y = scale * (double) y;
                        if (flip) {
                                swap(&x, &y);
                                x = -1 * (x - xmax);
                        }

                        if (i > 2)
                                putchar(',');
                        printf("%d,%d", x, y);
                }
        }
}


void resize_ci(char *cmd, double scale) {
        int r;


        putchar(cmd[0]);
        putchar(cmd[1]);
        r = scale * (double) getParamAsInt(cmd, 0);
        printf("%d;\n", r);
}


int getParamAsInt(char *cmd, int which) {
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

        return -1;
}


void swap(int *a, int *b) {
        int c;

        c = *a;
        *a = *b;
        *b = c;
}


void print_help() {
        printf("fit_hpgl version %s\n\n", PLOTTER_CONTROLLER_VERSION);

        printf("Usage:\n");
        printf("fit_hpgl [-i] -x <new x size> -y <new y size> -f <file.hpgl>\n\n");

        printf("-i\tprint info only\n");
        printf("-f\tinput file\n");
        printf("-x\tnew x size\n");
        printf("-y\tnew y size\n");
        return;
}
