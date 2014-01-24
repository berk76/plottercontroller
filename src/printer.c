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
#define DIRECTION_BIT 0	/* L = TO LEFT (FORWARD), H = TO RIGHT (BACK) */
#define READY_BIT 4		/* L = READY, H = NOT READY */

#define SLEEP_INTERVAL 		1000	/* in ms */
#define SLEEP_INTERVAL_LOW	10000

#define MAX_X 2600
#define MAX_Y 1850

#define check_bit(var,pos) ((var) & (1<<(pos)))

static int is_ready(PRINTER *p);
static void step(PRINTER *p, int repeat);
static void dirty_step(PRINTER *p);
static void switch_x_y(PRINTER *p, int value);
static void switch_direction(PRINTER *p, int value);
static void print_port_status(PRINTER *p);
static void set_bit(DATA *data, int pos, int value);

/*
*	Create printer instance
*/
PRINTER *pr_create_printer(char *device_name) {
	int fd;

	if ((fd = open_parport(device_name)) == 0) {
        fprintf(stderr, "Error: Probably you don't have permission access %s\n", device_name);
        return NULL;
    }

	PRINTER *result = (PRINTER *) malloc(sizeof(PRINTER));

	result->parport_fd = fd;
	result->max_position.x = 1750;
	result->max_position.y = 2100;
	result->curr_position.x = 0;
	result->curr_position.y = 0;
	result->origin_position.x = 0;
	result->origin_position.y = 0;
	result->moving_buffer.x = 0;
	result->moving_buffer.y = 0;
	result->data = 0;

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
	usleep(SLEEP_INTERVAL_LOW);

	switch_direction(p, 0);

	if (!is_ready(p)) {
        	int i;
        	for (i = 0; i < MAX_Y; i++) {
                	if (is_ready(p))
                        	break;
			dirty_step(p);
        	}

		switch_direction(p, 1);
		for (i = 0; i < 50; i++) {
			dirty_step(p);
		}
	}
}

/*
*	Returns current position;
*/
POSITION pr_get_current_position(PRINTER *p) {
	POSITION result;
	result.x =  p->curr_position.x;
	result.y =  p->curr_position.y;
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
	POSITION result;
	result.x = p->origin_position.x;
	result.y = p->origin_position.y;
	return result;
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
	POSITION result;
	result.x = p->moving_buffer.x;
	result.y = p->moving_buffer.y;
	return result;
}

/*
*	Set moving buffer;
*/
void pr_set_moving_buffer(PRINTER *p, int x, int y) {
	p->moving_buffer.x = x;
	p->moving_buffer.y = y;
}

/*
*	Set pen
*/
void pr_pen(PRINTER *p, int value) {
	set_bit(&(p->data), PEN_BIT, value);
	write_data(p->parport_fd, p->data);
	usleep(SLEEP_INTERVAL);
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
	set_bit(&(p->data), READY_BIT, 0);
	write_data(p->parport_fd, p->data);
	usleep(SLEEP_INTERVAL);

	set_bit(&(p->data), READY_BIT, 1);
	write_data(p->parport_fd, p->data);
	usleep(SLEEP_INTERVAL);

	DATA data;
	read_data(p->parport_fd, &data);
	return (check_bit(data, READY_BIT) == 0);
}

/*
*	Do step
*/
static void step(PRINTER *p, int repeat) {
	int i;
	for (i = 0; i < repeat; i++) {
		dirty_step(p);
		
		if (check_bit(p->data, XY_BIT) == 0) {
			if ((check_bit(p->data, DIRECTION_BIT) == 0)) {
				(p->curr_position.y)--;
			} else {
				(p->curr_position.y)++;
			}
		} else {
			if ((check_bit(p->data, DIRECTION_BIT) == 0)) {
				(p->curr_position.x)--;
			} else {
				(p->curr_position.x)++;
			}
		}
	}
}

static void dirty_step(PRINTER *p) {
	set_bit(&(p->data), STEP_BIT, 0);
        write_data(p->parport_fd, p->data);
        usleep(SLEEP_INTERVAL);
        set_bit(&(p->data), STEP_BIT, 1);
        write_data(p->parport_fd, p->data);
        usleep(SLEEP_INTERVAL);
}


/*
*	Set X/Y
*/
static void switch_x_y(PRINTER *p, int value) {
	set_bit(&(p->data), XY_BIT, value);
	//write_data(p->parport_fd, p->data);
	//usleep(SLEEP_INTERVAL);
}

/*
*	Set +/-
*/
static void switch_direction(PRINTER *p, int value) {
	set_bit(&(p->data), DIRECTION_BIT, value);
	//write_data(p->parport_fd, p->data);
	//usleep(SLEEP_INTERVAL);
}


/*
*	Print status
*/
static void print_port_status(PRINTER *p) {
	DATA data = p->data;
	//read_data(p->parport_fd, &data);

	printf("--status--\n");
	int i;
	for (i = 0; i < 8; i++) {
		printf("%d  ", i);
	}
	printf("\n");
	for (i = 0; i < 8; i++) {
		printf("%u  ", (check_bit(data, i) == 0) ? 0 : 1);
	}
	printf("\n");
	printf("----------\n");
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
