/*
*	parport.h
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#ifndef _PARPORT_H
#define _PARPORT_H

/*
*	Returns handler to parallel port
*/
extern int open_parport(char *device);

/*
*	Closes parallel port
*/
extern int close_parport(int fd);

/*
*	Writes byte to parallel port
*/
extern int write_data(int fd, unsigned char data);

/*
*	Reads byte from parallel port
*/
extern int read_data(int fd, unsigned char *data);

/*
*	usleep macro
*/
#ifdef __TURBOC__
#include <dos.h>
#define USLEEP(t) (delay(t))
#else
#include <unistd.h>
#define USLEEP(t) (usleep(t))
#endif

#endif
