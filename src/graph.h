/*
*	graph.h
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#ifndef _GRAPH_H
#define _GRAPH_H

typedef struct {
	double x;
	double y;
} D_POSITION;


/* Move Absolute */
extern void xy_ma(PRINTER *p, int x, int y);

/* Move Relative */
extern void xy_mr(PRINTER *p, int x, int y);

/* Vector Absolute */
extern void xy_va(PRINTER *p, int x, int y);

/* Vector Relative */
extern void xy_vr(PRINTER *p, int x, int y);

/* Point Absolute */
extern void xy_pa(PRINTER *p, int x, int y);

/* Point Relative */
extern void xy_pr(PRINTER *p, int x, int y);

/* Origin */
extern void xy_og(PRINTER *p, int x, int y);

/* Circle */
extern void xy_cr(PRINTER *p, int r);

/* Arcus */
extern void xy_ar(PRINTER *p, int r, double start_arc, double end_arc);

/* Home */
extern void xy_hm(PRINTER *p);

/* Transforms position  */
extern D_POSITION _transform_position(double x, double y, double angle);

#endif
