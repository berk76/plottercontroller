/*
*	main.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/


#include <stdio.h>
#include <math.h>
#include "printer.h"
#include "graph.h"
#include "text.h"
#include "main.h"

#define DEVICE "/dev/parport0" 


static int show_menu(PRINTER *prn);
static void cone_demo(PRINTER *prn);
static void circles_demo(PRINTER *prn);
static void text_demo(PRINTER *prn);
static void triangle_demo(PRINTER *prn);
static void draw_triangle_fragment(PRINTER *prn, D_POSITION vertex, double len, double distance, double angle);


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
	printf("PlotterController\n");
	printf("Build 20140208\n");
	printf("---------------------------\n\n");

	printf("1) Cone demo\n");
	printf("2) Circles demo\n");
	printf("3) Text demo\n");
	printf("4) Triangle demo\n");
	printf("0) Exit\n");

	printf("\n");
	printf("Choose your option...\n");

	int c = getchar();
	while (getchar() != '\n');

	switch (c) {
		case '1' :	cone_demo(prn);
				break;
		case '2' :	circles_demo(prn);
				break;
		case '3' :	text_demo(prn);
				break;
		case '4' :	triangle_demo(prn);
				break;
	}

	return c;
}

static void cone_demo(PRINTER *prn) {
	pr_init(prn);
	POSITION paper = pr_get_max_position(prn);

	int rx = 200;
	int ry = 600;
	int h = 1500;
	int n = 30;

	int cx1 = (paper.x - h) / 2;
	int cy1 = paper.y / 2;

	int cx2 = (paper.x - h) / 2 + h;
	int cy2 = paper.y / 2;
	int step = 360 / n;

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

static void circles_demo(PRINTER *prn) {
	pr_init(prn);
	POSITION paper = pr_get_max_position(prn);

	int r = 250;
	int dist = 30;

	xy_ma(prn, paper.x / 2 + r * 2 + dist, paper.y / 2 + r / 2 + dist);
	xy_cr(prn, r);
	xy_mr(prn, - r * 2 - dist, 0);
	xy_cr(prn, r);
	xy_mr(prn, - r * 2 - dist, 0);
	xy_cr(prn, r);

	xy_mr(prn, r + dist / 2, -r - dist);
	xy_cr(prn, r);
	xy_mr(prn, r * 2 + dist, 0);
	xy_cr(prn, r);

	xy_set_font_size(10);
	xy_ma(prn, paper.x / 2 - 350, paper.y / 2 + r * 2 + dist);
	xy_write(prn, "OLYMPIC GAMES");
	xy_ma(prn, paper.x / 2 - 450, paper.y / 2 - r * 2 - dist);
	xy_write(prn, "SOCHI RUSSIA 2014");

	xy_hm(prn);
}

static void text_demo(PRINTER *prn) {
	pr_init(prn);
	POSITION paper = pr_get_max_position(prn);

	xy_set_font_size(5);

	int i;
	for (i = 0; i < 16; i++) {
		xy_set_text_angle(M_PI * 2 / 16 * i);
		xy_ma(prn, paper.x / 2, paper.y / 2);
		xy_write(prn, "    XY4150.WEBSTONES.CZ");
	}

	xy_hm(prn);
}


static void triangle_demo(PRINTER *prn) {
	pr_init(prn);
	POSITION paper = pr_get_max_position(prn);

	double l = 1600;
	double h = l * cos(M_PI_2 / 3.0);
	double d = 80;

	D_POSITION p;
	p.x = paper.x / 2 - h / 2;
	p.y = paper.y / 2 - l / 2;
	draw_triangle_fragment(prn, p, l, d, 0);

	p.x = paper.x / 2 - h / 2;
	p.y = paper.y / 2 + l / 2;
	draw_triangle_fragment(prn, p, l, d, M_PI + M_PI / 3.0);

	p.x = paper.x / 2 + h / 2;
	p.y = paper.y / 2;
	draw_triangle_fragment(prn, p, l, d, M_PI * 2.0 / 3.0);

	xy_hm(prn);
}

static void draw_triangle_fragment(PRINTER * prn, D_POSITION vertex, double len, double distance, double angle) {

	double h = len * cos(M_PI_2 / 3.0);

	D_POSITION p = _transform_position(h, len / 2, angle);
	int x1s = vertex.x + p.x;
	int y1s = vertex.y + p.y;
	int x2s = vertex.x;
	int y2s = vertex.y;

	double x1, x2, y1, y2;
	int i;
	int dir = 0;
	for (i = 0; i < len / distance; i++) {
		p = _transform_position(distance * cos(M_PI_2 / 3.0) * (double) i, distance * sin(M_PI_2 / 3.0) * (double) i, angle);
		x1 = x1s - p.x;
		y1 = y1s - p.y;
		p = _transform_position(0, distance * (double) i, angle);
		x2 = x2s + p.x;
		y2 = y2s + p.y;

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
}

