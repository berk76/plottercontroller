/*
*	graph.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "printer.h"
#include "graph.h"

static void xy_move_rel(PRINTER *p, int x, int y);
static void xy_pen_up(PRINTER *p);
static void xy_pen_down(PRINTER *p);
static void xy_flush_moving_buffer(PRINTER *p);

/* Move Absolute */
void xy_ma(PRINTER *p, int x, int y) {
	POSITION pos;
	pos = pr_get_current_position(p);
	xy_mr(p, x - pos.x, y - pos.y);
}

/* Move Relative */
void xy_mr(PRINTER *p, int x, int y) {
	POSITION buff;
	xy_pen_up(p);
	buff = pr_get_moving_buffer(p);
	pr_set_moving_buffer(p, buff.x + x, buff.y + y);
}

/* Vector Absolute */
void xy_va(PRINTER *p, int x, int y) {
	POSITION pos;
	pos = pr_get_current_position(p);
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
	POSITION pos;
	pos = pr_get_current_position(p);
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
	POSITION pos;
	int start_deg, end_deg;
	int x, y;
	int i;

	pos = pr_get_current_position(p);
	xy_og(p, pos.x, pos.y);

	start_deg = start_arc * 180 / M_PI;
	end_deg = end_arc * 180 / M_PI;

	x = (double) r * cos(start_arc);
	y = (double) r * sin(start_arc);
	xy_ma(p, x, y);
	xy_pen_down(p);

	for (i = start_deg; i <= end_deg; i++) {
		x = (double) r * cos((double) i / 180 * M_PI);
		y = (double) r * sin((double) i / 180 * M_PI);
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

/* Set Velocity (0 - 9) */
void xy_vs(PRINTER *p, int v) {
	xy_flush_moving_buffer(p);
	pr_set_velocity(p, v);
}

/* Transforms position */
D_POSITION _transform_position(double x, double y, double angle) {
	D_POSITION result;
	result.x = x * cos(angle) + y * cos(angle + M_PI_2);
	result.y = y * cos(angle) + x * cos(angle - M_PI_2);
	return result;
}


/* Just Move */
static void xy_move_rel(PRINTER *p, int x, int y) {
	double x_step, y_step;
	int ax, ay;
	int i;
	int step;

	x_step = 0;
	y_step = 0;
	ax = abs(x);
	ay = abs(y);

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

	i = 0;
	while ((ax != 0) || (ay != 0)) {
		i++;

		/* move x */
		step = (x_step * i) - (abs(x) - ax);
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
