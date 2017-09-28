/*
*       graph.h
*       
*       This file is part of PlotterController project.
*       https://github.com/berk76/plottercontroller
*       
*       PlotterController is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 3 of the License, or
*       (at your option) any later version. <http://www.gnu.org/licenses/>
*       
*       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 10.1.2014  
*/


#ifndef _GRAPH_H
#define _GRAPH_H

typedef struct {
	double x;
	double y;
} D_POSITION;

#ifndef M_PI
#define M_PI		3.14159265358979323846	/* pi */
#endif

#ifndef M_PI_2
#define M_PI_2		1.57079632679489661923	/* pi/2 */
#endif

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

/* Set Velocity (0 - 9) */
extern void xy_vs(PRINTER *p, int v);

/* Transforms position  */
extern D_POSITION _transform_position(double x, double y, double angle);

#endif
