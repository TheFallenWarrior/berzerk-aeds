# Makefile para 'Berzerk'

CFLAGS := -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTPUT := berzerk

.PHONY: run clean

all: run

berzerk: main.c
	@gcc main.c $(CFLAGS) -o $(OUTPUT)

run: $(OUTPUT)
	@./$(OUTPUT)

clean:
	@rm $(OUTPUT)
