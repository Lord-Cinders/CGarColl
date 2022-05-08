FLAGS = -g
INCLUDE = -I ./include
OBJECTS = ./build/mld.o

./build/mld.o: ./src/mld.c
	gcc $(FLAGS) $(INCLUDE) ./src/mld.c -c -o ./build/mld.o

clean:
	del build\*

