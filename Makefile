#       Makefile
#       
#       This file is part of PlotterController project.
#       https://github.com/berk76/plottercontroller
#       
#       PlotterController is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 3 of the License, or
#       (at your option) any later version. <http://www.gnu.org/licenses/>
#       
#       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 24.1.2014  


CFLAGS = -O2 -Wall -c -o $@
objects = src/main.o src/graph.o src/printer.o src/par_port.o src/gpio_port.o src/gpio_io.o src/text.o src/hpgl.o

plotter_controller: ${objects}
	$(CC) -Wall -o $@ ${objects} -lm
	cp var/font1.fnt ./font1.fnt

main.o: src/main.c src/main.h src/graph.h src/printer.h src/text.h src/hpgl.h
printer.o: src/printer.c src/printer.h src/par_port.h src/gpio_port.h
graph.o: src/graph.c src/graph.h src/printer.h
par_port.o: src/par_port.c src/par_port.h
gpio_port.o: src/gpio_port.c src/gpio_port.h src/gpio_io.h
gpio_io.o: src/gpio_io.c src/gpio_io.h
text.o: src/text.c src/text.h src/graph.h src/printer.h
hpgl.o: src/hpgl.c src/hpgl.h src/graph.h src/printer.h

clean:
	rm -f ${objects} src/*.bak plotter_controller plotter_controller.exe *.bak *.fnt

