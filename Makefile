OBJECTS=main.o screen.o util.o buffer.o
EXECUTABLE=lydl
FLAGS=-std=c99 # If your compiler is from the 80s, you can use -std=c89.
			   # The code should be compatible with -std=89.

all: $(OBJECTS)
	cc $(OBJECTS) $(FLAGS) -lncurses -o $(EXECUTABLE)
	rm $(OBJECTS)

main.o: src/main.c screen.o
	cc -c $(FLAGS) src/main.c

screen.o: src/screen.c util.o
	cc -c $(FLAGS) src/screen.c

util.o: src/util.c screen.o
	cc -c $(FLAGS) src/util.c

buffer.o: src/buffer.c
	cc -c $(FLAGS) src/buffer.c

