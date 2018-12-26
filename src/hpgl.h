/*
*       hpgl.h
*
*       This file is part of PlotterController project.
*       https://github.com/berk76/plottercontroller
*
*       PlotterController is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 3 of the License, or
*       (at your option) any later version. <http://www.gnu.org/licenses/>
*
*       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 8.2.2014
*/


#ifndef _HPGL_H
#define _HPGL_H


/* Draw hpgl file */
extern void hpgl_draw_from_file(PRINTER *p, char *file_name, double scale_factor);


#endif
