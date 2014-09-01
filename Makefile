OBJECTS=main.o screen.o util.o
EXECUTABLE=lydl

all: $(OBJECTS)
	cc $(OBJECTS) -lncurses -o $(EXECUTABLE)
	rm $(OBJECTS)

main.o: src/main.c screen.o
	cc -c src/main.c

screen.o: src/screen.c util.o
	cc -c src/screen.c

util.o: src/util.c screen.o
	cc -c src/util.c