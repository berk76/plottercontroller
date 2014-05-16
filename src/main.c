/*
*	main.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/


#include <stdio.h>
#include <string.h>
#include <math.h>
#include "printer.h"
#include "graph.h"
#include "text.h"
#include "hpgl.h"
#include "main.h"

#ifdef __linux__
#define DEVICE "/dev/parport0"
#endif

#ifdef __FreeBSD__
#define DEVICE "/dev/ppi0" 
#endif

#ifdef _WIN32
#define DEVICE "0x378" 
#endif

#define BUFF_LEN 256
static char device[BUFF_LEN];


static int show_menu(PRINTER *prn);
static void test_page(PRINTER *prn);
static void cone_demo(PRINTER *prn);
static void circles_demo(PRINTER *prn);
static void text_demo(PRINTER *prn);
static void triangle_demo(PRINTER *prn);
static void draw_triangle_fragment(PRINTER *prn, D_POSITION vertex, double len, double distance, double angle);
static void limits_demo(PRINTER *prn);
static void hpgl_demo(PRINTER *prn);


int main(int argc, char **argv) {

	PRINTER *prn;

	printf("Select plotter port: (%s): ", DEVICE);
	fgets(device, BUFF_LEN - 1, stdin);

	if ((device[strlen(device) - 1]) == '\n') {
		device[strlen(device) - 1] = '\0';
	}

	if (strlen(device) == 0) {
		strncpy(device, DEVICE, BUFF_LEN - 1);
	}

	if ((prn = pr_create_printer(device)) == NULL) {
		fprintf(stderr, "Error: Cannot access port\n");
		return -1;
	}
	
	while (show_menu(prn) != '0');

	pr_close_printer(prn);
	return 0;
}

static int show_menu(PRINTER *prn) {
	printf("---------------------------\n");
	printf("PlotterController\n");
	printf("Build 20140516\n");
	printf("---------------------------\n");
	printf("Plotter port: %s \n\n", device);

	printf("1) Test page\n");
	printf("2) Cone demo\n");
	printf("3) Circles demo\n");
	printf("4) Text demo\n");
	printf("5) Triangle demo\n");
	printf("6) Limits demo\n");
	printf("7) HPGL demo\n");
	printf("0) Exit\n");

	printf("\n");
	printf("Choose your option...\n");

	int c = getchar();
	while (getchar() != '\n');

	switch (c) {
		case '1' :	test_page(prn);
				break;
		case '2' :	cone_demo(prn);
				break;
		case '3' :	circles_demo(prn);
				break;
		case '4' :	text_demo(prn);
				break;
		case '5' :	triangle_demo(prn);
				break;
		case '6' :	limits_demo(prn);
				break;
		case '7' :	hpgl_demo(prn);
				break;
	}

	return c;
}

static void test_page(PRINTER *prn) {
	pr_init(prn);
	POSITION paper = pr_get_max_position(prn);
	
	xy_va(prn, paper.x, 0);
	xy_va(prn, paper.x, paper.y);
	xy_va(prn, 0, paper.y);
	xy_va(prn, 0, 0);

	xy_vs(prn, 7);
	xy_set_font_size(10);
	xy_set_text_angle(M_PI_2);
	xy_ma(prn, 200, paper.y / 2 - 300);
	xy_write(prn, "TEST PAGE");

	xy_vs(prn, 8);
	int x = paper.x / 2;
	int y = paper.y / 2;
	int r = 800;
	int n = 12;

	int i;
	int dir = 0;
	for (i = 0; i < n; i ++) {
		int x1 = x + r * sin(M_PI / (double) n * (double) i); 
		int y1 = y + r * cos(M_PI / (double) n * (double) i);
		int x2 = x - r * sin(M_PI / (double) n * (double) i);
		int y2 = y - r * cos(M_PI / (double) n * (double) i);
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

	xy_vs(prn, 7);
	xy_set_font_size(8);
	xy_set_text_angle(M_PI_2);
	xy_ma(prn, paper.x - 370, 100);
	xy_write(prn, "0123456789");
	xy_ma(prn, paper.x - 280, 100);
	xy_write(prn, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	xy_ma(prn, paper.x - 190, 100);
	xy_write(prn, "abcdefghijklmnopqrstuvwxyz");
	xy_ma(prn, paper.x - 100, 100);
	xy_write(prn, "!\"#$%&'()*+,-./:;<=>?@[\\]^_{|}~");

	xy_vs(prn, 8);
	xy_hm(prn);
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
	xy_vs(prn, 7);
	xy_write(prn, "OLYMPIC GAMES");
	xy_vs(prn, 8);
	xy_ma(prn, paper.x / 2 - 450, paper.y / 2 - r * 2 - dist);
	xy_vs(prn, 7);
	xy_write(prn, "SOCHI RUSSIA 2014");
	xy_vs(prn, 8);
	xy_hm(prn);
}

static void text_demo(PRINTER *prn) {
	pr_init(prn);
	POSITION paper = pr_get_max_position(prn);

	xy_set_font_size(5);
	xy_vs(prn, 7);

	int i;
	for (i = 0; i < 16; i++) {
		xy_set_text_angle(M_PI * 2 / 16 * i);
		xy_ma(prn, paper.x / 2, paper.y / 2);
		xy_write(prn, "    XY4150.WEBSTONES.CZ");
	}

	xy_vs(prn, 8);
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

static void limits_demo(PRINTER *prn) {
	pr_init(prn);
	POSITION paper = pr_get_max_position(prn);

	xy_va(prn, paper.x, 0);
	xy_va(prn, paper.x, paper.y);
	xy_va(prn, 0, paper.y);
	xy_va(prn, 0, 0);

	xy_ma(prn, paper.x / 2, paper.y / 2);
	int i;
	for (i = 1; i < 6; i++) {
		xy_cr(prn, 300 * i);
	}

	xy_hm(prn);
}

static void hpgl_demo(PRINTER *prn) {
	char file_name[100];
	printf("Enter HPGL file name: ");
	scanf("%99s", file_name);
	while (getchar() != '\n');
	hpgl_draw_from_file(prn, file_name);
}

