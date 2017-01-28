
CC = clang++
CFLAGS = -g -Wall 

# compiling rules

# WARNING: *must* have a tab before each definition
example: main.o ObjectManager.o
	$(CC) $(CFLAGS) -o test main.o ObjectManager.o

main.o: main.c ObjectManager.h
	$(CC) $(CFLAGS) -c main.c

ObjectManager.o: ObjectManager.c ObjectManager.h
	$(CC) $(CFLAGS) -c ObjectManager.c

clean:
	rm -f test main.o ObjectManager.o