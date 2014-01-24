/*
*	printer.h
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#ifndef _PRINTER_H
#define _PRINTER_H

typedef struct {
	int			x;
	int			y;
} POSITION;

typedef unsigned char DATA;

typedef struct {
	int			parport_fd;
	POSITION	max_position;
	POSITION	curr_position;
	POSITION	origin_position;
	POSITION	moving_buffer;
	DATA		data;
} PRINTER;

/*
*	Create printer instance
*/
extern PRINTER *pr_create_printer(char *device_name);

/*
*	Close printer instance
*/
extern void pr_close_printer(PRINTER *p);

/*
*	Init printer
*/
extern void pr_init(PRINTER *p);

/*
*	Returns current position;
*/
extern POSITION pr_get_current_position(PRINTER *p);

/*
*	Returns origin position;
*/
extern POSITION pr_get_origin_position(PRINTER *p);

/*
*	Set origin position;
*/
extern void pr_set_origin_position(PRINTER *p, int x, int y);

/*
*	Returns moving buffer;
*/
extern POSITION pr_get_moving_buffer(PRINTER *p);

/*
*	Set moving buffer;
*/
extern void pr_set_moving_buffer(PRINTER *p, int x, int y);

/*
*	Set pen
*	0 = UP, 1 = DOWN
*/
extern void pr_pen(PRINTER *p, int value);

/*
*	Move
*	xy		0 = CART (Y), 1 = PAPER (X)
*	direction	0 = TO LEFT (FORWARD), 1 = TO RIGHT (BACK)
*	repeat		1 step = 0.1 mm
*/
extern void pr_move(PRINTER *p, int xy, int direction, int repeat);

#endif
