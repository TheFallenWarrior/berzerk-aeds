# Makefile para 'Berzerk'

CFLAGS := -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTPUT := dist/berzerk
SRC := $(shell find src/ -name *.c)

.PHONY: run clean commit

all: run

$(OUTPUT): $(SRC)
	@mkdir -p dist/
	@gcc $(SRC) $(CFLAGS) -o $(OUTPUT)

run: $(OUTPUT)
	@./$(OUTPUT)

clean:
	@rm $(OUTPUT)

commit:
	@git add --all
	@git commit
	@git push
