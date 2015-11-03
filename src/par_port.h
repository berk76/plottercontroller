/*
*	par_port.h
*	13.10.2015
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#ifndef _PAR_PORT_H
#define _PAR_PORT_H

/*
*	Initialize port
*/
extern int par_open(char *device);

/*
*	Closes port
*/
extern int par_close();

/*
*	Set pen
*/
extern void par_set_pen(int i);

/*
*	Set step
*/
extern void par_set_step(int i);

/*
*	Set/Get x/y
*	L = CART, H = PAPER
*/
extern void par_set_xy(int i);
extern int par_is_xy();

/*
*	Set/Get +/-
*	L = TO LEFT (FORWARD), H = TO RIGHT (BACK)
*/
extern void par_set_plus_minus(int i);
extern int par_is_plus_minus();

/*
*	Set ready
*	L = READY, H = NOT READY
*/
extern void par_set_ready(int i);

/*
*	Get ready
*/
extern int par_is_ready();

#endif
