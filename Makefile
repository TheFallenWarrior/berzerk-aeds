# Makefile para 'main.c'

CFLAGS := -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: run

berzerk: main.c
	@gcc $(CFLAGS) main.c  -o berzerk

run: berzerk
	@./berzerk
