/*
*	printer.h
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#ifndef _PRINTER_H
#define _PRINTER_H

/* Type of interface */
typedef enum {
        PARPORT,
        GPIO
} interface_t;

/* Pen */
typedef enum {
        UP   = 0,
        DOWN = 1
} value_pen_t;

/* Motor X/Y */
typedef enum {
        CART  = 0,
        PAPER = 1
} value_xy_t;

/* Direction */
typedef enum {
        BACKWARD = 0,
        FORWARD  = 1
} value_direction_t;

/* Ready */
typedef enum {
        READY   = 0,
        NOREADY = 1
} value_ready_t;

/* Position */
typedef struct {
	int    x;
	int    y;
} POSITION;

/* Printer */
typedef struct {
	POSITION               max_position;
	POSITION               curr_position;
	POSITION               origin_position;
	POSITION               moving_buffer;
	POSITION               virtual_position;
	int                    virtual_pen;
	int                    out_of_limits;
	int                    velocity;
        int                    (*close)();
        void                   (*set_pen)(int);
        void                   (*set_step)(int);
        void                   (*set_xy)(int);
        int                    (*is_xy)();
        void                   (*set_plus_minus)(int);
        int                    (*is_plus_minus)();
        void                   (*set_ready)(int);
        int                    (*is_ready)();
} PRINTER;


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
extern PRINTER *pr_create_printer(interface_t i, char *param);

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
*	Returns current POSITION;
*/
extern POSITION pr_get_current_position(PRINTER *p);

/*
*	Returns origin POSITION;
*/
extern POSITION pr_get_origin_position(PRINTER *p);

/*
*	Set origin POSITION;
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
*/
extern void pr_pen(PRINTER *p, value_pen_t value);

/*
*	Move
*	repeat		1 step = 0.1 mm
*/
extern void pr_move(PRINTER *p, value_xy_t xy, value_direction_t direction, int repeat);

#endif

