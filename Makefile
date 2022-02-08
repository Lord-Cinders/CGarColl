FLAGS = -g
INCLUDE = -I ./include
OBJECTS = ./build/mld.o

all: $(OBJECTS)
	gcc $(FLAGS) $(INCLUDE) ./src/main.c $(OBJECTS) -o ./bin/main 

./build/mld.o: ./src/mld.c
	gcc $(FLAGS) $(INCLUDE) ./src/mld.c -c -o ./build/mld.o

clean:
	del build\*

