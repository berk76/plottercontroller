/*
*	main.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/


#include <stdio.h>
#include <math.h>
#include "printer.h"
#include "graph.h"
#include "main.h"

#define DEVICE "/dev/parport0" 

static int show_menu(PRINTER *prn);
static void vector_test(PRINTER *prn);
static void circle_test(PRINTER *prn);
static void cone_demo(PRINTER *prn);


int main(int argc, char **argv) {

	PRINTER *prn;

	if ((prn = pr_create_printer(DEVICE)) == NULL) {
		fprintf(stderr, "Error: Probably you don't have permission access %s\n", DEVICE);
		return -1;
	}
	
	while (show_menu(prn) != '0');

	pr_close_printer(prn);
	return 0;
}

static int show_menu(PRINTER *prn) {
	printf("XY-1450 controlling program\n");
	printf("Build 20140123\n");
	printf("---------------------------\n\n");

	printf("1) Vector test\n");
	printf("2) Circle test\n");
	printf("3) Cone demo\n");
	printf("0) Exit\n");

	printf("\n");
	printf("Choose your option...\n");

	int c = getchar();
	while (getchar() != '\n');

	switch (c) {
		case '1' :	vector_test(prn);
					break;
		case '2' :	circle_test(prn);
					break;	
		case '3' :	cone_demo(prn);
					break;
	}

	return c;
}

static void vector_test(PRINTER *prn) {
 	pr_init(prn);

	xy_vr(prn, 2600, 0);
	xy_vr(prn, 0,1850);
	xy_vr(prn, -2600,0);
	xy_vr(prn, 0,-1850);
	xy_vr(prn, 2600,1850);
	xy_mr(prn, 0,-1850);
	xy_vr(prn, -2600,1850);
	xy_hm(prn);
}

static void circle_test(PRINTER *prn) {
	pr_init(prn);

	xy_mr(prn, 2600/2, 1850/2);
	xy_cr(prn, 800);
	xy_ar(prn, 700, M_PI * 0.25, M_PI * 0.75);
	xy_ar(prn, 700, M_PI, M_PI * 2);
	xy_hm(prn);
}


static void cone_demo(PRINTER *prn) {

	int rx = 200;
	int ry = 600;
	int h = 1500;
	int n = 30;

	int cx1 = (2600 - h) / 2;
	int cy1 = 1850 / 2;

	int cx2 = (2600 - h) / 2 + h;
	int cy2 = 1850 / 2;
	int step = 360 / n;

	pr_init(prn);

	int dir = 0;
	int i;
	for (i = 0; i < 360; i += step) {
		int x1 = (double) rx * sin(M_PI / 180 * i) + cx1;
		int y1 = (double) ry * cos(M_PI / 180 * i) + cy1;
		int x2 = (double) rx * sin(M_PI / 180 * (i + 12 * step)) + cx2;
		int y2 = (double) ry * cos(M_PI / 180 * (i + 12 * step)) + cy2;
		if (dir == 0) {
			xy_ma(prn, x1, y1);
			xy_va(prn, x2, y2);
			dir = 1;
		} else {
			xy_ma(prn, x2, y2);
			xy_va(prn, x1, y1);
			dir = 0;
		}
	}
	
	xy_hm(prn);
}
