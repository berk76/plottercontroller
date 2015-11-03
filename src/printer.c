/*
*	printer.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/


#include <stdio.h>
#include <stdlib.h>
#include "printer.h"
#include "par_port.h"
#include "gpio_port.h"


#define MAX_X 2600
#define MAX_Y 1850

/*
*	usleep macro
*/

#if defined(__linux__) || defined(__FreeBSD__) 
#include <unistd.h>
#define USLEEP(t) (usleep(t))
#endif

#ifdef __TURBOC__
#include <dos.h>
#define USLEEP(t) (delay(t))
#endif

#ifdef _WIN32
extern void usleep_win (long usec);
#define USLEEP(t) (usleep_win(t))
#endif

/*
 *	Speed value is sleep interval in ms
 *	There are 10 values (0 slow 9 fast)
 */
static int speed[] = {2600,2400,2200,2000,1800,1600,1400,1200,1000,800};

static void dirty_pen(PRINTER *p, int value);
static int is_ready(PRINTER *p);
static void step(PRINTER *p, int repeat);
static void dirty_step(PRINTER *p);
static int check_cross_limits(PRINTER *p);
static void sync_virt_curr_position(PRINTER *p);


/*
*	Create printer instance
*/
PRINTER *pr_create_printer(enum INTERFACE i, char *param) {
	PRINTER *result;

        result = (PRINTER *) malloc(sizeof(PRINTER));
        
        switch (i) {
                case PARPORT:
                        if (par_open(param) != 0) {
                                fprintf(stderr, "Invalid param %s\n", param);
                                exit(1);
                        }
                        result->close = &par_close;
                        result->set_pen = &par_set_pen;
                        result->set_step = &par_set_step;
                        result->set_xy = &par_set_xy;
                        result->is_xy = &par_is_xy;
                        result->set_plus_minus = &par_set_plus_minus;
                        result->is_plus_minus = &par_is_plus_minus;
                        result->set_ready = &par_set_ready;
                        result->is_ready = &par_is_ready;
                        break;
                case GPIO:
                        if (gpio_open(atoi(param)) != 0) {
                                fprintf(stderr, "Invalid param %s\n", param);
                                exit(1);
                        }
                        result->close = &gpio_close;
                        result->set_pen = &gpio_set_pen;
                        result->set_step = &gpio_set_step;
                        result->set_xy = &gpio_set_xy;
                        result->is_xy = &gpio_is_xy;
                        result->set_plus_minus = &gpio_set_plus_minus;
                        result->is_plus_minus = &gpio_is_plus_minus;
                        result->set_ready = &gpio_set_ready;
                        result->is_ready = &gpio_is_ready;
                        break;
                default:
			fprintf(stderr, "Invalid interface %d\n", i);
                        exit(1);
        }	

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
        //par_close();
}


/*
*	Init printer
*/
void pr_init(PRINTER *p) {
        p->set_ready(0);
	p->set_xy(1);
	p->set_step(0);
        p->set_plus_minus(1);
	USLEEP(speed[p->velocity]);


	if (!is_ready(p)) {
		int i;
		while (!is_ready(p)) {
			dirty_step(p);
		}

                p->set_plus_minus(0);
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
        p->set_pen(!value);
	USLEEP(speed[p->velocity] * 10);
}


/*
*	Move
*	xy		0 = CART (Y), 1 = PAPER (X)
*	direction	0 = TO LEFT (FORWARD), 1 = TO RIGHT (BACK)
*	repeat		1 step = 0.1 mm
*/
void pr_move(PRINTER *p, int xy, int direction, int repeat) {
        p->set_xy(!xy);
        p->set_plus_minus(!direction);
	step(p, repeat);
}


/*
*	Is ready test
*/
static int is_ready(PRINTER *p) {

        p->set_ready(1);
	USLEEP(speed[p->velocity]);

	p->set_ready(0);
	USLEEP(speed[p->velocity]);
                
	return (p->is_ready() == 0);
}


/*
*	Do step
*/
static void step(PRINTER *p, int repeat) {
	int step_dir;
	int i;

	step_dir = (p->is_plus_minus() == 1) ? -1 : 1;
	for (i = 0; i < repeat; i++) {
		
		if (p->is_xy() == 1) {
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
	p->set_step(1);
	USLEEP(speed[p->velocity]);
	p->set_step(0);
	USLEEP(speed[p->velocity]);
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
	int i, xy, plus_minus;

	xy = p->is_xy();
        plus_minus = p->is_plus_minus();
        

	if (p->curr_position.x != p->virtual_position.x) {
                p->set_xy(0);
                p->set_plus_minus((p->curr_position.x < p->virtual_position.x));
		for (i = 0; i < abs(p->virtual_position.x - p->curr_position.x); i++) {
			dirty_step(p);
		}
		p->curr_position.x = p->virtual_position.x;
	}

	if (p->curr_position.y != p->virtual_position.y) {
		p->set_xy(1);
                p->set_plus_minus((p->curr_position.y < p->virtual_position.y));
		for (i = 0; i < abs(p->virtual_position.y - p->curr_position.y); i++) {
			dirty_step(p);
		}
		p->curr_position.y = p->virtual_position.y;
	}

	p->set_xy(xy);
	p->set_plus_minus(plus_minus);
	dirty_pen(p, p->virtual_pen);
}

