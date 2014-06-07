/*
*	printer.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

/*
 *					PLOTTER
 *		PC-printer port        XY4131	XY4140 	      XY4150
 *	bit 2	 4 ------------------ 1 ----------- 1 ----------- 1	PEN
 *	bit 3 	 5 ------------------ 2 ----------- 2 ----------- 2	STEP
 *	bit 1	 3 ------------------ 3 ----------- 3 ----------- 3	X/Y
 *	bit 0	 2 ------------------ 4 ----------- 4 ----------- 4	+/-
 *	bit 4	 6 ------------------ 5 ----------- 5 ----------- 5	READY
 *	GND		18 ------------------ 6 ----------- 6 ----------- 6	GND
 *
 *	The parallel port (or printer port, IEEE1284 standard) has 8 data lines
 *	
 *	Pin	Function	Dir
 *	1	/STROBE	W
 *	2..9	DATA Bit0..7	R/W
 *	10	ACK	R
 *	11	/BUSY	R
 *	12	PAPEROUT	R
 *	13	SELECT	R
 *	14	/AUTOFD	W
 *	15	ERROR	R
 *	16	INIT	W
 *	17	/SELECT	W
 *	18..25	GND	-
 *
 *	Signals with a / prefix are low-active (i.e. writing logic 1 to them 
 *	results in 0 Volt output, writing logic 0 results in 5 volts on the output).
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "printer.h"
#include "parport.h"

#define PEN_BIT 2
#define STEP_BIT 3
#define XY_BIT 1		/* L = CART, H = PAPER */
#define DIRECTION_BIT 0		/* L = TO LEFT (FORWARD), H = TO RIGHT (BACK) */
#define READY_BIT 4		/* L = READY, H = NOT READY */

#define MAX_X 2600
#define MAX_Y 1850

#define check_bit(var,pos) ((var) & (1<<(pos)))

/*
 *	Speed value is sleep interval in ms
 *	There are 10 values (0 the slowes 9 the fasts)
 */
static int speed[] = {2600,2400,2200,2000,1800,1600,1400,1200,1000,800};

static void dirty_pen(PRINTER *p, int value);
static int is_ready(PRINTER *p);
static void step(PRINTER *p, int repeat);
static void dirty_step(PRINTER *p);
static void switch_x_y(PRINTER *p, int value);
static void switch_direction(PRINTER *p, int value);
static int check_cross_limits(PRINTER *p);
static void sync_virt_curr_position(PRINTER *p);
static void set_bit(DATA *data, int pos, int value);


/*
*	Create printer instance
*/
PRINTER *pr_create_printer(char *device_name) {
	PRINTER *result;
	int fd;

	if ((fd = open_parport(device_name)) == 0) {
		return NULL;
	}

	result = (PRINTER *) malloc(sizeof(PRINTER));

	result->parport_fd = fd;
	result->max_position.x = MAX_X;
	result->max_position.y = MAX_Y;
	result->curr_position.x = 0;
	result->curr_position.y = 0;
	result->origin_position.x = 0;
	result->origin_position.y = 0;
	result->moving_buffer.x = 0;
	result->moving_buffer.y = 0;
	result->virtual_position.x = 0;
	result->virtual_position.y = 0;
	result->virtual_pen = 0;
	result->out_of_limits = 0;
	result->data = 0;
	result->velocity = 8;

	return result;
}


/*
*	Close printer instance
*/
void pr_close_printer(PRINTER *p) {
	if (p != NULL) {
		free((void *) p);
		p = NULL;
	}
}


/*
*	Init printer
*/
void pr_init(PRINTER *p) {
	p->data = 0;
	set_bit(&(p->data), READY_BIT, 1);
	set_bit(&(p->data), STEP_BIT, 1);
	write_data(p->parport_fd, p->data);
	USLEEP(speed[p->velocity]);

	switch_direction(p, 0);

	if (!is_ready(p)) {
		int i;
		while (!is_ready(p)) {
			dirty_step(p);
		}

		switch_direction(p, 1);
		for (i = 0; i < 50; i++) {
			dirty_step(p);
		}
	}
}


/*
*	Returns max position;
*/
POSITION pr_get_max_position(PRINTER *p) {
	return p->max_position;
}


/*
*	Returns current position;
*/
POSITION pr_get_current_position(PRINTER *p) {
	POSITION result;
	result.x =  p->virtual_position.x;
	result.y =  p->virtual_position.y;
	result.x -= p->origin_position.x;
	result.y -= p->origin_position.y;
	result.x += p->moving_buffer.x; 
	result.y += p->moving_buffer.y;
	return result;
}


/*
*	Returns origin position;
*/
POSITION pr_get_origin_position(PRINTER *p) {
	return p->origin_position;
}


/*
*	Set origin position;
*/
void pr_set_origin_position(PRINTER *p, int x, int y) {
	p->origin_position.x = x;
	p->origin_position.y = y;
}


/*
*	Returns moving buffer;
*/
POSITION pr_get_moving_buffer(PRINTER *p) {
	return p->moving_buffer;
}


