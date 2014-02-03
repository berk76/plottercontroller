/*
*	text.h
*	26.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#ifndef _TEXT_H
#define _TEXT_H


/* Set font size */
extern void xy_set_font_size(int i);

/* Set text angle (in radians)*/
extern void xy_set_text_angle(double angle);

/* Write text */
extern void xy_write(PRINTER *p, char *text);

#endif
