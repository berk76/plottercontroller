# xy1450 (C) Jaroslav Beran, 2014
# 

xy1450: src/main.o src/graph.o src/printer.o src/parport.o src/text.o
	$(CC) -Wall -ansi -pedantic -lm -o $@ src/main.o src/graph.o src/printer.o src/parport.o src/text.o
	cp var/font1.fnt ./font1.fnt

src/main.o: src/main.c src/main.h src/graph.h src/printer.h
src/printer.o: src/printer.c src/printer.h src/parport.h
src/graph.o: src/graph.c src/graph.h src/printer.h
src/parport.o: src/parport.c src/parport.h
src/text.o: src/text.c src/text.h src/graph.h src/printer.h

clean:
	rm -f src/*.o src/*.bak xy1450 xy1450.exe *.bak *.fnt

