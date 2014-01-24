/*
*	graph.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#include <stdio.h>
#include <math.h>
#include "printer.h"
#include "graph.h"

static void xy_move_rel(PRINTER *p, int x, int y);
static void xy_pen_up(PRINTER *p);
static void xy_pen_down(PRINTER *p);
static void xy_flush_moving_buffer(PRINTER *p);

/* Move Absolute */
void xy_ma(PRINTER *p, int x, int y) {
	POSITION pos = pr_get_current_position(p);
	xy_mr(p, x - pos.x, y - pos.y);
}

/* Move Relative */
void xy_mr(PRINTER *p, int x, int y) {
	xy_pen_up(p);
	POSITION buff = pr_get_moving_buffer(p);
	pr_set_moving_buffer(p, buff.x + x, buff.y + y);
}

/* Vector Absolute */
void xy_va(PRINTER *p, int x, int y) {
	//printf("xy_va X=%d, Y=%d\n", x, y);
	POSITION pos = pr_get_current_position(p);
	xy_pen_down(p);
	xy_move_rel(p, x - pos.x, y - pos.y);
}

/* Vector Relative */
void xy_vr(PRINTER *p, int x, int y) {
	xy_pen_down(p);
	xy_move_rel(p, x, y);
}

/* Point Absolute */
void xy_pa(PRINTER *p, int x, int y) {
	POSITION pos = pr_get_current_position(p);
	xy_pen_up(p);
	xy_move_rel(p, x - pos.x, y - pos.y);
	xy_pen_down(p);
	xy_pen_up(p);
}

/* Point Relative */
void xy_pr(PRINTER *p, int x, int y) {
	xy_pen_up(p);
	xy_move_rel(p, x, y);
	xy_pen_down(p);
	xy_pen_up(p);
}

/* Origin */
void xy_og(PRINTER *p, int x, int y) {
	pr_set_origin_position(p, x, y);
}

/* Circle */
void xy_cr(PRINTER *p, int r) {
	xy_ar(p, r, 0, 2 * M_PI);
}

/* Arcus */
void xy_ar(PRINTER *p, int r, double start_arc, double end_arc) {
	POSITION pos = pr_get_current_position(p);
	xy_og(p, pos.x, pos.y);

	int start_deg = start_arc * 180 / M_PI;
	int end_deg = end_arc * 180 / M_PI;

	int x = (double) r * sin(start_arc);
	int y = (double) r * cos(start_arc);
	xy_ma(p, x, y);
	xy_pen_down(p);

	int i;
	for (i = start_deg; i <= end_deg; i++) {
		x = (double) r * sin((double) i / 180 * M_PI);
		y = (double) r * cos((double) i / 180 * M_PI);
		xy_va(p, x, y);
	}

	xy_pen_up(p);
	xy_ma(p, 0, 0);
	xy_og(p, 0, 0);
}

/* Home */
void xy_hm(PRINTER *p) {
	xy_ma(p, 0, 0);
	xy_flush_moving_buffer(p);
}

/* Just Move */
static void xy_move_rel(PRINTER *p, int x, int y) {
	//printf("xy_move_rel X=%d, Y=%d\n", x, y);

	double x_step = 0;
	double y_step = 0;
	int ax = abs(x);
	int ay = abs(y);

	if (ax > ay) {
		if (y != 0) {
			x_step = (double) ax / ay;
			y_step = 1;
		} else {
			x_step = ax;
			y_step = 0;
		}
	} else {
		if (x != 0) {
			x_step = 1;
			y_step = (double) ay / ax;
		} else {
			x_step = 0;
			y_step = ay;
		}
	}

	int i = 0;
	while ((ax != 0) || (ay != 0)) {
		i++;

		/* move x */
		int step = (x_step * i) - (abs(x) - ax);
		step = (ax >= step) ? step : ax;
		pr_move(p, 1, ((x > 0) ? 1 : 0), step);
		ax -= step;

		/* move y */
		step = (y_step * i) - (abs(y) - ay);
		step = (ay >= step) ? step : ay;
		pr_move(p, 0, ((y > 0) ? 1 : 0), step);
		ay -= step;
	}
}

/* Pen up */
static void xy_pen_up(PRINTER *p) {
	pr_pen(p, 0);
}

/* Pen down */
static void xy_pen_down(PRINTER *p) {
	xy_flush_moving_buffer(p);
	pr_pen(p, 1);
}

/* Flush moving buffer */
static void xy_flush_moving_buffer(PRINTER *p) {
	POSITION buff = pr_get_moving_buffer(p);
	xy_move_rel(p, buff.x, buff.y);
	pr_set_moving_buffer(p, 0, 0);
}
