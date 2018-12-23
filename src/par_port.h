/*
*       par_port.h
*
*       This file is part of PlotterController project.
*       https://github.com/berk76/plottercontroller
*
*       PlotterController is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 3 of the License, or
*       (at your option) any later version. <http://www.gnu.org/licenses/>
*
*       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 13.10.2015
*/


#ifndef _PAR_PORT_H
#define _PAR_PORT_H

/*
*       Initialize port
*/
extern int par_open(char *device);

/*
*       Closes port
*/
extern int par_close();

/*
*       Set pen
*/
extern void par_set_pen(int i);

/*
*       Set step
*/
extern void par_set_step(int i);

/*
*       Set/Get x/y
*       L = CART, H = PAPER
*/
extern void par_set_xy(int i);
extern int par_is_xy();

/*
*       Set/Get +/-
*       L = TO LEFT (FORWARD), H = TO RIGHT (BACK)
*/
extern void par_set_plus_minus(int i);
extern int par_is_plus_minus();

/*
*       Set ready
*       L = READY, H = NOT READY
*/
extern void par_set_ready(int i);

/*
*       Get ready
*/
extern int par_is_ready();

#endif
