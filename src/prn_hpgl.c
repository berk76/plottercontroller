/*
*       prn_hpgl.c
*
*       This file is part of PlotterController project.
*       https://github.com/berk76/plottercontroller
*
*       PlotterController is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 3 of the License, or
*       (at your option) any later version. <http://www.gnu.org/licenses/>
*
*       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 22.12.2018
*/


#include <stdio.h>
#include <stdlib.h>
#include "printer.h"
#include "hpgl.h"
#include "main.h"


static void print_help();


int main(int argc, char **argv) {
        PRINTER *prn;
        int i;
        char *val = NULL;
        char sw;

        double scale = 1;
        char *file = NULL;
        char interface = ' ';

        for (i = (argc - 1); i != 0; i--) {

                if (argv[i][0] != '-') {
                        val = argv[i];
                        continue;
                }

                sw = argv[i][1];

                switch (sw) {
                        case 's' :
                                        if (val != NULL) {
                                                scale = atof(val);
                                                val = NULL;
                                        }
                                        break;
                        case 'i' :
                                        if (val != NULL) {
                                                interface = *val;
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

        if ((interface == ' ') || (scale == 0) || (scale < 0)) {
                print_help();
                return -1;
        }

        switch (interface) {
                case '1' :      prn = pr_create_printer(PARPORT, "/dev/parport0");
                                break;
                case '2' :      prn = pr_create_printer(PARPORT, "/dev/ppi0");
                                break;
                case '3' :      prn = pr_create_printer(PARPORT, "0x378");
                                break;
                case '4' :      prn = pr_create_printer(GPIO, "1");
                                break;
                case '5' :      prn = pr_create_printer(GPIO, "2");
                                break;
                default :       fprintf(stderr, "Bad interface number\n");
                                print_help();
                                return -1;
        }

        if (prn == NULL) {
                fprintf(stderr, "Error: Cannot access port\n");
                return -1;
        }

        hpgl_draw_from_file(prn, file, scale);

        return 0;
}


void print_help() {
        printf("prn_hpgl version %s\n\n", PLOTTER_CONTROLLER_VERSION);

        printf("Usage:\n");
        printf("prn_hpgl [-s <scale factor>] -i <interface number> -f <file.hpgl>\n\n");

        printf("Interface numbers available:\n");
        printf("(1) Linux PC (/dev/parport0)\n");
        printf("(2) FreeBSD PC (/dev/ppi0)\n");
        printf("(3) DOS PC (0x378)\n");
        printf("(4) Raspberry PI version 1 (GPIO v1)\n");
        printf("(5) Raspberry PI version 2 (GPIO v2)\n\n");

        return;
}
