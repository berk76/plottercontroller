/*
*	gpio_port.h
*	12.10.2015
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#ifndef _GPIO_PORT_H
#define _GPIO_PORT_H

/*
*	Initialize gpio
*/
extern int gpio_open(int version);

/*
*	Closes gpio
*/
extern int gpio_close();

/*
*	Set pen
*/
extern void gpio_set_pen(int i);

/*
*	Set step
*/
extern void gpio_set_step(int i);

/*
*	Set/Get x/y
*	L = CART, H = PAPER
*/
extern void gpio_set_xy(int i);
extern int gpio_is_xy();

/*
*	Set/Get +/-
*	L = TO LEFT (FORWARD), H = TO RIGHT (BACK)
*/
extern void gpio_set_plus_minus(int i);
extern int gpio_is_plus_minus();

/*
*	Set ready
*	L = READY, H = NOT READY
*/
extern void gpio_set_ready(int i);

/*
*	Get ready
*/
extern int gpio_is_ready();

#endif
