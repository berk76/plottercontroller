/*
*       text.h
*       
*       This file is part of PlotterController project.
*       https://github.com/berk76/plottercontroller
*       
*       PlotterController is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 3 of the License, or
*       (at your option) any later version. <http://www.gnu.org/licenses/>
*       
*       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 26.1.2014  
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
