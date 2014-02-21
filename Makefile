# PlotterController (C) Jaroslav Beran, 2014
# 

plotter_controller: src/main.o src/graph.o src/printer.o src/parport.o src/text.o src/hpgl.o
	$(CC) -Wall -ansi -pedantic -o $@ src/main.o src/graph.o src/printer.o src/parport.o src/text.o src/hpgl.o -lm
	cp var/font1.fnt ./font1.fnt

src/main.o: src/main.c src/main.h src/graph.h src/printer.h src/text.h src/hpgl.h
src/printer.o: src/printer.c src/printer.h src/parport.h
src/graph.o: src/graph.c src/graph.h src/printer.h
src/parport.o: src/parport.c src/parport.h
src/text.o: src/text.c src/text.h src/graph.h src/printer.h
src/hpgl.o: src/hpgl.c src/hpgl.h src/graph.h src/printer.h

clean:
	rm -f src/*.o src/*.bak plotter_controller plotter_controller.exe *.bak *.fnt

