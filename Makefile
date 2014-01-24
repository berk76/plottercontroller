# xy1450 (C) Jaroslav Beran, 2014
# 

xy1450: src/main.o src/graph.o src/printer.o src/parport.o
	$(CC) -lm -o $@ src/main.o src/graph.o src/printer.o src/parport.o

src/main.o: src/main.c src/main.h src/graph.h src/printer.h
src/printer.o: src/printer.c src/printer.h src/parport.h
src/graph.o: src/graph.c src/graph.h src/printer.h
src/parport.o: src/parport.c src/parport.h

clean:
	rm -f src/*.o src/*.bak xy1450 xy1450.exe *.bak

