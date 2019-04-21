all: build

build: main.c
	gcc -Wall -ansi -pedantic main.c -lm -o priority_queue.out
