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
	POSITION	max_position;
	POSITION	curr_position;
	POSITION	origin_position;
	POSITION	moving_buffer;
	POSITION	virtual_position;
	int		virtual_pen;
	int		out_of_limits;
	int		velocity;
        int (*close)();
        void (*set_pen)(int i);
        void (*set_step)(int i);
        void (*set_xy)(int i);
        int (*is_xy)();
        void (*set_plus_minus)(int i);
        int (*is_plus_minus)();
        void (*set_ready)(int i);
        int (*is_ready)();
} PRINTER;

enum INTERFACE {
        PARPORT,
        GPIO
};

/*
*	Create printer instance
*	
*	Linux
*       pr_create_printer(PARPORT, "/dev/parport0")
*       FreeBSD
*       pr_create_printer(PARPORT, "/dev/ppi0")
*       DOS
*       pr_create_printer(PARPORT, "0x378")
*       RPi v1
*       pr_create_printer(GPIO, "1")
*       RPi v2
*       pr_create_printer(GPIO, "2")
*/
extern PRINTER *pr_create_printer(enum INTERFACE i, char *param);

/*
*	Close printer instance
*/
extern void pr_close_printer(PRINTER *p);

/*
*	Init printer
*/
extern void pr_init(PRINTER *p);

/*
*	Returns max position;
*/
extern POSITION pr_get_max_position(PRINTER *p);


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
*	Set velocity (0-9)
*/
extern void pr_set_velocity(PRINTER *p, int v);

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
