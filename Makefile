# Makefile para 'Berzerk'

CFLAGS := -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTPUT := dist/berzerk

.PHONY: run clean

all: run

$(OUTPUT): src/main.c
	mkdir -p dist/
	@gcc src/main.c $(CFLAGS) -o $(OUTPUT)

run: $(OUTPUT)
	@./$(OUTPUT)

clean:
	@rm $(OUTPUT)

commit:
	@git add --all
	@git commit
	@git push
