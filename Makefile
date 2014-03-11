# PlotterController (C) Jaroslav Beran, 2014
# 

CFLAGS = -O2 -Wall -c -o $@
objects = src/main.o src/graph.o src/printer.o src/parport.o src/text.o src/hpgl.o

plotter_controller: ${objects}
	$(CC) -Wall -o $@ ${objects} -lm
	cp var/font1.fnt ./font1.fnt

main.o: src/main.c src/main.h src/graph.h src/printer.h src/text.h src/hpgl.h
printer.o: src/printer.c src/printer.h src/parport.h
graph.o: src/graph.c src/graph.h src/printer.h
parport.o: src/parport.c src/parport.h
text.o: src/text.c src/text.h src/graph.h src/printer.h
hpgl.o: src/hpgl.c src/hpgl.h src/graph.h src/printer.h

clean:
	rm -f ${objects} src/*.bak plotter_controller plotter_controller.exe *.bak *.fnt