/*
*	Set moving buffer;
*/
void pr_set_moving_buffer(PRINTER *p, int x, int y) {
	p->moving_buffer.x = x;
	p->moving_buffer.y = y;
}


/*
*	Set velocity (0-9)
*/
void pr_set_velocity(PRINTER *p, int v) {
	if ((v >= 0) && (v < 10)) {
		p->velocity = v;
	}
}


/*
*	Set pen
*/
void pr_pen(PRINTER *p, int value) {
	if (p->virtual_pen != value) {
		p->virtual_pen = value;
		if (!p->out_of_limits) dirty_pen(p, value);
	}
}


/*
*	Set dirty pen
*/
static void dirty_pen(PRINTER *p, int value) {
	set_bit(&(p->data), PEN_BIT, value);
	write_data(p->parport_fd, p->data);
	USLEEP(speed[p->velocity] * 10);
}


/*
*	Move
*	xy		0 = CART (Y), 1 = PAPER (X)
*	direction	0 = TO LEFT (FORWARD), 1 = TO RIGHT (BACK)
*	repeat		1 step = 0.1 mm
*/
void pr_move(PRINTER *p, int xy, int direction, int repeat) {
	switch_x_y(p, xy);
	switch_direction(p, direction);
	step(p, repeat);
}


/*
*	Is ready test
*/
static int is_ready(PRINTER *p) {
	DATA data;

	set_bit(&(p->data), READY_BIT, 0);
	write_data(p->parport_fd, p->data);
	USLEEP(speed[p->velocity]);

	set_bit(&(p->data), READY_BIT, 1);
	write_data(p->parport_fd, p->data);
	USLEEP(speed[p->velocity]);

	read_data(p->parport_fd, &data);
	return (check_bit(data, READY_BIT) == 0);
}


/*
*	Do step
*/
static void step(PRINTER *p, int repeat) {
	int step_dir;
	int i;

	step_dir = (check_bit(p->data, DIRECTION_BIT) == 0) ? -1 : 1;
	for (i = 0; i < repeat; i++) {
		
		if (check_bit(p->data, XY_BIT) == 0) {
			p->virtual_position.y += step_dir;
			if (!check_cross_limits(p) && !p->out_of_limits) {
				p->curr_position.y += step_dir;
				dirty_step(p);
			}
		} else {
			p->virtual_position.x += step_dir;
			if (!check_cross_limits(p) && !p->out_of_limits) {
				p->curr_position.x += step_dir;
				dirty_step(p);
			}
		}
	}
}


/*
*	Perform dirty step
*/
static void dirty_step(PRINTER *p) {
	set_bit(&(p->data), STEP_BIT, 0);
	write_data(p->parport_fd, p->data);
	USLEEP(speed[p->velocity]);
	set_bit(&(p->data), STEP_BIT, 1);
	write_data(p->parport_fd, p->data);
	USLEEP(speed[p->velocity]);
}


/*
*	Set X/Y
*/
static void switch_x_y(PRINTER *p, int value) {
	set_bit(&(p->data), XY_BIT, value);
}


/*
*	Set +/-
*/
static void switch_direction(PRINTER *p, int value) {
	set_bit(&(p->data), DIRECTION_BIT, value);
}


/*
*	Check cross limits
*/
static int check_cross_limits(PRINTER *p) {
	int previous;
	int current;

	previous = p->out_of_limits;
	current = (p->virtual_position.x > p->max_position.x) 
			|| (p->virtual_position.x < 0)
			|| (p->virtual_position.y > p->max_position.y)
			|| (p->virtual_position.y < 0);

	if (previous < current) {
		p->out_of_limits = 1;
		dirty_pen(p, 0);
		return 1;
	}

	if (previous > current) {
		p->out_of_limits = 0;
		sync_virt_curr_position(p);
		return 1;
	}

	return 0;
}


/*
*	Move
*	xy		0 = CART (Y), 1 = PAPER (X)
*	direction	0 = TO LEFT (FORWARD), 1 = TO RIGHT (BACK)
*	repeat		1 step = 0.1 mm
*/
static void sync_virt_curr_position(PRINTER *p) {
	DATA data;
	int i;

	data = p->data;

	if (p->curr_position.x != p->virtual_position.x) {
		switch_x_y(p, 1);
		switch_direction(p, (p->curr_position.x < p->virtual_position.x));
		for (i = 0; i < abs(p->virtual_position.x - p->curr_position.x); i++) {
			dirty_step(p);
		}
		p->curr_position.x = p->virtual_position.x;
	}

	if (p->curr_position.y != p->virtual_position.y) {
		switch_x_y(p, 0);
		switch_direction(p, (p->curr_position.y < p->virtual_position.y));
		for (i = 0; i < abs(p->virtual_position.y - p->curr_position.y); i++) {
			dirty_step(p);
		}
		p->curr_position.y = p->virtual_position.y;
	}

	p->data = data;
	dirty_pen(p, p->virtual_pen);
}


/*
*	Set bit
*/
static void set_bit(DATA *data, int pos, int value) {
	if (value == 0) {
		*data &= ~(1<<pos);
	} else {
		*data |= (1<<pos);
	}
}
